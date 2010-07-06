#include <stdio.h>
#include <stdlib.h>

int mainloop(int);

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("This is a LOOP program.\nIt expects a single argument: a positive integer, which will be assigned to the variable `n'.\n");
        return 1;
    } else {
        int arg = atoi(argv[1]);
        int ret = mainloop(arg);
        printf("Program for n=%i evaluated to: %i\n", arg, ret);
        return 0;
    }
}

