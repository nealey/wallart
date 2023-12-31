BOARD = --board adafruit:samd:adafruit_trinket_m0

verify: main.ino
	arduino --verify $(BOARD) $<
	
install: main.ino
	arduino --upload $(BOARD) $<

