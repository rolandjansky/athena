#!/usr/local/bin/bash
#
# Basic Utility to create standard athena algorithms or tools
# M.Siebel, 05.2006 <Martin.Siebel@CERN.ch>
#
binDir=`dirname $0`
srcDir=`dirname $binDir`
#
#   inquiry section
#
echo
echo
cat $srcDir/introAlgorithmToolSetup.txt
echo
echo "  o-----------------------o" 
echo "  | Do you want to set up |"
echo "  |                       |"
echo "  |  (1)  a Tool          |"
echo "  |  (2)  an Algorithm    |" 
echo "  o-----------------------o" 
echo
isTool=" "
while [  "${isTool}" != "1" -a "${isTool}" != "2" ]   
do 
  read isTool 
  if [ "$isTool" != "1" -a "$isTool" != "2" ]
  then
    echo "   Please enter only '1' or '2' "
  fi
done
algName=""
while [ "$algName" == "" ]
do 
  if [ $isTool = 1 ] 
      then
      echo
      echo "  o-----------------------------o"
      echo "  | Please enter the TOOL NAME: |"
      echo "  o-----------------------------o"
      echo
  else
      echo
      echo "  o----------------------------------o"
      echo "  | Please enter the ALGORITHM NAME: |"
      echo "  o----------------------------------o"
      echo
  fi
  read algName
done
packageName=""
while [ "$packageName" == "" ]
do
  echo 
  echo "  o---------------------------------------------------o"
  echo "  | Please enter the PACKAGE NAME (e.g. RecExCommon): |"
  echo "  o---------------------------------------------------o"
  echo
  read packageName
done 
packageDir=""
while [ "$packageDir" == "" ]
do
  echo
  echo "  o--------------------------------------------------o"
  echo "  | Please enter the PACKAGE PATH beyond '/offline/' |"
  echo "  | and without the new package's directory          |"
  echo "  | (e.g. Reconstruction/RecExample ):               |"
  echo "  o--------------------------------------------------o"
  echo
  read packageDir
done
thisDir=`pwd`
targetDir=$thisDir/$packageDir/$packageName
if [ -d $targetDir ] 
then
    echo
    echo "  The Package $packageDir/$packageName does already exist. Do you want to insert $algName into it? (y/n)"
    echo
    answer=" "
    while [ "$answer" != "y" -a "$answer" != "n" ]
    do
      read answer 
      if [ $answer = "y" ] 
      then
        if [ -e ${targetDir}/${packageName}/${algName}.h ] 
	then
          echo
          echo "  The file $targetDir/$packageName/$algName.h already exists. --> Cancel setup"
          echo "  Have a nice day ..."
	  exit 1
        else
 	  isInsert=1
        fi
      elif [ "$answer" = "n" ] 
      then
        echo
        echo "  Have a nice day ...  bye!"
        exit 1
      else
        echo "  Please answer only with 'y' or 'n' "
      fi
    done
else
    echo
    echo "  The Package $packageDir/$packageName does not exist. Do you want to create it? (y/n)"
    echo
    answer=" "
    while [ "$answer" != "y" -a "$answer" != "n" ]
    do
      read answer 
      if [ "$answer" = "y" ] 
      then
        isInsert=0
      elif [ "$answer" = "n" ] 
      then
        echo
        echo "  Have a nice day ... bye!"
        exit 1
      else
        echo "  Please answer only with 'y' or 'n' "
      fi
    done
fi
answer="n"
nameSpace=""
while [ "$nameSpace" == "" -a "$answer" != "yes" ]
do
  echo
  echo "  o--------------------------------------------------o"
  echo "  | Please enter the NAMESPACE (e.g. Trk,InDet,...): |"
  echo "  o--------------------------------------------------o"
  echo
  read nameSpace
  if [ "$nameSpace" == "" ]
  then
    echo
    echo "  *###################################################*"
    echo "  #                                                   #"
    echo "  #              W A R N I N G !                      #"
    echo "  #                                                   #"
    echo "  #  You did not specify a namespace for your code    #"
    echo "  #  The use of namespaces is                         #"
    echo "  #               ** REQUIRED **                      #"
    echo "  #  by ATLAS C++ Coding Standards.                   #"
    echo "  # (see coding standards document, par. 2.4/NC4)     #"
    echo "  #                                                   #"
    echo "  #     If you are positively sure you want to        #"
    echo "  #     proceed without a namespace type yes,         #"
    echo "  #     type anything else to return and choose       #"
    echo "  #     a namespace.                                  #"
    echo "  #                                                   #"
    echo "  *###################################################*"
    echo
    read answer    
  fi
done
authorName=""
while [ "$authorName" == "" ]
do 
  echo
  echo "  o----------------------------o"
  echo "  | Please enter YOUR NAME:    |"
  echo "  | (not as mail address, that |"
  echo "  |  will be the next question)|"
  echo "  o----------------------------o"
  echo
  read authorName
done
authorMail=""
while [ "$authorMail" == "" ]
do 
  echo
  echo "  o-----------------------------------o"
  echo "  | Please enter YOUR E-MAIL ADDRESS: |"
  echo "  o-----------------------------------o"
  echo
  read authorMail
done 
#
#  create directories
#
if [ $isInsert -eq 0 ] 
then
    echo "  Setting up directory"
    mkdir -p $targetDir
    mkdir -p $targetDir/$packageName
    mkdir -p $targetDir/src
    mkdir -p $targetDir/src/components
    mkdir -p $targetDir/cmt
    mkdir -p $targetDir/doc
fi
#
# copy files
#
echo "  Setting up basic files"
if [ $isTool -eq 1 ] 
then
  type="tool"
  cp $srcDir/interface_tool.templ $targetDir/$packageName/I${algName}.h
else
  type="alg"
fi
cp $srcDir/source_$type.templ $targetDir/src/$algName.cxx
cp $srcDir/header_$type.templ $targetDir/src/$algName.h
if [ $isInsert -eq 0 ] 
then
  cp $srcDir/requirements.templ $targetDir/cmt/requirements
  cp $srcDir/version.templ $targetDir/cmt/version.cmt
  cp $srcDir/mainpage.templ $targetDir/doc/mainpage.h
  cp $srcDir/entries_$type.templ $targetDir/src/components/${packageName}_entries.cxx
  cp $srcDir/load.templ $targetDir/src/components/${packageName}_load.cxx
  cp $srcDir/ChangeLog.templ $targetDir/ChangeLog
else
  # Add to ChangeLog
  mv  $targetDir/ChangeLog $targetDir/ChangeLog.tmp
  cp $srcDir/ChangeLog.templ $targetDir/ChangeLog
  cat $targetDir/ChangeLog.tmp >> $targetDir/ChangeLog
  rm -f $targetDir/ChangeLog.tmp
fi
#
# replace keys
#
cppKey=`echo "${nameSpace}${algName}" | tr "[:lower:]" "[:upper:]"` 
today=`date +%F`
fileList="$targetDir/src/$algName.cxx $targetDir/src/$algName.h  $targetDir/ChangeLog"
if [ $isInsert -eq 0 ] 
then
  fileList="$fileList $targetDir/cmt/requirements $targetDir/cmt/version.cmt  $targetDir/doc/mainpage.h $targetDir/src/components/${packageName}_entries.cxx $targetDir/src/components/${packageName}_load.cxx"
fi
if [ $isTool -eq 1 ] 
then
  fileList="$fileList $targetDir/$packageName/I${algName}.h"
fi
echo "  Editing files: " $fileList
for file in  $fileList 
do 
  cat $file |sed s/@algName@/$algName/g         > ${file}.tmp ; mv -f ${file}.tmp $file
  cat $file |sed s/@packageName@/$packageName/g > ${file}.tmp ; mv -f ${file}.tmp $file
  cat $file |sed s/@authorName@/"$authorName"/g > ${file}.tmp ; mv -f ${file}.tmp $file
  cat $file |sed s/@CppKey@/$cppKey/g           > ${file}.tmp ; mv -f ${file}.tmp $file
  if [ "$nameSpace" != "" ]
  then
    cat $file |sed s/@nameSpace@/$nameSpace/g   > ${file}.tmp ; mv -f ${file}.tmp $file
  else
    cat $file |sed s/@nameSpace@::/" "/g          > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed 's#namespace @nameSpace@#// No NameSpace selected#g'          > ${file}.tmp ; mv -f ${file}.tmp $file
  fi
  cat $file |sed s/@authorMail@/"$authorMail"/g > ${file}.tmp ; mv -f ${file}.tmp $file
  cat $file |sed s/@date@/$today/g              > ${file}.tmp ; mv -f ${file}.tmp $file
  cat $file |sed s/@algName@/$algName/g         > ${file}.tmp ; mv -f ${file}.tmp $file
done
if [ "$nameSpace" == "" ]
then
  if [ $isTool -eq 1 ]
  then
    file="$targetDir/$packageName/$algName.h"
    echo "making adjustments not to use a namespace in $file"
    cat $file |sed '52d'        > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed '16d'        > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed '16,50s/^  //g'        > ${file}.tmp ; mv -f ${file}.tmp $file
    file="$targetDir/$packageName/I$algName.h"
    echo "making adjustments not to use a namespace in $file"
    cat $file |sed '29d'        > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed '11d'        > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed '11,27s/^  //g'        > ${file}.tmp ; mv -f ${file}.tmp $file
    file="$targetDir/src/components/${packageName}_entries.cxx"
    echo "making adjustments not to use a namespace in $file"
    cat $file |sed '4c DECLARE_TOOL_FACTORY(  @algName@ )'  > ${file}.tmp ; mv -f ${file}.tmp $file   
    cat $file |sed '8c DECLARE_TOOL(  @algName@ )'  > ${file}.tmp ; mv -f ${file}.tmp $file   
    cat $file |sed s/@algName@/$algName/g  > ${file}.tmp ; mv -f ${file}.tmp $file   
  else
    file="$targetDir/$packageName/$algName.h"
    echo "making adjustments not to use a namespace in $file"
    cat $file |sed '59d'        > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed '18d'        > ${file}.tmp ; mv -f ${file}.tmp $file
    cat $file |sed '18,57s/^  //g'        > ${file}.tmp ; mv -f ${file}.tmp $file
    file="$targetDir/src/components/${packageName}_entries.cxx"
    echo "making adjustments not to use a namespace in $file"
    cat $file |sed '4c DECLARE_ALGORITHM_FACTORY(  @algName@ )'  > ${file}.tmp ; mv -f ${file}.tmp $file   
    cat $file |sed '7c DECLARE_ALGORITHM(  @algName@ )'  > ${file}.tmp ; mv -f ${file}.tmp $file   
    cat $file |sed s/@algName@/$algName/g  > ${file}.tmp ; mv -f ${file}.tmp $file   
  fi
fi
#
# Printout Goodbye
#
if [ $isInsert -eq 1 ] 
then
  cat $srcDir/exitAlgorithmToolSetup_insert.txt
else 
  cat $srcDir/exitAlgorithmToolSetup.txt
fi



