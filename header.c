#include <stdio.h>
#include <stdlib.h>

int mainloop(int arg) {
    return arg;
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("Expecting a single argument: a number, which will be assigned to the variable `n'.\n");
        return 1;
    } else {
        int arg = atoi(argv[1]);
        int ret = mainloop(arg);
        printf("Program for n=%i evaluated to: %i\n", arg, ret);
        return 0;
    }
}

