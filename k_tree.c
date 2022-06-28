#include<stdio.h>
#include<stdlib.h>
#define MAX_VALUES 3
#define MAX_CHILD 4

//Global variables for the fake pointers
int nodeCount;
int maxNode;
struct k_tree **nodeArray;


//Define the k_tree struct, has values, value counter, and children indexes for the node array.
struct k_tree{
	int values[MAX_VALUES];
	unsigned nv;
	unsigned childIdx[4];
};


struct k_tree *createTree(int val){
	//Create an empty tree
	struct k_tree *myTree = malloc(sizeof(struct k_tree));
	//Fill the properties
	myTree->values[0] = val;
	myTree->nv = 1;
	myTree->childIdx[0] = 0;
	myTree->childIdx[1] = 0;
	myTree->childIdx[2] = 0;
	myTree->childIdx[3] = 0;
	//Add the reference in the node array
	//Realloc when the nodeArray is about to be full
	if (nodeCount == maxNode-16){
		maxNode = maxNode*2;
		realloc(nodeArray, maxNode);
	}
	nodeArray[++nodeCount] = myTree;
	//Return
	return myTree;
}


int isleaf(struct k_tree* myTree){
	//If each child is the empty value then it's a leaf
	for(int i = 0; i < MAX_CHILD; i++){
		if (myTree->childIdx[i] != 0)
			return 0;
	}
	return 1;
}

int isfull(struct k_tree* myTree){
	//If the number of values is equal to the max set then it's full
	return myTree->nv == MAX_VALUES;
}


void addToNode(struct k_tree* myTree, int val){
	//If the value already exist then exit
	for (int i = 0; i<MAX_CHILD; i++){
		if (myTree->values[i] == val)
			return;
	}

	//Else the value is added
	for ( int idx = (myTree->nv)-1 ; idx >= 0; idx-- ){
		if (val > myTree->values[idx]){
			myTree->values[idx+1] = val;
			break;
			}
		else if (val < myTree->values[idx]){
			myTree->values[idx+1] = myTree->values[idx];
		if (idx == 0)
			myTree->values[idx] = val;
			
		}
	}
	++(myTree->nv);
}


int isleafNotFull(struct k_tree* myTree){
	return (isleaf(myTree) && !isfull(myTree));
}


int getWhichChild(struct k_tree* myTree, int val){
	//Return the index of the child that has to receive a new value
	for (int i = 0; i < myTree->nv; i++){
		if (val < myTree->values[i]){
			return i;
		}
	}
	return myTree->nv;
}

void split(struct k_tree* myTree){
	//Create the 2 new children
	struct k_tree *newLeftChild = createTree(myTree->values[0]);
	int indexLeft = nodeCount;
	struct k_tree *newRightChild = createTree(myTree->values[2]);
	int indexRight = nodeCount;
	//Assign children
	newLeftChild->childIdx[0] = myTree->childIdx[0];
	newLeftChild->childIdx[1] = myTree->childIdx[1];
	newRightChild->childIdx[0] = myTree->childIdx[2];
	newRightChild->childIdx[1] = myTree->childIdx[3];
	myTree->childIdx[0] = indexLeft;
	myTree->childIdx[1] = indexRight;
	//myTree->childIdx[2] = 0;
	//myTree->childIdx[3] = 0;
	//Correct original node values 
	myTree->values[0] = myTree->values[1];
	myTree->nv = 1;
}

void addToNodeWithPos(struct k_tree* myTree,int val,int pos){
	//The difference with addToNode is that we know where to add the new value
	//Also since we are in a parent node the children indexes might need to be right shifted
	//Shift values to make some space, shift children during the process
	for(int i = myTree->nv; i > pos; i--){
		myTree->values[i] = myTree->values[i-1];
		myTree->childIdx[i+1] = myTree->childIdx[i];
	}
	//Add the value to the node
	myTree->values[pos] = val;
	++(myTree->nv);
}


void splitChild(struct k_tree* myTree,int childId){
	//Get the value of the middle from the child we are interested in
	int valueToAdd = nodeArray[myTree->childIdx[childId]]->values[1];
	//Add the value to the parent, childId is actually the position where we want to add the middle of the child
	addToNodeWithPos(myTree, valueToAdd, childId);
	//Create a new node with the child's right value
	int rightValue = nodeArray[myTree->childIdx[childId]]->values[2];
	struct k_tree *newChild = createTree(rightValue);
	//Child's left value remain in the same node, we just delete the middle and right value
	nodeArray[myTree->childIdx[childId]]->nv = 1;
	//new node is the right child of the new value added to the parent node
	myTree->childIdx[childId+1] = nodeCount;
	//un ptit test
	
}


void insert(struct k_tree* myTree,int val){
	//Insert if the node is a leaf and has space
	if (isleafNotFull(myTree)){
		addToNode(myTree,val);
		return;
	}
	//If the root is full we split it and we try again from the root
	if (isfull(myTree)){
		split(myTree);
	}
	//Find the right child to add the value
	int childIdToCheck = getWhichChild(myTree, val);
	//If there is no child we make one
	if (myTree->childIdx[childIdToCheck] == 0){
		myTree->childIdx[childIdToCheck] = nodeCount + 1;
		struct k_tree *newChild = createTree(val);
		return;
	}
	//If that child is full we split it then we try again on the same node
	if (isfull(nodeArray[myTree->childIdx[childIdToCheck]])){
		splitChild(myTree,childIdToCheck);
		insert(myTree, val);

	}
	// else if that child has space we insert into it
	else{
		insert(nodeArray[myTree->childIdx[childIdToCheck]],val);
	}
	
	
}

//Test function to get information about the node
void printNode(struct k_tree* myTree){
	printf("\nvalues : ");
	for (int i = 0 ; i < myTree->nv; i++ ){
		printf("|%d|",myTree->values[i]);
	}
	printf("\n\n");
	for (int i = 0; i <= 3; i++){
		printf("child %d : %d\n", i, myTree->childIdx[i]);
	}
	printf("\n");
}


int main (int argc, char* argv[]){
	//Initialising global variables
	int maxNode = 1024;
	nodeArray = malloc(maxNode*4);
	nodeCount = 0;
	//Creating a new tree
	struct k_tree *myTree = createTree(1);
	insert(myTree,2);
	insert(myTree,3);
	insert(myTree,4);
	insert(myTree,5);
	insert(myTree,6);
	insert(myTree,6);
	printNode(nodeArray[1]);
	printNode(nodeArray[2]);
	printNode(nodeArray[3]);
	printNode(nodeArray[4]);

	free(nodeArray);
}