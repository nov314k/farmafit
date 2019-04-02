# Notes on Design Decisions (desdecs)

Writing out `struct`s in full (eg `struct dp curr` and  `struct app *app`) is
clearer than using typedefs.

Decided to pass `struct app *app` in all but trivial cases since it means
passing only argument (pointer). Maybe review this decision later?

