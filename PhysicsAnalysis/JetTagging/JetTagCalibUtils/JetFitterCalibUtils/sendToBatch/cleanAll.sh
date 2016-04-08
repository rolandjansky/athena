#!/bin/sh
for a in $(ls); do
    echo "Cleaning directory: " $a
    cd $a
    rm -r comb
    rm -r standalone
    rm -r likelihood
    rm -r trainingResultsLikelihood
    cd ..
done