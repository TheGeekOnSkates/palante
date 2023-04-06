pet:
	clear && cl65 ./*.c -t pet -o palante-pet.prg || exit 1
	rm ./*.o && xpet ./palante-pet.prg
vic:
	clear && cl65 ./*.c -t vic20 -o palante-vic.prg -C vic20-32k.cfg || exit 1
	rm ./*.o && xvic ./palante-vic.prg
c64:
	clear && cl65 ./*.c -t c64 -o palante64.prg || exit 1
	rm ./*.o && x64 ./palante64.prg
