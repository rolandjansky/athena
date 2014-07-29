#!/bin/bash
#___________________________________________________________________________________________
# author: liza.mijovic_at_nospam_cern.ch
# dump PYTUNE tune parameter settings to file TNAME_TINDEX_params.txt
# run like: (chmod +x GET_PYTUNE_params.sh)
# ./GET_PYTUNE_params.sh TNAME TINDEX
# TNAME should be replaced with either ATLAS or PYTUNE
# TINDEX should be replaced by tune's index (ATLAS tunes) or MSTP(5) value (PYTUNE tunes)
#___________________________________________________________________________________________

if [ "$1" == "-h" -o "$1" == "-help" ]; then
    echo ""
    echo "Script prints a list of parameters of a tune that differ from the Pythia defaults." 
    echo ""
    echo "Run like: "      
    echo "./GET_PYTUNE_params.sh TNAME TINDEX [AI]"
    echo ""
    echo "TNAME = tune name; ATLAS or PYTUNE" 
    echo "- ATLAS : query params of ATLAS-internal tune implementation."
    echo "- PYTUNE : query params of Pythia-internal tune implementation." 
    echo ""   
    echo "TINDEX = tune index"
    echo ""
    echo "AI (opt.) : pass anything to enable debug printout of parameter values after Pythia initialization."
    echo ""
    echo "Examples:"
    echo "./GET_PYTUNE_params.sh ATLAS 20100001"
    echo "./GET_PYTUNE_params.sh PYTUNE 327"
    echo "./GET_PYTUNE_params.sh PYTUNE 20100001 AI "
    echo ""
    echo "Parameters are printed to file: TNAME_TINDEX_params.txt."  
    echo ""  
    exit 0 
fi

if [ "$1" != "PYTUNE" -a "$1" != "ATLAS" ] || [ "$2" == "" ]; then
    echo "Invalid script call; it should be e.g.:"
    echo ""
    echo "./GET_TUNE_params.sh ATLAS 20100001"
    echo "./GET_TUNE_params.sh PYTUNE 327"
    echo "" 
    echo "Run ./GET_TUNE_params.sh -h or ./GET_TUNE_params.sh -help for furhter help."    
    exit 0
fi 

if [ "$1" == "PYTUNE" ]; then
    cp PYTUNE_pprintout.py TUNE_pprintout_$2.py
else
    cp ATUNE_pprintout.py TUNE_pprintout_$2.py
fi

perl -e "s/REPNAM/$1/g;" -pi TUNE_pprintout_$2.py
perl -e "s/REPIND/$2/g;" -pi TUNE_pprintout_$2.py

athena.py TUNE_pprintout_$2.py &> tprint_temp.out
grep -A 1599 MSTU\(1\) tprint_temp.out &> tprint_temp2.out
grep 'has the value' tprint_temp2.out &> before_init.out
mv before_init.out tprint_tune1.out
mv after_init.out tprint_tune2.out
rm tprint_temp2.out

athena.py DEF_pprintout.py &> tprint_temp.out
grep -A 1599 MSTU\(1\) tprint_temp.out &> tprint_temp2.out
grep 'has the value' tprint_temp2.out &> before_init.out
mv before_init.out tprint_def1.out
mv after_init.out tprint_def2.out
rm tprint_temp2.out

# tune index should not be set expliciltly [MSTP(5)]
diff tprint_tune1.out tprint_def1.out | grep -v 'MSTP(5)' | grep \< &> tprint_temp1.out
diff tprint_tune2.out tprint_def2.out | grep -v 'MSTP(5)' | grep \< &> tprint_temp2.out

perl -e "s/<//g;" -pi tprint_temp1.out 
perl -e "s/<//g;" -pi tprint_temp2.out
perl -e "s/has the value/=/g;" -pi tprint_temp1.out 
perl -e "s/has the value/=/g;" -pi tprint_temp2.out

# clean old files:
if [[ -s $1_$2_params.txt ]]; then
    rm $1_$2_params.txt
fi
if [[ -s $1_$2_params_afterInit.txt ]]; then
    rm $1_$2_params_afterInit.txt
fi


BAKIFS=$IFS
IFS=$(echo -en "\n\b")
exec 3<&0
exec 0<tprint_temp1.out
nn=0
while read line
do
        let "nn += 1"
        echo $line | tr -d ' ' >> $1_$2_params.txt
done
exec 0<&3

IFS=$BAKIFS

if [ "$3" != "" ]; then 
BAKIFS=$IFS
IFS=$(echo -en "\n\b")
exec 3<&0
exec 0<tprint_temp2.out
nn=0
while read line
do
        let "nn += 1"
        echo $line | tr -d ' ' >> $1_$2_params_afterInit.txt
done
exec 0<&3
fi

echo "The parameters of the $1 tune" $2 "have been printed to file" $1_$2_params.txt
if [ "$3" != "" ]; then
    echo "Parameter diffs after the initialization have been printed to" $1_$2_params_afterInit.txt
fi    

#clean-up:
rm tprint_*.out TUNE_pprintout_$2.py

exit 0