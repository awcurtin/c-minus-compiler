Andrew Curtin
CS 435
Lab 11 - readme.txt

 Update: 12/16/2016

 The acknowledgment of snowballing bad design decisions under pressure turned
 out to be premonitory for code gen. I didn't leave myself with enough time to
 actually work through it, resulting in a gigantic mess. There are multiple errors
 at this point that I'm no longer able to find, including a segfault brought on
 by tinkering I can't specifically remember, and some weird signed-unsigned
 interaction behavior. I found working on it to be really interesting, but
 ultimately didn't give myself a good enough chance to properly figure it out.

 I did end up completing all of the work myself in this. Given the way code gen went,
 especially with everything ramping up toward the end of the semester, that was probably
 a mistake, at least for the final phase. It was cool figuring out and working on a
 large system of complex programs, and I probably could have benefited from somebody
 else's ideas.

 Everything up through the code gen seems to be handled more or less correctly. I didn't
 get to semantic analysis, but would have liked to have done that. I wanted to tweak the
 the grammar rules to help that out where possible, and go from there.

 The most challenging part, especially with the complexity of the system,
 was trying to account for weird programming behaviors that would still be
 valid in the language. Particularly, I kept coming back, when trying to
 figure out which functionality should be partitioned into which visitor, in
 particular, the example of endlessly cascading compount statements. In C-Minus,
 I can't see any reason to fire up a new scope for no reason, but the grammar
 allows that, and poses some challenges. I think these thoughts led me astray
 in the final stages, spending too much time trying to use niche examples that
 are bad programming practice to hone in on a perfect, generalized solution,
 instead of working more simply and concretely, and building from there.
 This experience reinforced to me the difficulty of language design. Even
 working with a simple grammar and set of possibilities had a number of
 challenges, and it only gets harder the more flexible/powerful a language
 is designed to be.

---
