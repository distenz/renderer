#/bin/sh

make clean && make && ./build/main && (imv ./artifact.tga &)
