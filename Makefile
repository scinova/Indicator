NAME = IndicatorBLE
CHIP = esp32
BOARD = nodemcu-32s
BUILD_DIR = build
SKETCH = $(NAME).ino
#LIBS = lib
USE_CCACHE = 1
VERBOSE = 1
include ~/makeEspArduino/makeEspArduino.mk

all:
	cp build/${NAME}.bin /sdcard/ArduinoDroid/precompiled
