SYS_LIBS="-lm -pthread -lva -lz -lbz2"
#"-ldl -lva -ljack -lasound -lSDL -lm -pthread -lbz2 -lz "


FFMPEG_LIBS="libavformat/libavformat.a libavcodec/libavcodec.a libswscale/libswscale.a libavutil/libavutil.a"

gcc -c mplay.c -g -I. -I.. -I./*
#make ffplay
make $FFMPEG_LIBS ADDITIONAL_FLAGS_GIVEN_BY_CMDLINE=-DSTEGANOGRAPHY_STREAM
gcc  mplay.o -o myplay $SYS_LIBS $FFMPEG_LIBS 
