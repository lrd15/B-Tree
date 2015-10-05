#include "def.h"

extern int btReadCount;

void printAndResetReadCount() {
    printf("# of reads on B-tree:  %d\n", btReadCount);
    btReadCount = 0;
}