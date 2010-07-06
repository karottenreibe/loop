# prerequesites #

*   [clang][]

# compile the compiler #

    clang loop.cpp -O2 `llvm-config --cppflags --ldflags --libs core jit native` -o loop

# LOOP grammar #

    <expression> := <expression> ; <expression> | <assignment> | <loop>
    <assignment> := <identifier> = <value>
    <value> := <value> + <term> | <value> - <term> | <term>
    <term> := <number> | <identifier> | <parens>
    <parens> := (<value>)
    <number> := [0-9]+
    <loop> := loop <value> do <expression> end
    <identifier> := [a-z][a-z0-9]*

# special identifiers #

*   the single input parameter to the program is stored in `n`
*   the single output parameter of the program is `f`

# compile a program #

    echo "f = n + 2" | loop | llvm-as | llc > prog.s
    clang prog.s -o prog

# run program #

    ./prog 3

# TODOs #

* make grammar case insensitive


[clang]: http://clang.llvm.org/ "clang -- the better C compiler"

