#!/bin/bash
if [ $# -eq 0 ]
then
  echo   $0 is a script to extract the GUID from a bytestream file.
  echo   Syntax is $0 bytestreamFileName
  exit
fi

myout=`readMetaData ${1} | grep "GUID: "`
if [ ${?} -ne 0 ]
then
  echo   ${1} seems not to contain a GUID
  exit 1
fi
guid=${myout#"GUID: "}

FCregisterPFN -p ${1} -g ${guid} -t BYTE_STREAM
if [ ${?} -ne 0 ]
then
  echo   Could not register ${1} with GUID ${guid}
  exit 1
fi

exit
