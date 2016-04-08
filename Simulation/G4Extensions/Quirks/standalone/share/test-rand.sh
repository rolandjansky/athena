#!/bin/bash
echo /random/setSeeds $RANDOM $RANDOM> in-rand.txt
cat in.txt >> in-rand.txt
../bin/Linux-g++/test < in-rand.txt
