#!/bin/bash

if [[ "$@" == "" ]]
then
  echo "ERROR : no arguments given (possible AMDB, AGDD, NSWD)"
  exit
fi

echo "preparing SqlProd directory"
rm -fr SqlProd
mkdir SqlProd
cd SqlProd
mkdir SqlFiles PreSql
mkdir AmdcDbCnvSqlToolKit
cd AmdcDbCnvSqlToolKit

echo "getting files via get_files (Athena needs to be set up)"
get_files -scripts ProduceSql.sh main.cc_KEEP makefile_KEEP
cd ..

echo "generating TagsId.txt file according to inputs" $@
touch TagsId.txt

for arg in "$@"
do
  if [[ "$1" == "AMDB" ]]
  then
    echo "adding" $1 "values to TagsId.txt"
    echo "ASZT          107321" >> TagsId.txt
    echo "ACUT          107363" >> TagsId.txt
    echo "ALIN          107365" >> TagsId.txt
    echo "ALMN          107366" >> TagsId.txt
    echo "AMDC          107367" >> TagsId.txt
    echo "APTP          107368" >> TagsId.txt
    echo "ASMP          107369" >> TagsId.txt
    echo "ATLN          107370" >> TagsId.txt
    echo "ATYP          107371" >> TagsId.txt
    echo "AWLN          107372" >> TagsId.txt
    echo "DBAM          107373" >> TagsId.txt
    echo "GGLN          107374" >> TagsId.txt
    echo "WCHV          107375" >> TagsId.txt
    echo "WCMI          107376" >> TagsId.txt
    echo "WCRO          107377" >> TagsId.txt
    echo "WCSC          107378" >> TagsId.txt
    echo "WDED          107379" >> TagsId.txt
    echo "WLBI          107380" >> TagsId.txt
    echo "WMDT          107381" >> TagsId.txt
    echo "WRPC          107382" >> TagsId.txt
    echo "WSPA          107383" >> TagsId.txt
    echo "WSUP          107384" >> TagsId.txt
    echo "WTGC          107385" >> TagsId.txt
    echo "HwSwIdMapping 107409" >> TagsId.txt
    shift
  elif [[ "$1" == "AGDD" ]]
  then
    echo "adding" $1 "values to TagsId.txt"
    echo "AGDD          107364" >> TagsId.txt
    shift
  elif [[ "$1" == "NSWD" ]]
  then
    echo "adding" $1 "values to TagsId.txt"
    echo "NSWD          107386" >> TagsId.txt
    shift
  else
    echo "argument" $1 "not supported for TagsId.txt"
    shift
  fi
done
