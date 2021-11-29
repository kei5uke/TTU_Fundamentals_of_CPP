#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"
#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ICS0017DataSource.h"


using namespace std;

class DataStructure {
    public:
        DataStructure();
        ~DataStructure();
        DataStructure(const DataStructure& Original);
        DataStructure(char* pFilename);

        char* createFullname(char*, char*);
        void printDataStructure();
        void insertItem(ITEM3*);
        void removeItem(char*);
        bool checkIDformat(char*);
        int GetItemsNumber();
        ITEM3* GetItemPointer(char*);
        void operator+=(ITEM3*);
        void operator-=(char*);
        DataStructure &operator=(const DataStructure&);
        int operator==(DataStructure&);
        void Write(char*);
        friend std::ostream& operator<<(std::ostream& out, DataStructure& tmp) {
            tmp.printDataStructure();
            return out;
        }
    private:
        HEADER_A** _header = NULL;
};

int DataStructure::GetItemsNumber() {
    if (this->_header == NULL) {
        cout << "Error: Structure is missing.\n";
        exit(1);
    }
    HEADER_A* currHEAD = NULL;
    ITEM3* currITEM = NULL;
    int counter = 0;

    for (int i = 0; i < 26; i++) {
        currHEAD = this->_header[i];
        while (currHEAD != nullptr) {
            currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                counter++;
                currITEM = currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
        }
    }
    return counter;
}

ITEM3* DataStructure::GetItemPointer(char* ItemID)
{
    if (this->checkIDformat(ItemID) == false) {
        cout << "Error: Format incorrect.\n";
        exit(1);
    }

    char firstLetter = ItemID[0];
    char secLetter = *(strchr(ItemID, ' ') + 1);
    int index = firstLetter - 'A';
    HEADER_A* currHEAD = this->_header[index];

    if (currHEAD == nullptr) {
        cout << "Error: This ItemID does not exist:" << ItemID << "\n";
        return 0;
    }

    while (currHEAD != nullptr) {
        if (currHEAD->cBegin == secLetter) {
            ITEM3* currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                if (strcmp(currITEM->pID, ItemID) == 0) {
                    return currITEM;
                }
                currITEM = currITEM->pNext;
            }
        }
        currHEAD = currHEAD->pNext;
    }
    return 0;
}

void DataStructure::operator+=(ITEM3* Item)
{
    this->insertItem(Item);
    return;
}

void DataStructure::operator-=(char* ItemID)
{
    this->removeItem(ItemID);
    return;
}

DataStructure& DataStructure::operator=(const DataStructure& Right)
{
    // Delete everything beforehand
    HEADER_A* currHEAD = this->_header[0];
    HEADER_A* delHEAD = nullptr;
    for (int i = 0; i < 26; i++) {
        if (currHEAD == nullptr) {
            delete(currHEAD);
            continue;
        }
        while (currHEAD != nullptr) {
            ITEM3* currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                ITEM3* delITEM = currITEM;
                currITEM = currITEM->pNext;
                delete(delITEM->pID);
                delete(delITEM);
            }
            delHEAD = currHEAD;
            currHEAD = currHEAD->pNext;
            delete(delHEAD->pNext);
        }
    }
    this->_header = nullptr;

    // Create new empy heads
    HEADER_A** ppNewHEAD = new HEADER_A * [26];
    for (int i = 0; i < 26; i++) {
        ppNewHEAD[i] = new HEADER_A;
        ppNewHEAD[i] = nullptr;
    }
    this->_header = ppNewHEAD;


    // reassign right 
    HEADER_A** pDataStructure = Right._header;
    
    for (int i = 0; i < 26; i++) {
        currHEAD = pDataStructure[i];
        while (currHEAD != nullptr) {
            ITEM3* currITEM = (ITEM3*)(currHEAD->pItems);
            while (currITEM != nullptr) {
                this->insertItem(currITEM);
                currITEM = currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
        }
    }
    
    return *this;
}

int DataStructure::operator==(DataStructure& Other)
{   
    if (this->_header == NULL) {
        cout << "Error: Structure is missing.\n";
        exit(1);
    }
    if (Other._header == NULL) {
        cout << "Error: Arg Structure is missing.\n";
        exit(1);
    }

    HEADER_A* currHEAD = NULL;
    HEADER_A* cp_currHEAD = NULL;
    ITEM3* currITEM = NULL;
    ITEM3* cp_currITEM = NULL;
    bool same = false;
    for (int i = 0; i < 26; i++) {
        currHEAD = this->_header[i];
        cp_currHEAD = Other._header[i];
        while (currHEAD != nullptr && cp_currHEAD != nullptr) {
            currITEM = (ITEM3*)currHEAD->pItems;
            cp_currITEM = (ITEM3*)cp_currHEAD->pItems;
            while (currITEM != nullptr && cp_currITEM != nullptr) {
                // Comparison
                if (strcmp(currITEM->pID, cp_currITEM->pID) == 0 &&
                    currITEM->Code == cp_currITEM->Code &&
                    currITEM->Time.Hour == cp_currITEM->Time.Hour &&
                    currITEM->Time.Min == cp_currITEM->Time.Min &&
                    currITEM->Time.Sec == cp_currITEM->Time.Sec) {
                    same = true;
                }
                else {
                    same = false;
                }
                currITEM = currITEM->pNext;
                cp_currITEM = cp_currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
            cp_currHEAD = cp_currHEAD->pNext;
        }
    }
    return int(same);
}

void DataStructure::Write(char* pFilename)
{
    FILE* fp = fopen(pFilename, "wb");

    if (this->_header == NULL) {
        cout << "Error: Structure is missing.\n";
        exit(1);
    }
    HEADER_A* currHEAD = NULL;
    ITEM3* currITEM = NULL;
    int counter = 0;

    for (int i = 0; i < 26; i++) {
        currHEAD = this->_header[i];
        while (currHEAD != nullptr) {
            currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                fprintf(fp, "%s %lu %d %d %d\n",
                    currITEM->pID,
                    currITEM->Code,
                    currITEM->Time.Hour,
                    currITEM->Time.Min,
                    currITEM->Time.Sec);
                currITEM = currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
        }
    }

    fclose(fp);
}

DataStructure::DataStructure()
{
    HEADER_A** ppNewHEAD = new HEADER_A * [26];
    for (int i = 0; i < 26; i++) {
        ppNewHEAD[i] = new HEADER_A;
        ppNewHEAD[i] = nullptr;
    }
    this->_header = ppNewHEAD;
}

DataStructure::~DataStructure()
{   
    HEADER_A* currHEAD = this->_header[0];
    HEADER_A* delHEAD = nullptr;
    for (int i = 0; i < 26; i++) {
        if (currHEAD == nullptr) {
            delete(currHEAD);
            continue;
        }
        while (currHEAD != nullptr) {
            ITEM3* currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                ITEM3* delITEM = currITEM;
                currITEM = currITEM->pNext;
                delete(delITEM->pID);
                delete(delITEM);
            }
            delHEAD = currHEAD;
            currHEAD = currHEAD->pNext;
            delete(delHEAD->pNext);
        }
    }
    delete this->_header;
}

DataStructure::DataStructure(const DataStructure& Original)
{
    if (this->_header != NULL) {
        cout << "Error: Member already exists\n";
        exit(1);
    }

    if (Original._header == NULL) {
        cout << "Error: Original _header does not exits\n";
        exit(1);
    }

    // Create NEW ppHead
    HEADER_A** ppNewHEAD = new HEADER_A * [26];
    for (int i = 0; i < 26; i++)
    {
        ppNewHEAD[i] = new HEADER_A;
        ppNewHEAD[i] = nullptr;
    }
    this->_header = ppNewHEAD;
    
    for (int i = 0; i < 26; i++) {
        HEADER_A* currHEAD = Original._header[i];
        if (currHEAD != nullptr && currHEAD->pItems != nullptr) {
            ITEM3* currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                ITEM3* newITEM = new ITEM3;
                newITEM->pNext = nullptr;
                int n = strlen(currITEM->pID) + 1;
                char* pNewID = new char[n];
                strcpy_s(pNewID, n, currITEM->pID);
                newITEM->pID = pNewID;
                newITEM->Code = currITEM->Code;
                newITEM->Time = currITEM->Time;
                this->insertItem(newITEM);
                currITEM = currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
        }
    }
    return;
}

DataStructure::DataStructure(char* pFilename)
{
    FILE* fp = fopen(pFilename, "rb");
    if (fp == NULL) {
        cout << "Error: File cannot open\n";
        exit(1);
    }
    // Create new heads
    HEADER_A** ppNewHEAD = new HEADER_A * [26];
    for (int i = 0; i < 26; i++) {
        ppNewHEAD[i] = new HEADER_A;
        ppNewHEAD[i] = nullptr;
    }

    this->_header = ppNewHEAD;
    char firstName[30], lastName[30];
    unsigned long int Code;
    int hour, minute, second;
    while (fscanf(fp, "%s %s %lu %d %d %d\n", firstName, lastName, &Code, &hour, &minute, &second) != EOF) {
        char* ID = (char*)malloc(sizeof(char) * (strlen(firstName) + strlen(lastName)));
        ITEM3* newITEM = (ITEM3*)GetItem(3);
        newITEM->pID = this->createFullname(firstName, lastName);
        newITEM->Code = Code;
        newITEM->Time.Hour = hour;
        newITEM->Time.Min = minute;
        newITEM->Time.Sec = second;
        this->insertItem(newITEM);
    }
}

char* DataStructure::createFullname(char* firstName, char* lastName) {
    for (int i = 0; i < strlen(firstName); i++) {
        if (firstName[i] == ' ') {
            firstName[i] = '\0';
            break;
        }
    }
    for (int i = 0; i < strlen(lastName); i++) {
        if (lastName[i] == ' ') {
            lastName[i] = '\0';
            break;
        }
    }
    char* ID = (char*)malloc(sizeof(char) * (strlen(firstName) + strlen(lastName)) + 2);
    sprintf(ID, "%s %s", firstName, lastName);
    return ID;

}

void DataStructure::printDataStructure() {
    if (this->_header == NULL) {
        cout << "Error: Structure is missing\n";
        return;
    }
    HEADER_A* currHEAD = NULL;
    ITEM3* currITEM = NULL;
    int counter = 0;

    for (int i = 0; i < 26; i++) {
        currHEAD = this->_header[i];
        while (currHEAD != nullptr) {
            currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                cout << char('A' + i) << currHEAD->cBegin << "|" <<
                    "No." << setw(2) << counter + 1 << "|" <<
                    "ID:" << setw(30) << currITEM->pID << "|" <<
                    "CODE:" << setw(20) << currITEM->Code << "|" <<
                    "TIME: " << setw(2) << currITEM->Time.Hour << ":" <<
                    setw(2) << currITEM->Time.Min << ":" <<
                    setw(2) << currITEM->Time.Sec << endl;
                counter++;
                currITEM = currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
        }
    }
}

void DataStructure::insertItem(ITEM3* insertITEM) {
    char* pNewItemID = insertITEM->pID;
    if (this->_header == NULL) {
        cout << "Error: Structure is missing\n";
        exit(0);
    }
    if (this->checkIDformat(pNewItemID) == false) {
        cout << "Error: ID format is wrong\n";
        exit(0);
    }

    char firstLetter = pNewItemID[0];
    char secLetter = *(strchr(pNewItemID, ' ') + 1);
    int index = firstLetter - 'A';
    HEADER_A* currHEAD = this->_header[index];
    ITEM3* newITEM = insertITEM;
    ITEM3* currITEM = nullptr;
    
    while (currHEAD != nullptr) {
        if (currHEAD->cBegin == secLetter) {
            currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                if (strcmp(currITEM->pID, pNewItemID) == 0) {
                    cout << "Error: This ID alredy exists(duplicate)" << pNewItemID << "\n";
                    exit(1);
                }
                if (currITEM->pNext == nullptr)break;
                currITEM = currITEM->pNext;
            }
            currITEM->pNext = new ITEM3;
            currITEM->pNext = newITEM;
            return;
        }
        if (currHEAD->pNext == nullptr)break;
        currHEAD = currHEAD->pNext;
    }

    HEADER_A* newHEAD = new HEADER_A;
    newHEAD->cBegin = secLetter;
    newHEAD->pItems = newITEM;
    if (this->_header[index] == nullptr) {
        this->_header[index] = newHEAD;
        return;
    }
    if (currHEAD->pNext == nullptr) {
        currHEAD->pNext = newHEAD;
        return;
    }

    return;
}

bool DataStructure::checkIDformat(char* itemID) {
    if (itemID == NULL) {
        cout << "Error: input missing\n";
        exit(1);
    }

    char firstLetter = {};
    char secLetter = {};
    for (int i = 0; i < int(strlen(itemID)); i++) {
        if (i == 0 && isupper(itemID[i])) {
            firstLetter = itemID[i];
        }
        else if (isspace(itemID[i])) {
            secLetter = itemID[i + 1];
        }
    }
    if (!firstLetter || !secLetter)return false;
    return true;
}

void DataStructure::removeItem(char* ItemID) {
    if (this->_header == NULL) {
        cout << "Error: Structure is missing\n";
        exit(1);
    }
    if (this->checkIDformat(ItemID) == false) {
        cout << "Error: ID format is wrong\n";
        exit(1);
    }

    char firstLetter = ItemID[0];
    char secLetter = *(strchr(ItemID, ' ') + 1);
    int index = firstLetter - 'A';
    ITEM3* currITEM = nullptr;
    HEADER_A* currHEAD = this->_header[index];

    if (currHEAD == nullptr) {
        cout << "This ItemID does not exist:" << ItemID << "\n";
        exit(0);
    }
    
    while (currHEAD != nullptr) {
        if (currHEAD->cBegin == secLetter) {
            currITEM = (ITEM3*)currHEAD->pItems;
            // First node
            if (strcmp(currITEM->pID, ItemID)==0) {
                currHEAD->pItems = currITEM->pNext;
                return;
            }
            // From 2nd node
            while (currITEM->pNext!= nullptr) {
                if (strcmp(currITEM->pNext->pID, ItemID) == 0) {
                    currITEM->pNext = currITEM->pNext->pNext;
                    return;
                }
                currITEM = currITEM->pNext;
            }
        }
        currHEAD = currHEAD->pNext;
    }

    cout << "The ItemID does not exist:" << ItemID << "\n";
    return;
}



int main()
{   
    cout << "--- COURSEWORK 2 ---" << endl;
    DataStructure* dataStructure = new DataStructure();
    for (int i = 0; i < 10; i++) {
        *dataStructure += (ITEM3*)GetItem(3);
    }
    cout << "Original DS:\n" << *dataStructure << endl;
    cout << "Number of items: " << dataStructure->GetItemsNumber() << "\n" << endl;
    cout << "Light Cyan pointer: " << dataStructure->GetItemPointer((char*)"Light Cyan") << "\n" << endl;
    cout << "X X pointer: " << dataStructure->GetItemPointer((char*)"X X") << "\n" << endl;
    DataStructure* copyOfDataStructure = new DataStructure(*dataStructure);
    *dataStructure -= (char*)"Banana Mania";
    *dataStructure -= (char*)"Persian Green";
    *dataStructure -= (char*)"Vegas Gold";
    cout << "DS after renmoving items:\n" << *dataStructure << endl;
    cout << "Comparing old data struct with the copy (1 = they are equal, 0 = they are not equal): " << (*copyOfDataStructure == *dataStructure) << "\n" << endl;
    dataStructure->Write((char*)"C:\\temp\\data.txt");
    DataStructure DSfromFile = DataStructure((char*)"C:\\temp\\data.txt");
    cout << "Comparing old data struct with the one created from a file (1 = they are equal, 0 = they are not equal): " << (*dataStructure == DSfromFile) << "\n" << endl;
    cout << "DS from file:\n" << DSfromFile << endl;
    cout << "COPY OF DS:\n" << *copyOfDataStructure << endl;
    DSfromFile = *copyOfDataStructure;
    cout << "COPY OF DS => DS from file:\n" << DSfromFile << endl;

    return 0;
}


