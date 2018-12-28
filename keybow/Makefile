CFLAGS_ALL=-I../libusbgx/include -I../bcm2835-1.57/build/include -L../bcm2835-1.57/build/lib -I../lua-5.3.5/src -L/lib -L../lua-5.3.5/src -lpng -lz -lpthread -llua -lm -lbcm2835 -ldl

keybow: CFLAGS+=-static $(CFLAGS_ALL) -lusbgx -lconfig
keybow: keybow.c lights.c lua-config.c gadget-hid.c
	$(CC)  $^ $(CFLAGS) -o $@


keybow-test: CFLAGS+=-DKEYBOW_DEBUG -DKEYBOW_NO_USB_HID -DKEYBOW_HOME='"."' $(CFLAGS_ALL)
keybow-test: keybow.c lights.c lua-config.c
	$(CC) $^ $(CFLAGS) -o $@
