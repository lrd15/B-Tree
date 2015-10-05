#include "def.h"

extern int fetchPageCount;
extern int btReadCount;

void printAndResetFetchPageCount() {
    printf("# of reads on B-tree:  %d\n", btReadCount);
    btReadCount = 0;
}