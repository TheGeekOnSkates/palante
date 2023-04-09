pet:
	clear && cl65 ./*.c -O3 -t pet -o palante-pet.prg || exit 1
	rm ./*.o && xpet ./palante-pet.prg
vic:
	clear && cl65 ./*.c -O3 -t vic20 -o palante-vic.prg -C vic20-32k.cfg || exit 1
	rm ./*.o && xvic ./palante20_MVB0B1B2B3B5.prg
c64:
	clear && cl65 ./*.c -O3 -t c64 -o palante64.prg || exit 1
	rm ./*.o && x64 ./palante64_M6.prg
c128:
	clear && cl65 ./*.c -O3 -t c128 -o palante128.prg || exit 1
	rm ./*.o && x128 ./palante128.prg
plus4:
	clear && cl65 ./*.c -O3 -t plus4 -o palante-plus4.prg || exit 1
	rm ./*.o && xplus4 ./palante-plus4.prg
cbm2:
	clear && cl65 ./*.c -O3 -t cbm2 -o palante-cbm2.prg || exit 1
	rm ./*.o && xcbm2 ./palante-cbm2.prg
x16:
	clear && cl65 ./*.c -O3 -t cx16 -o palante-X16.prg || exit 1
	rm ./*.o && ~/Apps/x16/x16emu -prg ./palante-X16.prg
