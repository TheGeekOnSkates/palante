# To-Do's

Before calling 0.3 done:

* Get the compiler working again
* Define Forth words in Forth where it makes sense - maybe in a disk image
* Finish updating the README

Known issues:

* If I do i.e. `s" some string" type` it only types half the string.  This is because `S"` is storing the chars in 16-bits, not 8.
* Also related to strings: if (on the PET) I do `32768 c@ emit` (with "Pa'lante 0.3" at the top left) I get a lowercase P, not a capital P.  This is because of ASCII vs. PETSCII confusion in printf("%c")... better way?



Notes / thoughts about version 0.4

With the HUGE optimization done in 0.3, and the addition of 3 branching words, I
kind of wonder about the last two big goals to get to 1.0: files and branching.
Words like IF and DO will take some research, so I don't expect to get on those
anytime soon.  Which means 0.4 will probably revolve around CBM DOS.

Now on that front, I've kinda pivoted a bit from my original idea.  At first I
wanted to just have something like BASIC, like `S" MY-FILE" 8 SAVE` (or `LOAD`).
But BASIC also has wrappers for the insane syntax of advanced disk commands,
like `OPEN 15,8,15,"S0:DELETE-ME":CLOSE 15`.  Forth, of course, can do better!
But I think the smart play would be to do the lower-level words (Kernal calls)
in native code, and then write more user-friendly words in Forth.  For example:

```
15 FP				\ Set the file ID/pointer/whatever (the first 15)
8 DISK				\ Set the disk drive number
15 COMMAND			\ Set the secondary number / disk command (I never got this)
OPENW				\ Open the disk for writing (maybe OPENR for reading)
15 PRINT-HERE		\ Set the output device to be the disk
." This would get printed to an SEQ file or whatever"
S" MY-FILE" SAVE	\ Save to disk
15 CLOSE			\ Also self-explanatory
```
With those 8 words (which I'd have to go over how to use), I could then create a
handful of higher-level words in Forth - not just `LOAD` and `SAVE` (and maybe
`VERIFY`?) but also `DELETE` and words for reading files etc.
