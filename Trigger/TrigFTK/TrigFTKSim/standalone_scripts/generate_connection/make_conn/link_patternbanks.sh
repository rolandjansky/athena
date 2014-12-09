#!/bin/bash

source ~/login/utils.sh

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
e_header "Prepping first pattern banks"
echo
echo

e_underline "Checking arguments ... "
if [ -z $1 ]; then
  e_error "Must provide a path to Dataset containing pattern banks"
  usage
else
  DSPATH=$1;
  e_success "Recognized pattern bank datapath as "$DSPATH
fi

shift;

echo
e_underline "Checking $DSPATH for first stage constants"

FOUND=0
for FILE in $(ls $DSPATH); do
  TMP_STR=$(echo $FILE | grep "patterns_raw_8L_")
  if [ -n "${TMP_STR}" ]; then
    e_success "Recognized a file that looks like a 8L sector. Assume the dataset is correct and proceed"
    e_note "file recognized : $FILE"
    FOUND=1
    break
  fi
done

if [ $FOUND -eq 0 ]; then
  e_error "Could not find 8L pattern matching a string 'patterns_raw_8L_' in the dataset $DSPATH"
  exit
fi

echo
e_underline "Creating directory"

e_arrow "create patt/"
mkdir -p patt

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

for FILE in $(ls $DSPATH); do
  TMP_STR=$(echo $FILE | grep "patterns_raw_8L_")
  if [ -n "${TMP_STR}" ]; then
    cd patt/
    NICENAME=$(extract_nicename $TMP_STR patterns)
    ln -sfv ../$DSPATH/$FILE patterns$NICENAME
    cd ../
  fi
done

echo
e_success "DONE"
