#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Please give input file!"
    exit
fi

inputfile=$1

cp $inputfile $inputfile.temp.root
tmpfile=$inputfile.temp.root

## Creates efficiency and other ratio plots
CreateEffAndRecoFracPlots.py $tmpfile

## Creates pt resolution and pt scale plots 
CreateResolutionPlots.py $tmpfile

## Normalizes all plots to unity (for comparisons). MUST RUN AFTER EFFICIENCY MACROS!!! OTHERWISE EFFICIENCY WILL BE WRONG!!!!
#normalizator.py $tmpfile

echo "Created $tmpfile."


