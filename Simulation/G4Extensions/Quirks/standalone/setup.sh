#!/bin/bash
for i in ../src/*.cxx ../Quirks/*.h ../Quirks/*.icc; do
    ln -fs ../$i src/${i/.cxx/.cc}
done
