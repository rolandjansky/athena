#!/bin/bash

sig_file=""
seq_file=""

if [ $# -ne 2 ]; then
    if [ ! -e outputHLTsignature.xml -o ! -e outputHLTsequence.xml ]; then
        echo "[!!] Please either run this script in the folder"
        echo "     containing the outputHLTsignature.xml and"
        echo "     outputHLTsequence.xml,"
        echo "                         OR"
        echo "     provide the full path to the signature and"
        echo "[!!] sequence file (in that order)."
        exit
    fi

    sig_file="outputHLTsignature.xml"
    seq_file="outputHLTsequence.xml"
fi

if [ $# -eq 2 ]; then
    if [ ! -e $1 -o ! -e $2 ]; then
        echo "[!!] The signature or sequence file provided does not exist."
        exit
    fi

    sig_file=$1
    seq_file=$2
fi

last_str=""
curr_str=""
temp=""
count=0
num_sigs=0
num_prescales=0
num_tes=0
num_te_nums=0
sigs[0]="null"
prescales[0]="null"
tes[0]="null"
te_nums[0]="null"

for i in $( grep "<SIGNATURE signature_id=" $sig_file ); do

    temp=`echo $i | awk '/signature_id/'`
    
    if [ $temp ]; then
        sigs[$num_sigs]=`echo $i | awk '{split ($0, a, "\""); print a[2]}'`
        num_sigs=$((num_sigs+1))
    fi

    temp=`echo $i | awk '/prescale/'`
    
    if [ $temp ]; then
        prescales[$num_prescales]=`echo $i | awk '{split ($0, a, "\""); print a[2]}'`
        num_prescales=$((num_prescales+1))
    fi

done

for i in $( grep "TRIGGERELEMENT te_name=" $sig_file ); do
    temp=`echo $i | awk '/te_name/'`
    
    if [ $temp ]; then
        last_str=$curr_str
        curr_str=$temp

        #echo "$last_str =? $curr_str"

        if [ ! "$last_str" = "$curr_str" ]; then
            if [ $count -ne 0 ]; then
                te_nums[$num_te_nums]=`echo $count`
                num_te_nums=$((num_te_nums+1))
            fi
            tes[$num_tes]=`echo "$temp" | awk '{split ($0, a, "\""); print a[2]}'`
            count=1
            num_tes=$((num_tes+1))
        else 
            count=$((count + 1))
        fi
        
    fi

done

te_nums[$num_te_nums]=`echo $count`
num_te_nums=$((num_te_nums+1))

itr=0
while [ $itr -lt $num_sigs ]; do
    echo "--------------------------------------------------------------------------------"
    echo "${sigs[itr]}:"
    echo "    prescale = ${prescales[itr]}"
    echo "    trigger elements = ${te_nums[itr]} X ${tes[itr]}"
    echo "    sequence:"
    echo `./get_sequence.sh $seq_file ${tes[itr]}`
    echo "--------------------------------------------------------------------------------"
    echo ""
    itr=$((itr+1))
done


