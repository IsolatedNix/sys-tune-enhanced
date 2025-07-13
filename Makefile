export GITHASH 		:= $(shell git rev-parse --short HEAD)
export VERSION 		:= 4.0.0
export API_VERSION 	:= 4
export WANT_FLAC 	:= 1
export WANT_MP3 	:= 1
export WANT_WAV 	:= 1

all: overlay nxExt module

clean:
	$(MAKE) -C sys-tune-enhanced/nxExt clean
	$(MAKE) -C overlay clean
	$(MAKE) -C sys-tune-enhanced clean
	-rm -r dist
	-rm sys-tune-enhanced-*-*.zip

overlay:
	$(MAKE) -C overlay

nxExt:
	$(MAKE) -C sys-tune-enhanced/nxExt

module:
	$(MAKE) -C sys-tune-enhanced

dist: all
	mkdir -p dist/switch/.overlays
	mkdir -p dist/atmosphere/contents/4200000000000000
	cp sys-tune-enhanced/sys-tune-enhanced.nsp dist/atmosphere/contents/4200000000000000/exefs.nsp
	cp overlay/sys-tune-enhanced-overlay.ovl dist/switch/.overlays/
	cp sys-tune-enhanced/toolbox.json dist/atmosphere/contents/4200000000000000/
	cd dist; zip -r sys-tune-enhanced-$(VERSION)-$(GITHASH).zip ./**/; cd ../;
	-hactool -t nso sys-tune-enhanced/sys-tune-enhanced.nso

.PHONY: all overlay module
