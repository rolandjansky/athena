#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/utils.sh

usage() {
  echo "Usage -"
  echo ""
  echo "   $0 [DATASET_FIRSTSTAGE] [DATASET_SECONDSTAGE]"
  echo ""
  echo ""
  echo ""
  echo "  for more information consult : https://twiki.cern.ch/twiki/bin/viewauth/Atlas/FTKSimShortTutorial#Generating_connection_files"
  exit
}

echo
e_header "Prepping first stage banks for second stage bank generation"
echo
echo

e_underline "Checking arguments ... "
if [ -z $1 ]; then
  e_error "Must provide a path to Dataset containing first stage banks"
  usage
else
  DSPATH_1=$1;
  e_success "Recognized first stage bank datapath as "$DSPATH_1
fi

if [ -z $2 ]; then
  e_error "Must provide a path to Dataset containing second stage banks"
  usage
else
  DSPATH_2=$2;
  e_success "Recognized second stage bank datapath as "$DSPATH_2
fi

shift; shift;

echo
e_underline "Checking $DSPATH_1 for first stage constants"

FOUND=0
for FILE in $(ls $DSPATH_1); do
  TMP_STR=$(echo $FILE | grep "corrgen_raw_8L_reg")
  if [ -n "${TMP_STR}" ]; then
    e_success "Recognized a file that looks like a 8L sector. Assume the dataset is correct and proceed"
    e_note "file recognized : $FILE"
    FOUND=1
    break
  fi
done

if [ $FOUND -eq 0 ]; then
  e_error "Could not find 8L sector matching a string 'corrgen_raw_8L_reg' in the dataset $DSPATH_1"
  exit
fi

echo
e_underline "Checking $DSPATH_2 for second stage constants"

FOUND=0
for FILE in $(ls $DSPATH_2); do
  TMP_STR=$(echo $FILE | grep "corrgen_raw_11L_reg")
  if [ -n "${TMP_STR}" ]; then
    e_success "Recognized a file that looks like a 12L sector. Assume the dataset is correct and proceed"
    e_note "file recognized : $FILE"
    FOUND=1
    break
  fi
done

if [ $FOUND -eq 0 ]; then
  e_error "Could not find 8L sector matching a string 'corrgen_raw_11L_reg' in the dataset $DSPATH_2"
  exit
fi

echo
e_underline "Creating directory"

e_arrow "create patt/"
mkdir -p patt
e_arrow "create gcon/"
mkdir -p gcon

echo
e_underline "Linking files ..."

extract_nicename() {
  TMP_STR=$1
  PATTERN=$2
  echo ${TMP_STR##*$PATTERN}
  #$(expr match "$TMP_STR" "$PATTERN")
  #for i in $(seq 1 5); do
  #  INDEX=$(expr index $TMP_STR ".")
  #  TMP_STR=${TMP_STR:$INDEX}
  #done
  #echo $TMP_STR
}

for FILE in $(ls $DSPATH_1); do
  TMP_STR=$(echo $FILE | grep "sectors_raw_8L_reg")
  if [ -n "${TMP_STR}" ]; then
    cd patt/
    NICENAME=$(extract_nicename $TMP_STR sectors)
    ln -sfv ../$DSPATH_1/$FILE sectors$NICENAME
    cd ../
  fi
  TMP_STR=$(echo $FILE | grep "corrgen_raw_8L_reg")
  if [ -n "${TMP_STR}" ]; then
    cd gcon/
    NICENAME=$(extract_nicename $TMP_STR corrgen)
    ln -sfv ../$DSPATH_1/$FILE corrgen$NICENAME
    cd ../
  fi
done


for FILE in $(ls $DSPATH_2); do
  TMP_STR=$(echo $FILE | grep "sectors_raw_8L_reg")
  if [ -n "${TMP_STR}" ]; then
    cd patt/
    NICENAME=$(extract_nicename $TMP_STR sectors)
    ln -sfv ../$DSPATH_2/$FILE sectors$NICENAME
    cd ../
  fi
  TMP_STR=$(echo $FILE | grep "corrgen_raw_11L_reg")
  if [ -n "${TMP_STR}" ]; then
    cd gcon/
    NICENAME=$(extract_nicename $TMP_STR corrgen)
    ln -sfv ../$DSPATH_2/$FILE corrgen$NICENAME
    cd ../
  fi
done

echo
e_success "DONE"
