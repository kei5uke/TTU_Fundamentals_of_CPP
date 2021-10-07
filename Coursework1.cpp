#include <iostream>
#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ICS0017DataSource.h"

class Data_editor {
    public:
        Data_editor(HEADER_A** p) : _header(p) {}
        void printDataStructure();
        void insertItem(char* pNewItemID);
        void removeItem(char* ItemID);
    private:
        HEADER_A** _header=nullptr;
};

void Data_editor::printDataStructure() {
    if (this->_header == NULL) {
        printf("Error: Structure is missing.\n");
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
                printf("%c%c | NO.%3d | ID: %32s | CODE: %15d | TIME: %2d:%2d:%2d\n",
		       char('A' + i),
		       currHEAD->cBegin,
		       counter + 1,
		       currITEM->pID,
		       currITEM->Code,
		       currITEM->Time.Hour,
		       currITEM->Time.Hour,
		       currITEM->Time.Sec);
                counter++;
                currITEM = currITEM->pNext;
            }
            currHEAD = currHEAD->pNext;
        }
    }
}

void Data_editor::insertItem(char* pNewItemID) {
    if (pNewItemID == NULL) {
        printf("ItemID is missing");
        return;
    }
    if (this->_header == NULL) {
        printf("Error: Structure is missing.\n");
        return;
    }

    // Check Input format
    char firstLetter = {};
    char secLetter = {};
    for (int i = 0; i < int(strlen(pNewItemID)); i++) {
        if (i == 0 && isupper(pNewItemID[i])) {
            firstLetter = pNewItemID[i];
        }
        else if (isspace(pNewItemID[i])) {
            secLetter = pNewItemID[i + 1];
        }
    }
    if (!firstLetter || !secLetter) {
        printf("The format for ItemID is wrong: %s \n", pNewItemID);
        return;
    }

    int index = firstLetter - 'A';
    HEADER_A* currHEAD = this->_header[index];
    ITEM3* newITEM = (ITEM3*)GetItem(3, pNewItemID);
    ITEM3* currITEM = nullptr;
    
    while (currHEAD != nullptr) {
        if (currHEAD->cBegin == secLetter) {
            currITEM = (ITEM3*)currHEAD->pItems;
            while (currITEM != nullptr) {
                if (strcmp(currITEM->pID, pNewItemID) == 0) {
                    printf("Error: This ID alredy exists(duplicate) %s\n", pNewItemID);
                    return;
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

void Data_editor::removeItem(char* ItemID) {
    if (ItemID == NULL) {
        printf("ItemID is missing");
        return;
    }
    if (this->_header == NULL) {
        printf("Error: Structure is missing.\n");
        return;
    }

    // Check Input format
    char firstLetter = {};
    char secLetter = {};
    for (int i = 0; i < int(strlen(ItemID)); i++) {
        if (i == 0 && isupper(ItemID[i])) {
            firstLetter = ItemID[i];
        }
        else if (isspace(ItemID[i])) {
            secLetter = ItemID[i + 1];
        }
    }
    if (!firstLetter || !secLetter) {
        printf("The format for ItemID is wrong: %s \n", ItemID);
        return;
    }

    int index = firstLetter - 'A';
    ITEM3* currITEM = nullptr;
    HEADER_A* currHEAD = this->_header[index];

    if (currHEAD == nullptr) {
        printf("This ItemID does not exist: %s\n", ItemID);
        return;
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

    printf("The ItemID does not exist: %s\n", ItemID);
    return;
}

int main()
{   
    HEADER_A** data = GetStruct3(3, 100);
    Data_editor d(data);

    printf("--ORIGINAL--\n");
    d.printDataStructure();
    printf("--ORIGINAL--\n\n");

    char newID[15][32] = {
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
        "M Z",
        "M Ba",
        "Mba",
    };

    printf("--INSERT ITEM--\n");
    for (int i = 0; i < 15; i++) {
        d.insertItem(newID[i]);
    }
    d.printDataStructure();
    printf("-----------\n\n");

    printf("--REMOVE ITEM--\n");
    for (int i = 0; i < 15; i++) {
        d.removeItem(newID[i]);
    }
    d.printDataStructure();
    printf("-----------\n\n");

    return 0;
}


