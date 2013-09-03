#!/bin/bash

seq_file=""
in=""

if [ $# -eq 1 ]; then
    if [ ! -e outputHLTsequence.xml ]; then
        echo "[!!] Please  provide the full path to the sequence"
        echo "     file, as well as the last trigger element of"
        echo "     the sequence (in that order)."
        echo ""
        echo "     Note that this script is meant to be used by"
        echo "[!!] 'list_signatures.sh'"
        exit
    fi

    seq_file="outputHLTsequence.xml"
    in=$1
elif [ $# -eq 2 ]; then
    if [ ! -e $1 ]; then
        echo "[!!] Please  provide the full path to the sequence"
        echo "     file, as well as the last trigger element of"
        echo "     the sequence (in that order)."
        echo ""
        echo "     Note that this script is meant to be used by"
        echo "[!!] 'list_signatures.sh'"
        exit
    fi

    seq_file=$1
    in=$2

else
    echo "[!!] Please  provide the full path to the sequence"
    echo "     file, as well as the last trigger element of"
    echo "     the sequence (in that order)."
    echo ""
    echo "     Note that this script is meant to be used by"
    echo "[!!] 'list_signatures.sh'"
    exit
fi

out=""
count=0
next=""

for i in `grep -B 2 "output=\"$in\"" $seq_file`; do
    
    count=$((count+1))
    temp=`echo $i | awk '/input/'`
    
    if [ $temp ]; then
        out=`echo $i | awk '{split ($0, a, "\""); print a[2]}'`
        next=`./$0 $seq_file $out`
        echo "$next -> $in"
        
    fi

done

if [ $count -eq 0 ]; then
    echo "$in"
fi

