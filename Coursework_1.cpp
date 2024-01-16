#include <iostream> 
#include <iomanip>
#include "stdarg.h"
#include "DateTime.h" 
#include "Items.h" 
#include "Headers.h" 
#include "ICS0017DataSource.h"
#include "DataStructure.h"
using namespace std;
#pragma warning ( disable : 4996 )
// IMPORTANT: follow the given order of *.h files: ICS0017DataSource.h must be 
// the last
void PrintItem7(item7* pI, int* n) {
	char* id = pI->pID;
	unsigned long int Code = pI->Code;
	
	printf("%d) %s %u %d-%s-%d \r\n", *n, id, Code, pI->pDate->Day, pI->pDate->Month, pI->pDate->Year);
	(*n)++;
}

void PrintHeaderA(HEADER_A* pA, int* n) {
	ITEM7* pI = (ITEM7*)pA->pItems;
	while (pI->pNext != 0) {
		PrintItem7(pI, n);
		pI = pI->pNext;
	}
	PrintItem7(pI, n);
}

void PrintHeaderB(HEADER_B* p, int* n) {
	HEADER_A* pA = p->pHeaderA;
	while (pA->pNext != 0) {
		PrintHeaderA(pA, n);
		pA = pA->pNext;
	}
	PrintHeaderA(pA, n);
}

void PrintDataStructure(HEADER_B* p) {
	int n = 1;
	while (p->pNext != 0) {
		PrintHeaderB(p, &n);
		p = p->pNext;
	}
	PrintHeaderB(p, &n);
}

bool checkWord(char* p, int len) {
	for (int i = 0; i < len; i++) {
		int ascii = (int)*(p + i);
		if (!((ascii >= 65 || ascii <= 90) || (ascii >= 97 || ascii <= 122) || ascii == 45)) return true;
	}
	return false;
}

bool validateIDFormat(char* pNewItemID) {
	if (strlen(pNewItemID) < 3) return false;
	char* spacePos = strchr(pNewItemID, ' ');
	if ( spacePos == 0 || strchr(spacePos + 1, ' ') != 0) return false;
	char first = *pNewItemID;
	char second = *(spacePos + 1);
	if ((int)first < 65 || (int)first > 90 || (int)second < 65 || (int)second > 90) return false;
	int lenFirst = spacePos - pNewItemID;
	int lenSecond = strlen(pNewItemID) - lenFirst - 1;
	if (checkWord(pNewItemID, lenFirst) || checkWord(spacePos + 1, lenSecond)) return false;
	return true;
}

void createBHeader(char first, char second, HEADER_B* p, ITEM7* pI) {
	HEADER_A *newHeader = new HEADER_A;
	newHeader = (HEADER_A *)malloc(sizeof(HEADER_A));
	newHeader->cBegin = second;
	newHeader->pItems = pI;
	newHeader->pNext = 0;
	HEADER_B *newBHeader = new HEADER_B;
	newBHeader = (HEADER_B *)malloc(sizeof(HEADER_B));
	newBHeader->cBegin = first;
	newBHeader->pHeaderA = newHeader;
	newBHeader->pNext = 0;
	while (p->pNext != 0 && (int) p->pNext->cBegin < (int) first) { p = p->pNext; }
	if (p->pNext == 0) {
		p->pNext = newBHeader;
	}
	else {
		newBHeader->pNext = p->pNext;
		p->pNext = newBHeader;
	}
	return;
}

void createAHeader(char second, HEADER_A* p, ITEM7 *pI) {
	HEADER_A* newHeader;
	newHeader = (HEADER_A*)malloc(sizeof(HEADER_A));
	newHeader->cBegin = second;
	newHeader->pItems = pI;
	newHeader->pNext = 0;
	while (p->pNext != 0 && (int) p->pNext->cBegin < (int)second) { p = p->pNext; }
	if (p->pNext == 0) {
		p->pNext = newHeader;
	}
	else {
		newHeader->pNext = p->pNext;
		p->pNext = newHeader;
	}
	return;
}

HEADER_B* InsertItem(HEADER_B* p, char* pNewItemID = 0) {
	if (pNewItemID == 0 || validateIDFormat(pNewItemID)) {
		ITEM7* pI = (ITEM7*)GetItem(7, pNewItemID);
		printf("\r\n Item name: %s\r\n", pI->pID);
		if (pNewItemID == 0) {
			pNewItemID = pI->pID;
		}
		char first = *pNewItemID;
		char second = *(strchr(pNewItemID, ' ') + 1);
		HEADER_B* searchP = p;
		while (searchP->cBegin != first && searchP->pNext != 0) { searchP = searchP->pNext; }
		if (searchP->cBegin != first && searchP->pNext == 0) {
			createBHeader(first, second, p, pI);
			return p;
		}
		else {
			HEADER_A* searchA = searchP->pHeaderA;
			while (searchA->cBegin != second && searchA->pNext != 0) { searchA = searchA->pNext; }
			if (searchA->cBegin != second && searchA->pNext == 0) {
				createAHeader(second, searchP->pHeaderA, pI);
				return p;
			}
			else {
				ITEM7* searchI =(ITEM7 *) searchA->pItems;
				while (strcmp(searchI->pID, pI->pID) != 0 && searchI->pNext != 0) { searchI = searchI->pNext; }
				if (strcmp(searchI->pID, pI->pID) != 0 && searchI->pNext == 0) {
					pI->pNext = (ITEM7 *) searchA->pItems;
					searchA->pItems = pI;
					return p;
				}
				else {
					throw 1337;
				}
			}
		}
	}
	else {
		throw 1337;
	}
}

void RemoveItem(HEADER_B* p, char* pItemID) {
	if (validateIDFormat(pItemID)) {
		char first = *pItemID;
		char second = *(strchr(pItemID, ' ') + 1);
		HEADER_B* searchP = p;
		HEADER_B* prevB = 0;
		while (searchP->cBegin != first && searchP->pNext != 0) {
			prevB = searchP;
			searchP = searchP->pNext;
		}
		if (searchP->cBegin != first && searchP->pNext == 0) {
			throw 1337;
		}
		else {
			HEADER_A* searchA = searchP->pHeaderA;
			HEADER_A* prevA = 0;
			while (searchA->cBegin != second && searchA->pNext != 0) {
				prevA = searchA;
				searchA = searchA->pNext;
			}
			if (searchA->cBegin != second && searchA->pNext == 0) {
				throw 1337;
			}
			else {
				ITEM7* searchI = (ITEM7*)searchA->pItems;
				ITEM7* prev = 0;
				while (strcmp(searchI->pID, pItemID) != 0 && searchI->pNext != 0) {
					prev = searchI;
					searchI = searchI->pNext;
				}
				if (strcmp(searchI->pID, pItemID) != 0 && searchI->pNext == 0) {
					throw 1337;
				}
				else {
					if (searchI == (ITEM7*)searchA->pItems) {
						delete(searchI->pID);
						delete(searchI->pDate);
						searchA->pItems = searchI->pNext;
						delete(searchI);
					}
					else if (searchI->pNext == 0) {
						delete(searchI->pID);
						delete(searchI->pDate);
						prev->pNext = 0;
						delete(searchI);
					}
					else {
						delete(searchI->pID);
						delete(searchI->pDate);
						prev->pNext = searchI->pNext;
						delete(searchI);
					}
					if (searchA->pItems == 0) {
						if (prevA == 0) {
							searchP->pHeaderA = searchA->pNext;
						}
						else {
							prevA->pNext = searchA->pNext;
						}
						delete(searchA);
						if (searchP->pHeaderA == 0) {
							if (prevB == 0) {
								p = searchP->pNext;
							}
							else {
								prevB->pNext = searchP->pNext;
							}
							delete(searchP);
						}
					}
					return;
				}
			}
		}
	}
	else {
		throw 1337;
	}
}

void oldCoursework2() {
	DataStructure* dS = new DataStructure();
	cout << "print empty" << endl;
	cout << *dS << endl;
	cout << "printed" << endl;
	char name[] = "Phtahlo Blue";
	ITEM7* it = (ITEM7*)GetItem(7, name);
	*dS += it;
	*dS += (ITEM7*)GetItem(7);
	cout << *dS << endl;
	cout << dS->GetItemPointer(name) << endl;
	DataStructure* newS = new DataStructure(*dS);
	cout << *newS << endl;
	cout << "newS and dS are equal: " << (*newS == *dS) << endl;
	*newS += (ITEM7*)GetItem(7);
	*newS += (ITEM7*)GetItem(7);
	cout << *newS << endl;
	cout << *dS << endl;
	DataStructure dSS;
	dSS = *newS;
	cout << "dSS is now equal to newS" << endl;
	cout << dSS << endl;
	dSS += (ITEM7*)GetItem(7);
	cout << *newS << endl;
	cout << dSS << endl;
	char dest[] = "C:\\temp\\data.txt";
	newS->Write(dest);
	cout << "Wrote a file" << endl;
	DataStructure evenNewerS = DataStructure(dest);
	cout << "From file" << endl;
	cout << evenNewerS << endl;
	//delete dS;
	//cout << *dS << endl;
}

void coursework1() {
	HEADER_B* p = GetStruct1(7, 30);
	PrintDataStructure(p);
	char insert[][5] = {
		"Z A",
		"Z Z",
		"Z K",
		"A Z",
		"A A",
		"A K",
		"G Z",
		"G A",
		"G K",
		"M A",
		"M Ba",
		"M Bb",
		"M Z"
	};
	for (int i = 0; i < 13; i++) {
		InsertItem(p, insert[i]);
	}
	try {
		char insertNew[] = "M";
		InsertItem(p, insertNew);
	}
	catch (...) {
		printf("invalid operation\n\r");
	}
	try {
		char insertNew[] = "Ba";
		InsertItem(p, insertNew);
	}
	catch (...) {
		printf("invalid operation\n\r");
	}
	try {
		char insertNew[] = "Mba";
		InsertItem(p, insertNew);
	}
	catch (...) {
		printf("invalid operation\n\r");
	}
	PrintDataStructure(p);
	for (int i = 0; i < 13; i++) {
		RemoveItem(p, insert[i]);
	}
	try {
		char insertNew[] = "M";
		RemoveItem(p, insertNew);
	}
	catch (...) {
		printf("invalid operation\n\r");
	}
	try {
		char insertNew[] = "Ba";
		RemoveItem(p, insertNew);
	}
	catch (...) {
		printf("invalid operation\n\r");
	}
	try {
		char insertNew[] = "Mba";
		RemoveItem(p, insertNew);
	}
	catch (...) {
		printf("invalid operation\n\r");
	}
	PrintDataStructure(p);
	return;
}

void coursework2() {
	DataStructure *dataStructure = new DataStructure();
	for (int i = 0; i < 10; i++) {
		*dataStructure += (ITEM7*)GetItem(7);
	}
	cout << *dataStructure << endl;
	cout << "Number of items: " << dataStructure->GetItemsNumber() << endl;
	cout << dataStructure->GetItemPointer((char *)"Light Cyan") << endl;
	cout << dataStructure->GetItemPointer((char*)"X X") << endl;
	DataStructure *newDS = new DataStructure(*dataStructure);
	*dataStructure -= (char*)"Banana Mania";
	*dataStructure -= (char*)"Vegas Gold";
	*dataStructure -= (char*)"Persian Green";
	cout << "Old data structure and new are equal? " << (*newDS == *dataStructure) << endl;
	dataStructure->Write((char*)"C:\\temp\\data.txt");
	DataStructure fromFile = DataStructure((char*)"C:\\temp\\data.txt");
	cout << "Old data structure and one created from file are equal? " << (fromFile == *dataStructure) << endl;
	fromFile = *newDS;
	cout << "Assigned 10 item data structure to one created from file " << endl;
	cout << fromFile << endl;
	return;
}

int main()
{
	coursework2();
	coursework1();
	return 0;
}