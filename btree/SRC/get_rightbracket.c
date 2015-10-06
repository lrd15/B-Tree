#include "def.h"

extern int FreePage(struct PageHdr *PagePtr);
extern struct PageHdr *FetchPage(PAGENO page);
extern void strtolow(char *str);
extern int CompareKeys(char *Key, char *Word);
PAGENO GetRightbracketLeafpage(PAGENO PageNo, char *key);
char* ScanLeafRightBracket(PAGENO PageNo, char* key);
int IsPrefix(char *prefix, char *word);

int get_rightbracket(char *key, char *result) {
    strtolow(key);

    // implement me!
    PAGENO LeafPageNo = GetRightbracketLeafpage(ROOT, key);
    char *word = ScanLeafRightBracket(LeafPageNo, key);
    strcpy(result, word);
    return 0;
}

PAGENO GetRightbracketLeafpage(PAGENO PageNo, char *key) {
    PAGENO LeafPageNo;
    struct PageHdr *PagePtr = FetchPage(PageNo);
    if (IsLeaf(PagePtr)) { /* found leaf */
        LeafPageNo = PageNo;
    } else if (PagePtr->NumKeys == 0) {
        assert(IsNonLeaf(PagePtr));
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        LeafPageNo = GetRightbracketLeafpage(FIRSTLEAFPG, key);
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

        LeafPageNo = GetRightbracketLeafpage(ChildPage, key);
    }
    FreePage(PagePtr);
    return LeafPageNo;
}

char* ScanLeafRightBracket(PAGENO PageNo, char* key) {
    struct PageHdr *PagePtr;
    struct KeyRecord *KeyListTraverser;
    char *word;
    PAGENO FindNumPagesInTree(void);

    while (PageNo >= 1 && PageNo <= FindNumPagesInTree()) {
        PagePtr = FetchPage(PageNo);
        KeyListTraverser = PagePtr->KeyListPtr;
        while (KeyListTraverser) {
            word = KeyListTraverser->StoredKey;
            int Result = CompareKeys(key, word);
            if (Result == 2 || IsPrefix(key, word)) {
                KeyListTraverser = KeyListTraverser->Next;
            } else {
                char *result = malloc(strlen(word) + 1);
                strcpy(result, word);
                FreePage(PagePtr);
                return result;
            }
        }
        PageNo = PagePtr->PgNumOfNxtLfPg;
        FreePage(PagePtr);
    }
    return "*NONE*";
}

int IsPrefix(char *prefix, char *word) {
    size_t preLen = strlen(prefix);
    size_t wordLen = strlen(word);
    return preLen > wordLen ? 0 : (strncmp(prefix, word, preLen) == 0);
}