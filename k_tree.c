#include<stdio.h>
#include<stdlib.h>
#include <Windows.h>
#define MAX_VALUES 3
#define MAX_CHILD 4

//Global variables for the fake pointers
int nodeCount;
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
	//Modify global variables
	nodeArray[++nodeCount] = myTree;
	//Return
	return myTree;
}


int isleaf(struct k_tree* myTree){
	//If each child is the empty value then it's a leaf
	return (myTree->childIdx[0] == 0 && myTree->childIdx[1] == 0
	&& myTree->childIdx[2] == 0 && myTree->childIdx[3] == 0);
}

int isfull(struct k_tree* myTree){
	//If each child is different from the empty child value(0) then it's full
	return myTree->nv == 3;
}


int addToNode(struct k_tree* myTree, int val){
	//Simply add to a node
	int pos = -1; //mod
	if (myTree->nv == 0){
		myTree->values[0] = val;
		pos = 0; //mod
	}
	else{
		for ( int idx = (myTree->nv)-1 ; idx >= 0; idx-- ){
			if (val > myTree->values[idx]){
				myTree->values[idx+1] = val;
				pos = idx+1; //mod
				break;
				}
			else if (val < myTree->values[idx]){
				myTree->values[idx+1] = myTree->values[idx];
			if (idx == 0)
				myTree->values[idx] = val;
				pos = 0; //mod
				
			}
		}
	}
	++(myTree->nv);
	return pos; //mod
}


int isleafNotFull(struct k_tree* myTree){
	return (isleaf(myTree) && !isfull(myTree));
}


int getWhichChild(struct k_tree* myTree, int val){
	//Return the number of the child that has to receive a new value
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
	//Correct original node values 
	myTree->values[0] = myTree->values[1];
	myTree->nv = 1;
}

void addToNodeWithPos(struct k_tree* myTree,int val,int pos){
	//Shift values to make some space, shift children during the process
	for(int i = myTree->nv; i > pos; i--){
		myTree->values[i] = myTree->values[i-1];
		myTree->childIdx[i] = myTree->childIdx[i-1];
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
	
}


void insert(struct k_tree* myTree,int val){
	//Insert if the node is a leaf and has space
	if (isleafNotFull(myTree)){
		printf("Feuille non vide j'ajoute %d\n",val); //------------------------------------------------
		addToNode(myTree,val);
		return;
	}
	//If the root is full we split it and we try again from the root
	if (isfull(myTree)){
		printf("Root Full, je split\n");//--------------------------------------------------------
		split(myTree);
		//insert(myTree,val);
	}
	//Find the right child to add the value
	int childIdToCheck = getWhichChild(myTree, val);
	//If there is no child we make one
	if (myTree->childIdx[childIdToCheck] == 0){
		printf("Pas de child je créer avec la valeur %d\n", val);//--------------------------------------------------------
		myTree->childIdx[childIdToCheck] = nodeCount + 1;
		struct k_tree *newChild = createTree(val);
		return;
	}
	//If that child is full we split it then we try again on the same node
	if (isfull(nodeArray[myTree->childIdx[childIdToCheck]])){
		printf("Split foireusement le child qui est full\n");//--------------------------------------------------------
		splitChild(myTree,childIdToCheck);
		insert(myTree, val);

	}
	// else if that child has space we insert into it
	else{
		printf("Appel récursif sur le fils trouve pour ajouter %d\n",val);
		insert(nodeArray[myTree->childIdx[childIdToCheck]],val);
	}
	//Then we call the function on that child OR LETS TRY ON THE SAME NODE
	//printf("Appel récursif sur le node %d, valeur du node courrant : %d\n, valeur a ajouter : %d", childIdToCheck, myTree->values[0], val);//--------------------------------------------------------
	//insert(nodeArray[myTree->childIdx[childIdToCheck]],val);
	//insert(myTree,val);
	
	
}


void printNode(struct k_tree* myTree){
	printf("\nvalues : ");
	for (int i = 0 ; i < myTree->nv; i++ ){
		printf("|%d|",myTree->values[i]);
	}
	printf("\n\n");
	printf("number of values : %d", myTree->nv);
	printf("\n\n");
	for (int i = 0; i <= 3; i++){
		printf("children %d index : %d\n", i, myTree->childIdx[i]);
	}
	printf("\n");
}


int main (int argc, char* argv[]){
	//Initialising global variables
	nodeArray = malloc(1024*(sizeof(struct k_tree)));
	nodeCount = 0;
	//Creating a new tree
	struct k_tree *myTree = createTree(1);
	insert(myTree,2);
	insert(myTree,3);
	insert(myTree,4);
	insert(myTree,5);
	insert(myTree,6);
	//insert(myTree,0);
	printNode(nodeArray[4]);
}