a | b .
a :- b .
b :- a .
c :- a, not b.
b :- a, not c.
:- a, b.

