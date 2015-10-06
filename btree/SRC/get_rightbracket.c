#include "def.h"

extern int FreePage(struct PageHdr *PagePtr);
extern struct PageHdr *FetchPage(PAGENO page);
extern void strtolow(char *str);
extern int CompareKeys(char *Key, char *Word);
struct PageHdr* GetRightbracketLeafpage(PAGENO PageNo, char *key);
void ScanLeafRightBracket(struct PageHdr *PagePtr, char *key, char *result);
int IsPrefix(char *prefix, char *word);

int get_rightbracket(char *key, char *result) {
    strtolow(key);

    // implement me!
    struct PageHdr *PagePtr = GetRightbracketLeafpage(ROOT, key);
    ScanLeafRightBracket(PagePtr, key, result);
    return 0;
}

struct PageHdr* GetRightbracketLeafpage(PAGENO PageNo, char *key) {
    struct PageHdr *PagePtr = FetchPage(PageNo);
    struct PageHdr *result;
    if (IsLeaf(PagePtr)) { /* found leaf */
        result = PagePtr;
    } else if (PagePtr->NumKeys == 0) {
        assert(IsNonLeaf(PagePtr));
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        result = GetRightbracketLeafpage(FIRSTLEAFPG, key);
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
            if (Result == 2 || IsPrefix(key, Word)) {
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

        result = GetRightbracketLeafpage(ChildPage, key);
    }
    if (result != PagePtr) {
        FreePage(PagePtr);
    }
    return result;
}

/* Need to free the page pointer before return */
void ScanLeafRightBracket(struct PageHdr *PagePtr, char *key, char *result) {
    struct KeyRecord *KeyListTraverser;
    char *word;
    PAGENO FindNumPagesInTree(void);
    PAGENO PageNo = PagePtr->PgNum;

    while (TRUE) {
        KeyListTraverser = PagePtr->KeyListPtr;
        while (KeyListTraverser) {
            word = KeyListTraverser->StoredKey;
            int Result = CompareKeys(key, word);
            if (Result == 2 || IsPrefix(key, word)) {
                KeyListTraverser = KeyListTraverser->Next;
            } else {
                strcpy(result, word);
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
    strcpy(result, "*NONE*");
}

int IsPrefix(char *prefix, char *word) {
    size_t preLen = strlen(prefix);
    size_t wordLen = strlen(word);
    return preLen > wordLen ? 0 : (strncmp(prefix, word, preLen) == 0);
}