# Pa'lante
Version 0.1

## Overview

Pa'lante is a [Forth](https://www.forth.com/starting-forth/) interpreter for the following 8-bit computers:

* The Commodore PET (starting in version 0.0)
* The VIC-20 (starting in version 0.1, requires 35K expansion, though if I could decrypt their config-language I might get it working on other VIC setups)
* The C64 (starting in version 0.1)
* The C128 (planned)
* The Plus4 (planned)
* The Commander X16 (planned)
* And when it's done, it might also be portable to other 8-bit systems (I'd kind of like to see Apple II and Atari builds, for example)

My goal is to keep the memory size at under 16K.  Yes I could go lower, but the lower you go the less like Forth you get.  This Git repo is *NOT* my first attempt - oh, there have been many others - so I 've done my research.  I'm all too familiar with those who say you can build a Forth with just 3-4 instructions (`@` `!` and `EXECUTE`).  I could do that, easily, and it would run on just about anything.  But that's more like some kind of strange Assembly REPL than a Forth.  All you can do with that is store binary opcodes in memory and then jump to those addresses.  That's neat, but how do you define new words?  Where's the stack?  Where are `DUP` and `SWAP` and stuff that makes Forth... Forth?  Not that I expect Pa'lante to be fully ANS Forth standard compliant, not that it's gonna have a zillion words out of the box, but I want it to be something a programmer who knows Forth can look at and easily be able to pick up.

## The name

Forth has a rich history of cheesy puns.  With different variants having names like "Blazin' Forth", and countless one-liners like "a Forth to be reconed with" and any opportunity to use words like "Forthcoming" etc.  I love the goofy cheesy fun of it all.  So when I wanted to build my own, I played around with a few names:

* "BreakForth" (cuz Breakaway Forth doesn't sound as cool as Breakaway BASIC - a project I may pick up again someday... or not :D)
* "Skate Forth" (which might end up being my web-based Forth - tho I may not need/want one anymore cuz someone already [beat me to it](https://brendanator.github.io/jsForth/)).

But I'm also fluent in Spanish, and especially familiar with Caribbean (Cuban, Puerto Rican etc.) Spanish slang.  So if you would ask me, how would you translate kingly-sounding phrases like "Let us go Forth" or "I shall go forth"... the translation is obvious.  In "proper" Spanish, Forth would be "para adelante".  But in some places, that get shortened to "para'delante" or (more commonly) just "pa'lante".  This of course reminds me of a few of my favorite songs: [Vamos Pa'lante!](https://www.youtube.com/watch?v=G7TDR9CpQOc), [Voy Pa'lante](https://www.youtube.com/watch?v=hK2wySMe_jc) and even [No Vuelvo Pa'tras](https://www.youtube.com/watch?v=Slng3XclDQ8) ("pa'tras" being the opposite of pa'lante - rough translation is "I'm not turning back").  Asi que vamos pa'lante! :)

## Available words

I'm not going to dive into how Forth works - the link in the "overview" section is really the best place to start.  But what I will do is list the words Pa'lante supports, and extra explanation if they are not "standard" Forth words (like loading/saving files on disk, which of course will be very different on Commodore machines than anything else).

### Standard Forth words

If you know Forth, these do what you think they do. 😆  Note that they are case-sensitive - that is, they're lowercase if you're using the lowercase character set, or upper case if you're using that character set.  I used uppercase in my docs just as a convention, but by default they will be lowercase (not sure how I can fix that - it's a compiler thing).

* **Memory:** `@` `!` `@` `!`
* **Math:** `+` `-` `*` `/` `MOD` `LSHIFT` `RSHIFT` `AND` `OR` `XOR`
* **Stack:** `DUP` `SWAP` `OVER` `.` `.S` `EMIT` `DROP`
* **Misc:** `BYE` `EXECUTE`

### Words only included in Pa'lante

None yet - but I expect a `LOAD` and `SAVE` at least
