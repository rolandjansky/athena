#!/bin/bash

if ([ -d ../ReweightUtils ] && [ -d ../src ]); then
  cd ../ReweightUtils
  for file in `ls`; do cat $file | grep -v ClassDef > ${file}_new; mv ${file}_new $file; done
  cd ../src
  for file in `ls`; do cat $file | grep -v ClassImp > ${file}_new; mv ${file}_new $file; done
  sed -n '/void APWeightEntry::Streamer(TBuffer &b)/q;p' APWeightEntry.cxx > APWeightEntry.cxx_new
  mv APWeightEntry.cxx_new APWeightEntry.cxx   
else
  echo "script must be executed from the test directory of the ReweightUtils package. exiting."
  exit 1
fi
