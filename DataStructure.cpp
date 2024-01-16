#include "DataStructure.h"
#pragma warning (disable: 4996)

ITEM7* DataStructure::CopyItem7(ITEM7* p) {
	ITEM7* it = new ITEM7;
	it->pNext = 0;
	int n;
	char* pNewID = new char[n = strlen(p->pID) + 1];
	strcpy_s(pNewID, n, p->pID);
	DATE2* date = new DATE2;
	date->Day= p->pDate->Day;
	date->Month[0] = p->pDate->Month[0];
	date->Month[1] = p->pDate->Month[1];
	date->Month[2] = p->pDate->Month[2];
	date->Month[3] = p->pDate->Month[3];
	date->Year = p->pDate->Year;
	it->Code = p->Code;
	it->pID = pNewID;
	it->pDate = date;
	return it;
}

void DataStructure::PrintItem7(ITEM7* pI, int n) const{
	char* id = pI->pID;
	unsigned long int Code = pI->Code;
	int day = pI->pDate->Day;
	char *month = pI->pDate->Month;
	int year = pI->pDate->Year;

	cout << n << " " << id << " " << Code << " " << day << " " << month << " " << year << endl;

}

	void DataStructure::CreateAHeader(char second, HEADER_A* p, ITEM7* pI) {
		HEADER_A* newHeader = new HEADER_A;
		newHeader->cBegin = second;
		newHeader->pItems = pI;
		newHeader->pNext = 0;
		while (p->pNext != 0 && (int)p->pNext->cBegin < (int)second) { p = p->pNext; }
		if (p->pNext == 0) {
			p->pNext = newHeader;
		}
		else {
			newHeader->pNext = p->pNext;
			p->pNext = newHeader;
		}
		return;
	}

	void DataStructure::CreateBHeader(char first, char second, ITEM7* pI) {
		HEADER_B* p = this->EntryP;
		HEADER_A* newHeader = new HEADER_A;
		newHeader->cBegin = second;
		newHeader->pItems = pI;
		newHeader->pNext = 0;
		HEADER_B* newBHeader = new HEADER_B;
		newBHeader->cBegin = first;
		newBHeader->pHeaderA = newHeader;
		newBHeader->pNext = 0;
		if (p == 0) {
			this->EntryP = newBHeader;
		}
		else {
			while (p->pNext != 0 && (int)p->pNext->cBegin < (int)first) { p = p->pNext; }
			if (p->pNext == 0) {
				p->pNext = newBHeader;
			}
			else {
				newBHeader->pNext = p->pNext;
				p->pNext = newBHeader;
			}
		}
		return;
	}

	void DataStructure::InsertItem(ITEM7* pI) {
		//printf("\r\n Item name: %s\r\n", pI->pID);
		char first = *pI->pID;
		char second = *(strchr(pI->pID, ' ') + 1);
		HEADER_B* searchP = this->EntryP;

		if (this->EntryP == 0) {
			CreateBHeader(first, second, pI);
			return;
		}
		while (searchP->cBegin != first && searchP->pNext != 0) { searchP = searchP->pNext; }
		if (searchP->cBegin != first && searchP->pNext == 0) {
			CreateBHeader(first, second, pI);
			return;
		}
		else {
			HEADER_A* searchA = searchP->pHeaderA;
			while (searchA->cBegin != second && searchA->pNext != 0) { searchA = searchA->pNext; }
			if (searchA->cBegin != second && searchA->pNext == 0) {
				CreateAHeader(second, searchP->pHeaderA, pI);
				return;
			}
			else {
				ITEM7* searchI = (ITEM7*)searchA->pItems;
				while (strcmp(searchI->pID, pI->pID) != 0 && searchI->pNext != 0) { searchI = searchI->pNext; }
				if (strcmp(searchI->pID, pI->pID) != 0 && searchI->pNext == 0) {
					pI->pNext = (ITEM7*)searchA->pItems;
					searchA->pItems = pI;
					return;
				}
				else {
					throw exception("Item Exists");
				}
			}
		}
	}

	ITEM7* DataStructure::FindItem(char* pItemID, bool deleteItem = false) {
		char first = *pItemID;
		char second = *(strchr(pItemID, ' ') + 1);
		HEADER_B* searchP = this->EntryP;
		HEADER_B* prevB = 0;
		while (searchP->cBegin != first && searchP->pNext != 0) {
			prevB = searchP;
			searchP = searchP->pNext;
		}
		if (searchP->cBegin != first && searchP->pNext == 0) {
			return NULL;
		}
		else {
			HEADER_A* searchA = searchP->pHeaderA;
			HEADER_A* prevA = 0;
			while (searchA->cBegin != second && searchA->pNext != 0) {
				prevA = searchA;
				searchA = searchA->pNext;
			}
			if (searchA->cBegin != second && searchA->pNext == 0) {
				return NULL;
			}
			else {
				ITEM7* searchI = (ITEM7*)searchA->pItems;
				ITEM7* prev = 0;
				while (strcmp(searchI->pID, pItemID) != 0 && searchI->pNext != 0) {
					prev = searchI;
					searchI = searchI->pNext;
				}
				if (strcmp(searchI->pID, pItemID) != 0 && searchI->pNext == 0) {
					return 0;
				}
				else if (deleteItem) {
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
								this->EntryP = searchP->pNext;
							}
							else {
								prevB->pNext = searchP->pNext;
							}
							delete(searchP);
						}
					}
					return NULL;
				}
				else {
					return searchI;
				}
			}
		}
	}

	void DataStructure::IterItem7(item7* pI, IterOperation it, int& amt, ITEM7** arr) const {
		if (it == IterOperation::List) {
			arr[amt] = pI;
		}
		amt++;
		return;
	}

	void DataStructure::IterHeaderA(HEADER_A* pA, IterOperation it, int& amt, ITEM7** arr) const {
		ITEM7* pI = (ITEM7*)pA->pItems;
		while (pI->pNext != 0) {
			IterItem7(pI, it, amt, arr);
			pI = pI->pNext;
		}
		IterItem7(pI, it, amt, arr);
		return;
	}

	void DataStructure::IterHeaderB(HEADER_B* p, IterOperation it, int& amt, ITEM7** arr) const {
		HEADER_A* pA = p->pHeaderA;
		while (pA->pNext != 0) {
			IterHeaderA(pA, it, amt, arr);
			pA = pA->pNext;
		}
		IterHeaderA(pA, it, amt, arr);
		return;
	}


	void DataStructure::Iterate(IterOperation it, int &amt, ITEM7** arr) const{
		HEADER_B* pB = this->EntryP;
		if (pB == 0) {
			return;
		}
		while (pB->pNext != 0) {
			IterHeaderB(pB, it, amt, arr);
			pB = pB->pNext;
		}
		IterHeaderB(pB, it, amt, arr);
		return;
	}



	int DataStructure::GetItemsNumber() const{
		int itemAmt = 0;
		Iterate(IterOperation::Count, itemAmt, 0);
		return itemAmt;
	}

	ITEM7** DataStructure::GetItems() const{
		int amt = this->GetItemsNumber();
		ITEM7** items = (ITEM7**)malloc(amt * sizeof(ITEM7*));
		int zero = 0;
		Iterate(IterOperation::List, zero, items);
		return items;
	}

	void DataStructure::PrintDataStructure() const {
		int amt = this->GetItemsNumber();
		ITEM7** items = this->GetItems();
		if (amt == 0) return;
		for (int i = 0; i < amt; i++) {
			int printNum = i + 1;
			PrintItem7(items[i], printNum);
		}
		return;
	}

	



	DataStructure::DataStructure()
	{
		this->EntryP = 0;
	}
	DataStructure::DataStructure(char* pFileName) {
		FILE* pFile = fopen(pFileName, "rb");
		char* pData;
		long lSize;
		if (pFile) {
			fseek(pFile, 0, SEEK_END);
			lSize = ftell(pFile);
			rewind(pFile);
			pData = (char*)malloc(lSize);
			int n = fread(pData, 1, lSize, pFile);
			fclose(pFile);
		}
		else {
			cout << "file error" << endl;
			return;
		}
		char* cursor;
		int totalItems = 0;
		memcpy(&totalItems, cursor = pData, sizeof(int));
		cursor += sizeof(int);
		for (int i = 0; i < totalItems; i++) {
			int pIdSize = 0;
			char* pID;
			unsigned long int code = 0;
			int Day = 0;
			char Month[4] = "";
			int Year = 0;
			memcpy(&pIdSize, cursor, sizeof(int));
			pID = (char*)malloc(pIdSize * sizeof(char));
			memcpy(pID, cursor += sizeof(int), pIdSize);
			memcpy(&code, cursor += pIdSize, sizeof(unsigned long int));
			memcpy(&Day, cursor += sizeof(unsigned long int), sizeof(int));
			memcpy(&Month, cursor += sizeof(char)*4, sizeof(char));
			memcpy(&Year, cursor += sizeof(int), sizeof(int));
			cursor += sizeof(int);
			ITEM7* p =(ITEM7*) GetItem(7, pID);
			p->pDate->Day = Day;
			p->pDate->Month[0] = Month[0];
			p->pDate->Year = Year;
			p->Code = code;
			p->pID = pID;
			this->InsertItem(p);
		}
	}
	DataStructure::DataStructure(const DataStructure& original) {
		ITEM7** originalItems = original.GetItems();
		int itemNum = original.GetItemsNumber();
		for (int i = 0; i < itemNum; i++) {
			this->InsertItem(this->CopyItem7(originalItems[i]));
		}
	}
	DataStructure::~DataStructure() {
		ITEM7** items = this->GetItems();
		int itemNum = this->GetItemsNumber();
		for (int i = 0; i < itemNum; i++) {
			this->FindItem(items[i]->pID, true);
		}
		delete this->EntryP;
	}
	void DataStructure::operator+=(ITEM7* p) {
		this->InsertItem(p);
		return;
	}
	void DataStructure::operator-=(char* c) {
		this->FindItem(c, true);
		return;
	}

	DataStructure& DataStructure::operator=(const DataStructure& s)
	{
		ITEM7** items = this->GetItems();
		int itemNum = this->GetItemsNumber();
		for (int i = 0; i < itemNum; i++) {
			this->FindItem(items[i]->pID, true);
		}
		this->EntryP = 0;
		ITEM7** originalItems = s.GetItems();
		int otherItems = s.GetItemsNumber();
		for (int i = 0; i < otherItems; i++) {
			this->InsertItem(this->CopyItem7(originalItems[i]));
		}
		return *this;
	}


	int DataStructure::operator==(DataStructure& other)
	{
		int thisNum = this->GetItemsNumber();
		int otherNum = other.GetItemsNumber();
		if (thisNum == otherNum) {
			ITEM7** thisList = this->GetItems();
			for (int i = 0; i < thisNum; i++) {
				ITEM7* fromThis = thisList[i];
				ITEM7* fromOther = other.FindItem(thisList[i]->pID, false);
				if (!(
					strcmp(fromThis->pID, fromOther->pID)==0 &&
					fromThis->Code == fromOther->Code &&
					fromThis->pDate->Day == fromOther->pDate->Day &&
					fromThis->pDate->Month == fromOther->pDate->Month &&
					fromThis->pDate->Year == fromOther->pDate->Year
					)) {
					return 0;
				}
					
			}
			return 1;
		}
		return 0;
	}

	char* DataStructure::SerializeItem7(ITEM7* pI, int *m)
	{
		int n = strlen(pI->pID) + 1;
		char* itemSer, * r;
		*m = n + sizeof(ITEM7) + 3 * sizeof(int) + sizeof(unsigned long int) - sizeof(ITEM7*) - sizeof(TIME*) - sizeof(char*);
		itemSer = (char*)malloc(*m);
		memcpy(r = itemSer, &n, sizeof(int));
		memcpy(r += sizeof(int), pI->pID, n);
		memcpy(r += n, &pI->Code, sizeof(unsigned long int));
		memcpy(r += sizeof(unsigned long int), &pI->pDate->Day, sizeof(int));
		memcpy(r += sizeof(char), &pI->pDate->Month, sizeof(char));
		memcpy(r += sizeof(int), &pI->pDate->Year, sizeof(int));
		return itemSer;
	}

	void DataStructure::Write(char* location)
	{
		FILE* pFile = fopen(location, "wb");
		int thisNum = this->GetItemsNumber();
		ITEM7** thisList = this->GetItems();
		if (!pFile) {
			cout << "File not opened" << endl;
			return;
		}
		char* totalNum = (char*)malloc(sizeof(int));
		memcpy(totalNum, &thisNum, sizeof(int));
		fwrite(totalNum, 1, sizeof(int), pFile);
		for (int i = 0; i < thisNum; i++) {
			int n = 0;
			char* serialized = this->SerializeItem7(thisList[i], &n);
			fwrite(serialized, 1, n, pFile);
		}
		fclose(pFile);
	}