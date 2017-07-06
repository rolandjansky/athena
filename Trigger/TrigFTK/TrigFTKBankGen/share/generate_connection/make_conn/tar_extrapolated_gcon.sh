#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/utils.sh

e_header "tarring newly generated gcon files"

echo

e_underline "checking arguments ..."

if [ -z $1 ]; then
  e_error "you must provide the path to the directory containing newly generated .gcon files"
  exit
else
  DIRPATH=$1; shift;
  e_success "argument recognized : DIRPATH="$DIRPATH
fi

FOUND=0
for FILE in $(ls $DIRPATH); do
  TMP_STR=$(echo $FILE | grep ".gcon" | grep -v "bz2")
  if [ -n "${TMP_STR}" ]; then
    e_success "Recognized a file that looks like a newly generated gcon file. Assume the dataset is correct and proceed"
    e_note "file recognized : $FILE"
    FOUND=1
    break
  fi
done

if [ $FOUND -eq 0 ]; then
  e_error "Could not find file that match 'gcon' in the directory $DIRPATH"
  exit
fi


e_arrow "Linking gcon files"
for FILE in $(ls $DIRPATH); do
  TMP_STR=$(echo $FILE | grep "gcon")
  if [ -n "${TMP_STR}" ]; then
    CHECK_BZIPPED=$(echo ${TMP_STR} | grep "bz2")
    if [ -z ${CHECK_BZIPPED} ]; then
      e_arrow "bzipping" $DIRPATH/$FILE
      bzip2 $DIRPATH/$FILE
      FILE=$FILE.bz2
    fi
    NEWFILENAME=${FILE//812L/11L}
    cd $DIRPATH/../gcon/
    ln -sfv ../patt/$FILE $NEWFILENAME
    cd ..
  fi
done

