#/bin/sh

make clean && make && ./main && (eog ./artifact.tga &)
