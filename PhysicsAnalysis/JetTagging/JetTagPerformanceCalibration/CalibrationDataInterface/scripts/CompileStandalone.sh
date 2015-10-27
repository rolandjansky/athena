#!/bin/bash

# Note that the use of this script is deprecated; the recommended way to build the library
# is using RootCore, CMT, or its successors

for dir in obj obj/dep ; do
  if ! [ -d $dir ] ; then
    mkdir $dir
  fi
done

FILES="InterfaceBase InterfaceROOT Container EigenVariations Backend Utilities"
HEADERS="${FILES} Variables"

[ -f LinkDef.h ] && rm LinkDef.h
sed -e "s/CalibrationDataInterface\//..\/CalibrationDataInterface\//g" ../Root/LinkDef.h > ./LinkDef.h
for file in ${FILES}; do
  sed -e "s/CalibrationDataInterface\///g" ../Root/CalibrationData${file}.cxx > ./CalibrationData${file}.cxx
 done
for file in ${HEADERS}; do
  sed -e "s/CalibrationDataInterface\///g" ../CalibrationDataInterface/CalibrationData${file}.h > ./CalibrationData${file}.h
done

make
