CC := g++
CPPFLAGS := -std=gnu++14 -Wall
LDLIBS := -lSDL2_image `sdl2-config --cflags --static-libs` -static
#LDLIBS := -lSDL2_image -lSDL2_ttf -lSDL2_mixer -ljpeg -lpng -lz -ltiff -llzma -lwebp -lfreetype -lharfbuzz -lfreetype -lgraphite2 -lbz2 -l:libmpg123.dll.a -lFLAC -lvorbisfile -lvorbis -logg `sdl2-config --cflags --static-libs` -static

all: main
	mv main simurban
	rm -f *.o
