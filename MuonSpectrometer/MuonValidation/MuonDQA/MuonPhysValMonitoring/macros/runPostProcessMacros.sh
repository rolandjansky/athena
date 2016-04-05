#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Please give input file!"
    exit
fi

inputfile=$1

cp $inputfile $inputfile.temp.root
tmpfile=$inputfile.temp.root

## Settings doAverage option will add a TF1 function of a horizontal line
#  showing overall efficiency, resolution, scale, etc.

## Creates efficiency and other ratio plots
#CreateEffAndRecoFracPlots.py $tmpfile #doAverage

## Creates pt resolution and pt scale plots. doBinned option makes res plots for each bin in 2D hists
CreateResolutionProfiles.py $tmpfile #doAverage doBinned

## Creates profiles of pull plots over pt
#CreatePullProfiles.py $tmpfile #doAverage

## Normalizes all plots to unity (for comparisons). MUST RUN AFTER EFFICIENCY MACROS!!! OTHERWISE EFFICIENCY WILL BE WRONG!!!!
# normalizator.py $tmpfile

echo "Created $tmpfile."
