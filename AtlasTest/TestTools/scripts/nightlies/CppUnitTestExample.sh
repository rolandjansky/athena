#!/usr/bin/env bash
# MAILTO : undrus@bnl.gov
echo " ======================================================== "
echo " Starting CppUnitTestExample "
echo " ======================================================== "
if [ "$CMTSTRUCTURINGSTYLE" = "without_version_directory" ]; then
cd ${NIGHTLYAREA}/Atlas*Release/cmt
else
cd ${NIGHTLYAREA}/Atlas*Release/*/cmt
fi
cmt broadcast -select=CppUnitExample make CppUnit
stat=$? 
if [ "$stat" != "0"  ]; then
        echo " -------------------------------- "
        echo " FAILURE : test CppUnitTestExample "
        echo " -------------------------------- "
fi


