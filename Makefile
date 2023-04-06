pet:
	clear && cl65 ./*.c -t pet -o palante-pet.prg || exit 1
	rm ./*.o && xpet ./palante-pet.prg
