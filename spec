Grammar (case insensitive):

<expression> := <expression> ; <expression> | <assignment> | <loop>
<assignment> := <identifier> = <value>
<value> := <value> + <term> | <value> - <term> | <term>
<term> := <number> | <identifier> | <parens>
<parens> := (<value>)
<number> := [0-9]+
<loop> := loop <value> do <expression> end
<identifier> := [a-z][a-z0-9]*

