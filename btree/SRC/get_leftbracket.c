#include <stdio.h>
#include <string.h>
#include "def.h"

extern int FreePage(struct PageHdr *PagePtr);
extern struct PageHdr *FetchPage(PAGENO page);
extern void strtolow(char *str);
extern int CompareKeys(char *Key, char *Word);
PAGENO GetLeftbracketLeafpage(PAGENO PageNo, char *key, char *candidate);
char* ScanLeafLeftBracket(PAGENO PageNo, char* key);

int get_leftbracket(char *key, char *result) {
    strtolow(key);

    // implement me!
    char candidate[MAXWORDSIZE];
    PAGENO LeafPageNo = GetLeftbracketLeafpage(ROOT, key, candidate);
    char *word = ScanLeafLeftBracket(LeafPageNo, key);
    if (word) {
        strcpy(result, word);
    } else {
        strcpy(result, candidate);
    }
    return 0;
}

PAGENO GetLeftbracketLeafpage(PAGENO PageNo, char *key, char *candidate) {
    PAGENO LeafPageNo;
    strcpy(candidate, "*NONE*");

    struct PageHdr *PagePtr = FetchPage(PageNo);
    if (IsLeaf(PagePtr)) { /* found leaf */
        LeafPageNo = PageNo;
    } else if (PagePtr->NumKeys == 0) {
        assert(IsNonLeaf(PagePtr));
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        LeafPageNo = GetLeftbracketLeafpage(FIRSTLEAFPG, key, candidate);
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
        LeafPageNo = GetLeftbracketLeafpage(ChildPage, key, newCandidate);
        if (strcmp(newCandidate, "*NONE*") != 0) {
            strcpy(candidate, newCandidate);
        }
    }
    FreePage(PagePtr);
    return LeafPageNo;
}

char* ScanLeafLeftBracket(PAGENO PageNo, char* key) {
    struct PageHdr *PagePtr;
    struct KeyRecord *KeyListTraverser;
    char *word;
    PAGENO FindNumPagesInTree(void);
    char *result = NULL;
    int finished = FALSE;

    while (!finished && PageNo >= 1 && PageNo <= FindNumPagesInTree()) {
        PagePtr = FetchPage(PageNo);
        KeyListTraverser = PagePtr->KeyListPtr;
        while (KeyListTraverser) {
            word = KeyListTraverser->StoredKey;
            int Result = CompareKeys(key, word);
            if (Result == 2) {
                result = word;
                KeyListTraverser = KeyListTraverser->Next;
            } else {
                finished = TRUE;
                break;
            }
        }
        PageNo = PagePtr->PgNumOfNxtLfPg;
        FreePage(PagePtr);
    }
    return result;
}