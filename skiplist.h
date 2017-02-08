/*---------------------------------------------------------------------------------------------------------------------------------------------------

	Author:		Boyer, Destiny
	Project:	Skip List
	Professor:	Rob Nash
	Class:		CSS342

	This is a templatized class implementing the Skip List data structure. This implementation builds a Skip List with four levels and
	doubly-linked Nodes. Each level has a dummy head, and dummy tail Node. Each Node contains one object and pointers the Nodes above,
	below, before, and after it. When a new Node is created it is inserted in to the master level and has a 50% chance of being moved up
	and inserted in to the next level up. This reduces the average case big o of searching to O(logn). Searching the list starts at the top
	level where a check is performed to determine if the level is empty. If the level is empty the algorithm moves down to the next level.
	If the level is not empty each Node's data is compared to the target Object for equality. If a Node is encountered that has contains
	data greater than the target Object then the search moves back one Node, and down one level. If the bottom level is reached and a
	matching Node is not found, or if a Node is found at the bottom level that has data greater than the target Object then the Object
	is not in the list. All memory for Nodes is dynamically created and this class also has built in functionality to remove and deallocate
	a certain Node from the list, or delete all nodes in the List. 

	NOTE:	Objects are responsible for having overloaded equality operators for comparison. 

---------------------------------------------------------------------------------------------------------------------------------------------------*/

#pragma once
#ifndef skiplist_h
#define skiplist_h

//imports libraries
#include <iostream>
#include <cstdlib>
#include <stdlib.h>

using namespace std;

template<class Object>

class SkipList {

	private:

	struct SLNode {
		
		Object* data;		//Object data member
		SLNode* up;			//Node pointer to the Node above the Node
		SLNode* down;		//Node pointer to the Node below the Node
		SLNode* previous;	//Node pointer to the Node before the Node
		SLNode* next;		//Node pointer to the Node after the Node
		int level;				//level that the Node is on
		bool isDummy = false;	//bool value indicating if the Node is a dummy tail or dummy head

		//destructors for the Node. Default no-args constructor initializes all data members to NULL
		SLNode() : data(NULL), up(NULL), down(NULL), previous(NULL), next(NULL), level(NULL) {};
		SLNode(const Object& d, SLNode* u, SLNode* dow, SLNode* p, SLNode* n, int l) {
			data = new Object(d);
			up = u;
			down = dow;
			previous = p;
			next = n;
			level = l;
		
		};
	};

	public:

	SkipList(void);										//Default no-args contructor
	~SkipList(void);									//Destructor
	SkipList(const SkipList& toCopy);					//Copy constructor
	int size(void) const;								//returns size of the list
	void show(void) const;								//displays the list
	bool isEmpty(void) const;							//returns bool indicating if there are any Nodes in the lsit
	bool contains(const Object& target) const;			//returns true if the target object is in the list
	void operator=(const SkipList& toCopy);				//overloaded assignment operator
	bool operator==(const SkipList& toCompare) const;	//overloaded equality operator
	bool insert(const Object& toInsert);				//inserts a new Node with the parameter Object
	bool remove(const Object& toRemove);				//removes Node with data equal to the parameter Object
	void makeEmpty(void);								//deallocates any dynamically allocated memory in the list
	int getCost(void) const;							//returns int value counting number of operations

	private:

	SLNode* retrieve(const Object& target) const;		//done
	void initialize(void);								//dome
	bool moveUp(void) const;							//done
	void deleteNode(SLNode*& toClear);					//done
	void clear(void);									//done
	bool levelIsEmpty(const int currentLevel) const;	//done
	int cost;

	static const int LEVEL = 4;			//final number of levels
	SLNode* dummyHead[LEVEL];			//dynamic array for nodes
};

#endif // !skiplist_h


/*-------------------------------------------------------------------------------------------------
	
	Method retuns an int value equal to the number of operations it has taken to insert
	and retrieve from the list. Method cannot change any data members.

	POSTCONDTIONS:
		- returns cost

-------------------------------------------------------------------------------------------------*/

template<class Object>
int SkipList<Object>::getCost(void) const {
	return cost;
};

/*-------------------------------------------------------------------------------------------------

	Copy-constructor. Uses the overloaded assignment operator to make a deep-copy of the SkipList.

	NOTES:	Uses the overloaded assignment operator

-------------------------------------------------------------------------------------------------*/

template<class Object>
SkipList<Object>::SkipList(const SkipList& toCopy) {
	*this = toCopy;	//uses overloaded assignment operator to copy list
};

/*-------------------------------------------------------------------------------------------------

	Method prints the contents of the Skip List to the console. Levels of the Skip List and the
	dummy nodes are indicated by "+inf" and "-inf". Method cannot change any data members.

-------------------------------------------------------------------------------------------------*/

template<class Object>
void SkipList<Object>::show(void) const {
	cout << "contents:" << endl;		//prints header for the Skip List
	for (SLNode* col = dummyHead[0]; col != NULL; col = col->next) {	//iterates over each level in the list
		SLNode* row = col;
		for (int level = 0; row != NULL && level < LEVEL; level++) {	//iterates over each Node in the level
			if (row->previous == NULL) { 
				cout << "-inf\t";		//prints the dummy header
			}
			else if (row->next == NULL) {
				cout << "+inf\t";		//prints the dummy tail
			}
			else {
				cout << *row->data << "\t";	//prints the data of the Node
			}
			row = row->up;					//moves to the next level of the list
		}
		cout << endl;
	}
};

/*-------------------------------------------------------------------------------------------------

	Constructor. Instantiates a SkipList object and calls initialize to create the dummy
	head and dummy tail nodes.

	POSTCONDITIONS:
		- instantiates new SkipList Object
		- initializes dummy head and dummy tail nodes

-------------------------------------------------------------------------------------------------*/

template<class Object>
SkipList<Object>::SkipList(void) {
	initialize();					//initializes dummy nodes for each level
	cost = 0;
};

/*-------------------------------------------------------------------------------------------------

	Destructor. Calls makeEmpty to deallocate all memory allocated for Nodes that have
	been inserted in to the list. Deletes all dummy head and dummy tail nodes.

	POSTCONDITIONS:
		- deallocates all dynamically allocated memory

-------------------------------------------------------------------------------------------------*/

template<class Object>
SkipList<Object>::~SkipList(void) {
	makeEmpty();	//calls makeEmpty
	//iterates over the levels in the list deleting
	//all dummy head and dummy tail nodes
	for (int i = 0; i < LEVEL; i++) {
		delete dummyHead[i]->next->data;
		delete dummyHead[i]->next;
		delete dummyHead[i]->data;
		delete dummyHead[i];
	}
};

/*-------------------------------------------------------------------------------------------------

	Method takes in an Object and inserts it in to the list. A new node is created to contain
	the object. If the Object is already in the list the method returns false. This method calls
	contains, retrieve, and moveUp as helper functions.

	POSTCONDITIONS:
		- inserts a new Node in to the list

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::insert(const Object& toInsert) {

	int currentLevel = 0;
	SLNode* nodeToInsert;
	SLNode* nodeBefore;
	SLNode* nodeAfter;
	bool success = false;

	//checks if the list is empty. If so inserts the Node in the master level between the two
	//dummy Nodes. Connects the dummy nodes to the new node
	if (isEmpty()) {
		
		nodeBefore = dummyHead[currentLevel];
		nodeAfter = dummyHead[currentLevel]->next;
		nodeToInsert = new SLNode(toInsert, NULL, NULL, nodeBefore, nodeAfter, currentLevel);
		
		nodeBefore->next = nodeToInsert;
		nodeAfter->previous = nodeToInsert;
		//sets success to true
		success = true;
	} else {	//otherwise we know that there are nodes in the list

		bool isInList = contains(toInsert);		//checks if the Object is already in the list

		if (!isInList) {
			nodeAfter = retrieve(toInsert);		//returns a pointer to the SLNode we want to insert our new SLNode before

			while (nodeAfter->level > 0) {				//while loop cycles through SLNodes until we are at the master level
				nodeAfter = nodeAfter->down;
			}

			nodeBefore = nodeAfter->previous;	//SLNode* to the node right before where we want to insert

			//creates new SLNode and connects it to the nodeBefore and nodeAfter
			nodeToInsert = new SLNode(toInsert, NULL, NULL, nodeBefore, nodeAfter, currentLevel++);

			nodeAfter->previous = nodeToInsert;			//connects previous node to new node
			nodeBefore->next = nodeToInsert;			//connects nodeAfter to newly inserted node
			success = true;
		} else {
			return false;
		}
	}
	//calls moveUp() to determine if the Node should be inserted in the
	//the next level up
	bool moveOneUp = moveUp();
	SLNode* moveNodeUp;
	SLNode* nodeBelow = nodeToInsert;

	//continues to insert the Node in to the next level up while moveOneUp
	//is true. moveUp() is called at the end of each loop to determine if the
	//Node should be moved up again
	while (moveOneUp) {

		nodeBefore = nodeToInsert->previous;
		nodeAfter = nodeToInsert->next;

		moveNodeUp = new SLNode(toInsert, NULL, nodeBelow, NULL, NULL, currentLevel + 1);

		currentLevel++;
		//finds the nodefore the node in the next level up
		//so that pointers can be reset and the new Node sewn in
		while (nodeBefore->up == NULL && nodeBefore->level < LEVEL) {
			nodeBefore = nodeBefore->previous;
		}
		nodeBefore = nodeBefore->up;
		//finds the nodeAfter the node in the next level up
		//so that pointers can be reset and the new Node sewn in
		while (nodeAfter->up == NULL && nodeAfter->level < LEVEL) {
			nodeAfter = nodeAfter->next;
		}
		nodeAfter = nodeAfter->up;
		//sews the new Node into the current level
		nodeBelow->up = moveNodeUp;
		nodeBelow = nodeBelow->up;
		nodeBefore->next = moveNodeUp;
		moveNodeUp->previous = nodeBefore;
		nodeAfter->previous = moveNodeUp;
		moveNodeUp->next = nodeAfter;
		//bnreaks if we have reached the top level
		if (currentLevel >= LEVEL) {
			break;
		}
		//resets moveOneUp
		moveOneUp = moveUp();

	}
	return success;
};

/*-------------------------------------------------------------------------------------------------

	Method returns a bool indicating if the Node should be moved up a level. Value is determined
	by a random number generator. Returns true if the number generated is greater than 50.
	Method cannot change any data members.

	POSTCONDITIONS:
		- returns bool indicating if the Node should be moved up a level

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::moveUp(void) const {
	int moveUp = rand() % 100;	//generates random numebr
	return moveUp > 50;			//returns if moveUp is greater than 50
};

/*-------------------------------------------------------------------------------------------------

	Method returns a SLNode* to the Node that contains a data Object that matches the target
	parameter. If the target parameter is not found in the list then the Node* to the Node
	that contains a Object data member one greater than the target parameter is returned.
	Method cannot change any data members.

	POSTCONDITIONS:
		- returns a Node* to the Node that contains the target parameter
		- returns a Node* to the Node greater than the target parameter

-------------------------------------------------------------------------------------------------*/

template<class Object>
typename SkipList<Object>::SLNode* SkipList<Object>::retrieve(const Object& target) const  {
	//checks if the list is empty, if so returns the
	//dummy tail node on the bottom master level
	if (isEmpty()) {
		return dummyHead[0]->next;
	} else {								
		SLNode* current;	//SLNode* to walk the list

		//loop iterates over the list starting at the top level and working its
		//way down. If a SLNode is encountered that contains a data member greater
		//than the target current is moved one back and one down. If the bottom
		//level is reached and current encounters a data member greater than target
		//that SLNode* is returned.
		for (int currentLevel = LEVEL - 1; currentLevel > -1; currentLevel--) {
			current = dummyHead[currentLevel];

			if (levelIsEmpty(currentLevel) && currentLevel != 0) {	//checks if the level is empty
				current = current->down;							//if so move the Node down one
			}  else {
				current = current->next;
				while (current->isDummy == false) {	//iterates over the current level comparing
					if (*current->data == target) {	//current to target
						return current;
					} else if (*current->data > target) {
						if (current->level == 0) {
							return current;
						} else {
							current = current->previous->down;
						}
					}
					current = current->next;
				}
			}
		}
		return current;
	}
}

/*-------------------------------------------------------------------------------------------------

	Method returns a bool value indicating if the target Object is in the SkipList. retrieve() is
	called as a helper method and the data of the Node returned is evaluated for equality with
	the target parameter. Method cannot change any data members.

	POSTCONDITIONS:
		- returns a bool value indicating if the target is in the SkipList

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::contains(const Object& target) const  {
	SLNode* temp = retrieve(target);	//attempts to retrieve the target parameter
	if (temp->data == NULL) {			//returns if the target parameter's data is
		return false;					//NULL indicating a dummy Node
	}
	if (target == *temp->data) {		//returns if the Node's data is equal
		return true;					//to the target
	}
	return false;
};

/*-------------------------------------------------------------------------------------------------

	Method returns an int value corresponding to the number of nodes in the master level.
	Method cannot change any data members.

	POSTCONDITIONS:
		- returns an int value corresponding to the number of Nodes in the master level

-------------------------------------------------------------------------------------------------*/

template<class Object>
int SkipList<Object>::size(void) const {
	int retVal = 0;		//variable to hold the number of unique nodes in the list
	if (isEmpty()) {	//checks if the list is empty
		return retVal;	//returns a size of zero
	}
	SLNode* current = dummyHead[0];	//node to walk the list

	while (current->next->data != NULL) {	//increases retVal
		current = current->next;			//for each node in the master level
		retVal++;
	}
	return retVal;	//returns retVal
};

/*-------------------------------------------------------------------------------------------------

	Method returns a bool value indicating if there are any Nodes in the master level.
	Method cannot change any data members.

	POSTCONDITIONS:
		- returns false if there are no Nodes in the master level

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::isEmpty(void) const {
	SLNode* current = dummyHead[0];	//Node to walk the list
	return current->next->data == NULL;	//returns if the next node in the list is the dummy tail
};

/*-------------------------------------------------------------------------------------------------

	Method deallocates all dynamically allocated memory for Nodes other than the dummy head
	and dummy tail nodes. Calls clear as a helper function

	POSTCONDITIONS:
		- deletes all non-dummy nodes in the list

-------------------------------------------------------------------------------------------------*/

template<class Object>
void SkipList<Object>::makeEmpty(void) {
	if (isEmpty()) {	//bails if the list is already empty
		return;
	}
	clear();			//calls clear
};

/*-------------------------------------------------------------------------------------------------

	Method deallocates all dynamically allocated memory for Nodes other than the dummy head
	and dummy tail nodes. Calls toDelete as a helper function.

	POSTCONDITIONS:
		- deletes all non-dummy nodes in the list

-------------------------------------------------------------------------------------------------*/

template<class Object>
void SkipList<Object>::clear(void) {

	SLNode* current;		//SLNode to keep track of the current node
	SLNode* toDelete;

	//for loop that iterates over each level in the skip list and 
	//deletes nodes starting at the top (most sparsely populated level)
	//and works it's way down
	for (int currentLevel = LEVEL - 1; currentLevel > -1; currentLevel--) {
		current = dummyHead[currentLevel];
		if (levelIsEmpty(currentLevel)) {
			current = current->down;
		} else {

			current = current->next;	//sets current equal to the first initialized node in the level

			//while loop that continues until the dummy tail (identified by data == NULL)
			//is reached. deletes each node on the current level

			while (current->isDummy == false) {	//deletes each node in the level until the
				toDelete = current;				//dummy tail is reached
				current = current->next;
				deleteNode(toDelete);
			}
		}
	}
};

/*-------------------------------------------------------------------------------------------------

	Method returns a bool value indicating if the level of the list is empty, has no nodes
	other than the dummy head and dummy tail nodes. Method cannot change any data members.

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::levelIsEmpty(const int currentLevel) const {
	//returns true if the dummy head's data member equals the next node (both are NULL)
	if (dummyHead[currentLevel]->isDummy == true && dummyHead[currentLevel]->next->isDummy == true) {
		return true;
	}
	return false;
};

/*-------------------------------------------------------------------------------------------------

	Method deallocates all memory for the Node passed through as a parameter. The Nodes before,
	after, below, and above the Node to be deleted will be redirected to correctly be sewn into
	the list.

-------------------------------------------------------------------------------------------------*/

template<class Object>
void SkipList<Object>::deleteNode(SLNode*& toClear) {
	if (toClear == NULL) {	//checks if toClear is NULL
		return;				//bails
	}
	//Node pointers set to each of the Node's
	//surrounding the Node to be deleted
	//int variable equal to the level toClear is on
	SLNode* nodeBefore = toClear->previous;
	SLNode* nodeAfter = toClear->next;
	SLNode* nodeUp = toClear->up;
	SLNode* nodeDown = toClear->down;
	int level = toClear->level;

	//deletes toClear's data
	//also deletes toClear
	delete toClear->data;
	delete toClear;
	toClear = NULL;	//sets toClear to NULL

	//sews in the Nodes surrounding the delted Node
	nodeBefore->next = nodeAfter;
	nodeAfter->previous = nodeBefore;

	if (nodeDown != NULL) {
		nodeDown->up = nodeUp;
	}
	if (nodeUp != NULL) {
		nodeUp->down = nodeDown;
	}
};

/*-------------------------------------------------------------------------------------------------

	Takes in an object and calls contain and retrieve to find if the object is in the SkipList.
	If the object is in the SkipList it is removed via deleteNode. A bool value is returned
	indicating whether the Node was successfully removed from the list.

	POSTCONDITIONS:
		- removes the node containing the data member equal to the target parameter from the
		  list
		- returns a bool value indicating success

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::remove(const Object& toRemove) {
	
	bool canRemove = contains(toRemove);	//checks to see if the object is in the list
	SLNode* current;
	SLNode* toDelete;						//will hold SLNode* to the node containing toRemove

	if (canRemove) {	//if the object is in the list
		current = retrieve(toRemove);		//sets deleteNode to the node containing toRemove
		for (int currentLevel = current->level; currentLevel > -1; currentLevel--) {
			toDelete = current;
			current = current->down;
			deleteNode(toDelete);				//deletes the appropriate node and resets pointers
		}
		return true;
	} else {
		cout << "Object not in SkipList!" << endl;
	}
	
	return false;							//returns false if the object was not in the list
};

/*-------------------------------------------------------------------------------------------------

	Method initializes dummy head and dummy tail nodes. Dummy nodes have data objects
	initialized to NULL.

	POSTCONDITIONS:
		- initializes dummy head and dummy tail nodes

-------------------------------------------------------------------------------------------------*/

template<class Object>
void SkipList<Object>::initialize(void) {

	for (int currentLevel = 0; currentLevel < LEVEL; currentLevel++) {
		
		dummyHead[currentLevel] = new SLNode;		//creates empty SLNode
		dummyHead[currentLevel]->isDummy = true;
		dummyHead[currentLevel]->level = currentLevel;
		
		if (currentLevel > 0) {						//if the current level is not the master level
			//sets the down pointer to the dummy node below
			dummyHead[currentLevel]->down = dummyHead[currentLevel - 1];
		}

		if (currentLevel > 0) {					//if the current level is not the top
			dummyHead[currentLevel - 1]->up = dummyHead[currentLevel];
		}


		dummyHead[currentLevel]->next = new SLNode;	//creates new SLNode
		dummyHead[currentLevel]->next->isDummy = true;
		dummyHead[currentLevel]->next->level = currentLevel;
		dummyHead[currentLevel]->next->previous = dummyHead[currentLevel];

		if (currentLevel > 0) {						//if the current level is not the master level
													//sets the down pointer to the dummy node below
			dummyHead[currentLevel]->next->down = dummyHead[currentLevel - 1]->next;
		}
		if (currentLevel > 0) {					//if the current level is not the top
			dummyHead[currentLevel - 1]->next->up = dummyHead[currentLevel]->next;
		}
	}

};

/*-------------------------------------------------------------------------------------------------

	Overloaded operator equals. Walks the bottom level of the SkipList and compares the
	Nodes to the Nodes in the parameter SkipList. Returns false at the first sign of
	inequality. Method cannot change any data members.

-------------------------------------------------------------------------------------------------*/

template<class Object>
bool SkipList<Object>::operator==(const SkipList& toCompare) const {
	//checks is either of the lists are empty or if one is empty
	//and the other is not
	if (this->isEmpty() && toCompare.isEmpty() == false) {
		return false;
	} else if (this->isEmpty() == false && toCompare.isEmpty()) {
		return false;
	} else if (this->isEmpty() && toCompare.isEmpty()) {
		return true;
	}
	//returns false if they are not of the same size
	if (this->size() != toCompare.size()) {
		return false;
	}

	SLNode* thisList = dummyHead[0]->next;
	SLNode* thatList = toCompare.dummyHead[0]->next;

	//walks the bottom level and compares each Node for equality
	//returns false at the first sign of inequality
	while (thisList->data != NULL) {
		if (thisList->data != thatList->data) {
			return false;
		}
		thisList = thisList->next;
		thatList = thatList->next;
	}
	return true;
};

/*-------------------------------------------------------------------------------------------------
	
	Overloaded assignment operator. Makes a deep copy of the parameter list.

	POSTCONDITIONS:
		- creates a deep copy of the parameter list

	NOTES:	List will not have all the same nodes at all the same levels. Only the master
			level is copied.

-------------------------------------------------------------------------------------------------*/

template<class Object>
void SkipList<Object>::operator=(const SkipList& toCopy) {
	if (*this == toCopy) {	//bails if the parameter list is equal to this
		return;
	}

	this->makeEmpty();	//deallocates all memory in the list
	SLNode* current;
	Object toInsert;

	if (toCopy.isEmpty()) {		//bails if the parameter list is empty
		return;
	}

	current = toCopy.dummyHead[0]->next;	//walks the parameter master level and 
	while (current->isDummy == false) {		//inserts each Node in to the new list
		toInsert = current->data;
		insert(toInsert);
		current = current->next;
	}
};