#include <stdio.h>
#include <string.h>
#include "def.h"

extern int FreePage(struct PageHdr *PagePtr);
extern struct PageHdr *FetchPage(PAGENO page);
extern void strtolow(char *str);
extern int CompareKeys(char *Key, char *Word);
struct PageHdr* GetLeftbracketLeafpage(PAGENO PageNo, char *key, char *candidate);
void ScanLeafLeftBracket(struct PageHdr *PagePtr, char *key, char *result);

int get_leftbracket(char *key, char *result) {
    strtolow(key);

    // implement me!
    char candidate[MAXWORDSIZE];
    struct PageHdr *PagePtr = GetLeftbracketLeafpage(ROOT, key, candidate);
    ScanLeafLeftBracket(PagePtr, key, result);
    if (strcmp(result, "*NONE*") == 0) {
        strcpy(result, candidate);
    }
    return 0;
}

struct PageHdr* GetLeftbracketLeafpage(PAGENO PageNo, char *key, char *candidate) {
    strcpy(candidate, "*NONE*");

    struct PageHdr *PagePtr = FetchPage(PageNo);
    struct PageHdr *result;
    if (IsLeaf(PagePtr)) { /* found leaf */
        result = PagePtr;
    } else if (PagePtr->NumKeys == 0) {
        assert(IsNonLeaf(PagePtr));
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        result = GetLeftbracketLeafpage(FIRSTLEAFPG, key, candidate);
    } else {
        assert((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys > 0));

        int Result;
        char *Word; /* Key stored in B-Tree */
        struct KeyRecord *KeyListTraverser = PagePtr->KeyListPtr;
        NUMKEYS NumKeys = PagePtr->NumKeys;

        PAGENO ChildPage;
        while (TRUE) {
            Word = KeyListTraverser->StoredKey;
            (*(Word + KeyListTraverser->KeyLen)) = '\0';
            Result = CompareKeys(key, Word);
            --NumKeys;
            if (Result == 2) {
                strcpy(candidate, Word);
                if (NumKeys > 0) {
                    KeyListTraverser = KeyListTraverser->Next;
                } else {
                    ChildPage = PagePtr->PtrToFinalRtgPg;
                    break;
                }
            } else {
                ChildPage = KeyListTraverser->PgNum;
                break;
            }
        }
        char newCandidate[MAXWORDSIZE];
        result = GetLeftbracketLeafpage(ChildPage, key, newCandidate);
        if (strcmp(newCandidate, "*NONE*") != 0) {
            strcpy(candidate, newCandidate);
        }
    }
    if (result != PagePtr) {
        FreePage(PagePtr);
    }
    return result;
}

void ScanLeafLeftBracket(struct PageHdr *PagePtr, char *key, char *result) {
    strcpy(result, "*NONE*");

    PAGENO PageNo = PagePtr->PgNum;
    struct KeyRecord *KeyListTraverser;
    char *word;
    PAGENO FindNumPagesInTree(void);

    while (TRUE) {
        KeyListTraverser = PagePtr->KeyListPtr;
        while (KeyListTraverser) {
            word = KeyListTraverser->StoredKey;
            int Result = CompareKeys(key, word);
            if (Result == 2) {
                strcpy(result, word);
                KeyListTraverser = KeyListTraverser->Next;
            } else {
                FreePage(PagePtr);
                return;
            }
        }
        PageNo = PagePtr->PgNumOfNxtLfPg;
        FreePage(PagePtr);
        if (PageNo < 1 || PageNo > FindNumPagesInTree()) {
            break;
        }
        PagePtr = FetchPage(PageNo);
    }
}