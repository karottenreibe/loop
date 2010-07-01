Grammar (case insensitive):

<expression> := <expression> ; <expression> | <assignment> | <loop>
<assignment> := <identifier> = <value>
<value> := <number> | <term> | <parens>
<parens> := (<value>)
<term> := <value> + <value> | <value> - <value>
<number> := [0-9]+
<loop> := loop <value> do <expression> end
<identifier> := [a-z][a-z0-9]*

