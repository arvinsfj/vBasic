## vBASIC

### vBASIC Grammar

vBASIC is tiny, so it's grammar is also tiny. The grammar, if you were
wondering, is copied directly off of wikipedia. It is listed in Backus-Naur
form. Hopefully, you are familiar with it. `CR` stands for carriage return,
<kbd>Enter</kbd>.

```
line ::= number statement CR

statement ::= print plist
              let var = expr
              if relation then statement else statement
              for var = expr to expr
              next var
              goto number
              gosub number
              return
              end        
  
plist ::= (str | , | expr)*
relation ::= expr ((< | > | =) expr)*
expr ::= term ((+|-|&||) term)*
term ::= factor ((*|/|%) factor)*
factor ::= var | number | (expr)

var ::= a | b | c ...| y | z
number ::= digit digit*
digit ::= 0 | 1 | 2 | 3 | ... | 8 | 9
str ::= " char* "
char :: = a | b | c ...| y | z ... | A | B | C ... | Y | Z ... | ! | @ | # | $ ... | ( | ) ...
```