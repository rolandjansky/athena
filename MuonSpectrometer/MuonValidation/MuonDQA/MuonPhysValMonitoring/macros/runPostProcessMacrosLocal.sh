#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Please give input file!"
    exit
fi

inputfilename=`basename $1 .root`
tmpfile=$(dirname $1)/$(basename $1 .root).PROC.root
cp $1 $tmpfile

## Settings doAverage option will add a TF1 function of a horizontal line
#  showing overall efficiency, resolution, scale, etc.

## Creates efficiency and other ratio plots
python CreateEffAndRecoFracPlots.py $tmpfile #doAverage

## Creates pt resolution and pt scale plots. doBinned option makes res plots for each bin in 2D hists
python CreateResolutionProfiles.py $tmpfile #doAverage doBinned

## Creates profiles of pull plots over pt
python CreatePullProfiles.py $tmpfile #doAverage

## Normalizes all plots to unity (for comparisons). MUST RUN AFTER EFFICIENCY MACROS!!! OTHERWISE EFFICIENCY WILL BE WRONG!!!!
python normalizator.py $tmpfile
 
echo "Created $tmpfile."
