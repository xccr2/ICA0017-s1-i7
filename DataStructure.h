#pragma once
#include <iostream>
#include "stdarg.h"
#include "DateTime.h" 
#include "Items.h" 
#include "Headers.h" 
#include "ICS0017DataSource.h"
using namespace std;
enum class IterOperation {Count, List};
enum class LastMatch {Match, NoMatch, Nil};
class DataStructure
{
private:
	HEADER_B* EntryP;
	void CreateBHeader(char, char, ITEM7*);
	void CreateAHeader(char, HEADER_A*, ITEM7*);
	ITEM7* FindItem(char*, bool);
	ITEM7* CopyItem7(ITEM7*);
	// Functions for iteration - so that less copy-pasted code occurs
	void IterItem7(item7*, IterOperation, int&, ITEM7**) const;
	void IterHeaderA(HEADER_A*, IterOperation, int&, ITEM7**) const;
	void IterHeaderB(HEADER_B*, IterOperation, int &, ITEM7**) const;
	void Iterate(IterOperation, int&, ITEM7 **) const;
	void PrintItem7(ITEM7*, int) const;
	void PrintDataStructure() const;
	char* SerializeItem7(ITEM7*, int*);
	void InsertItem(ITEM7*);
	ITEM7** GetItems() const;
public:
	//Empty datastructure
	DataStructure();
	//Read datastructure from file
	DataStructure(char *);
	//Copy from another datastructure
	DataStructure(const DataStructure&);
	~DataStructure();
	int GetItemsNumber() const;
	ITEM7* GetItemPointer(char* pID) { return FindItem(pID, false); }
	void operator+=(ITEM7*);
	void operator-=(char *);
	DataStructure &operator=(const DataStructure&);
	int operator==(DataStructure&);
	void Write(char*);
	//print everything to console
	friend std::ostream& operator<<(std::ostream& out, const DataStructure& list) {
		list.PrintDataStructure();
		return out;
	}
};


