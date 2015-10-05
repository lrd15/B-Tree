#include "def.h"

extern int btReadCount;

void PrintAndResetReadCount() {
    printf("# of reads on B-tree:  %d\n", btReadCount);
    btReadCount = 0;
}