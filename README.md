# Pa'lante

## Overview

Pa'lante is a [Forth](https://www.forth.com/starting-forth/) interpreter for the following 8-bit computers:

* The Commodore PET
* The VIC-20 (and TheVIC20 :D)
* The C64 (and TheC64 :D)
* The C128
* The Plus4
* The Commander X16.
* And when it's done, it might also be portable to other 8-bit systems (I'd kind of like to see Apple II and Atari builds, for example)

My goal is to keep the memory size at under 16K.  Yes I could go lower, but the lower you go the less like a "real" Forth you get.  This Git repo is *NOT* my first attempt - oh, there have been many others - so I 've done my research.  I'm all too familiar with those who say you can build a Forth with just 3-4 instructions (`@` `!` and `EXECUTE`).  I could do that, easily, and it would run on just about anything.  But that's more like some kind of strange Assembly REPL than a Forth.  All you can do with that is store binary opcodes in memory and then jump to those addresses.  That's neat, but how do you define new words?  Where's the stack?  Where are `DUP` and `SWAP` and stuff that makes Forth... Forth?  Not that I expect Pa'lante to be fully ANS Forth standard compliant, not that it's gonna have a zillion words out of the box, but I want it to be something a programmer who knows Forth can look at and easily be able to pick up.

## The name

Forth has a rich history of cheesy puns.  With different variants having names like "Blazin' Forth", and countless one-liners like "a Forth to be reconed with" and any opportunity to use words like "Forthcoming" etc.  I love the goofy cheesy fun of it all.  So when I wanted to build my own, I played around with a few names:

* "BreakForth" (cuz Breakaway Forth doesn't sound as cool as Breakaway BASIC - a project I may pick up again someday... or not :D)
* "Skate Forth" (which might end up being my web-based Forth - tho I may not need/want one anymore cuz someone already [beat me to it](https://brendanator.github.io/jsForth/)).

But I'm also fluent in Spanish, and especially familiar with Caribbean (Cuban, Puerto Rican etc.) Spanish slang.  So if you would ask me, how would you translate kingly-sounding phrases like "Let us go Forth" or "I shall go forth"... the translation is obvious.  In "proper" Spanish, Forth would be "para adelante".  But in some places, that get shortened to "para'delante" or (more commonly) just "pa'lante".  This of course reminds me of a few of my favorite songs: [Vamos Pa'lante!](https://www.youtube.com/watch?v=G7TDR9CpQOc), [Voy Pa'lante](https://www.youtube.com/watch?v=hK2wySMe_jc) and even [No Vuelvo Pa'tras](https://www.youtube.com/watch?v=Slng3XclDQ8) ("pa'tras" being the opposite of pa'lante - rough translation is "I'm not turning back").  Asi que vamos pa'lante! :)

## My rough strategy

I've learned *A LOT* about the inner workings of a Forth interpreter this year, made plenty of mistakes, and have at least 4-5 different half-done Forths onmy computer.  The closest I've gotten - at previous attempt at Pa'lante - was following a tutorial on YouTube (which I'll probably include here when I release my first update.  A lot of that code will probably make its way over, but the most important change is going to be how branching works.  In the tutorial, the guy never really introduced the return stack until video... 14?  15?  When it came time to talk branching.  I plan to watch that video and see how he ends up doing it, but I already have a strategy in mind.  I have to get to work, so I won't write it all out like I've been doing here, but the TL;DR is:

```
\ In sudo-code (misspelling intentional, better logically/phonetically) :D
while running:
	get user input.
	push each word to the return stack.
	while return stack is not empty:
		pop return stack to interpreter pointer.
		if the word is a branching word or a user-defined word:
			push it back onto the return stack.
			move the interpreter pointer to the right word.
```

Also, I'm thinking of doing this in very small releases - like maybe version 0.0 will only have numbers, c@ and c!; then maybe 0.1 will add math words, 0.2 will add comparison words, 0.3 will add branching etc.
