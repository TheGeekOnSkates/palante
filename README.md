# It's fun rewrite time! :)

After getting a few branching words working in another project (yes, I'm working
on another Forth lol) I realized a few things:

* I'm overusing the puck out of the return stack, so branching would be hard!
* Pa'lante 0.2 is even slower than BASIC!  Eso no es bueno.  :D
* With every word, I'm having to to WAY too many strcmp's - that could be why.

My idea with this rewrite is:

1. Start out with AGAIN BEGIN C@ C! and + (compile to XTs first)
2. Test the code BEGIN 53280 C@ 1 + 53280 C! AGAIN - the C64 border trick :)
3. If the trick runs faster than version 0.2, add more words and test again
4. Gradually re-add all the words from backup.txt and keep testing
