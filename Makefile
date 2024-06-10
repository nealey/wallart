BOARD = --board esp32:esp32:featheresp32

verify: wallart.ino
	arduino --verify $(BOARD) $<
	
install: wallart.ino
	arduino --upload $(BOARD) $<

