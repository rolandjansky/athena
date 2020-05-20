#!/bin/sh
#  script to copy output file from athenaPT or athenaMT to afs  if it is good
#
if [ $# -ne 2 ]; then
    echo "Syntax: `basename $0` file  reference-directory   "
    exit 1
fi

#
#  replace the pattern "latest" with release name location
#
echo "save_to_refdire.sh: save data file to T0 references area"
printenv CMTPATH
thisrel=`printenv CMTPATH | awk '{ print  substr($1, index($1,"builds/nightlies/")+17, 30 )  }' | awk '{print substr($1,1,index($1,"/Atlas")-1) }' `
# remove any -P1HLT from thisrel
T0rel=`echo $thisrel | sed "s/-P1HLT//"`
# now add the T0 name to get the right place
refdir=`echo $2 | sed "s#/latest#/${T0rel}-T0/latest#"`

# fill in valstr if this is val or not
printenv CMTPATH | grep -q VAL
if [[ $? -eq 0 ]] ; then 
   valstr="VAL" 
else 
   valstr=""
fi 
datestr=`date -I`

echo "this release: $thisrel"
echo "T0rel: ${T0rel}"
echo "new directory: $refdir"

echo "val and date strings:" $valstr $datestr
#
# did athena finish okay?  ( it would be nice to also have the athena status code)
#
#egrep " ERROR "\|" FATAL"\|" CRITICAL"\|"^Exception:"\|"Traceback"\|"Shortened traceback"\|"stack trace"\|"^Algorithm stack:"\|".IncludeError"\|"inconsistent use of tabs and spaces in indentation" atn_test.log &> /dev/null
check_log.py --errors --config checklogTrigP1Test.conf --showexcludestats atn_test.log
testOK=$?
if [[ $testOK -eq 0 ]]
then
    echo "No ERROR or FATAL found in atn_test.log"
    ls -l atn_test.log
else 
    echo "ERROR or FATAL , see atn_test.log"
    ls -l atn_test.log
    # egrep " ERROR "\|" FATAL"\|" CRITICAL"\|"^Exception:"\|"Traceback"\|"Shortened traceback"\|"stack trace"\|"^Algorithm stack:"\|".IncludeError"\|"inconsistent use of tabs and spaces in indentation" atn_test.log  
fi

#
# how big is the output file
#

outputRaw=`readlink -f $1`
size=`ls -s $outputRaw | awk '{print $1}'`

# make sure file is atleat 1 MB (1000kB)
if [[ $size -gt 1000 ]]
then
    echo "File looks size okay: $size"
    ls -l $outputRaw
else
    echo "File too small, not copied, $size"
    ls -l $outputRaw
fi

###  override prevention when not running  in AtlasP1HLT
if [[ $AtlasProject == "AtlasP1HLT" ]] 
then
    echo "project $AtlasProject"
else
    echo "project $AtlasProject, file not copied"
fi 

if [[ $testOK -eq 0 && $size -gt 1000  && $AtlasProject == "AtlasP1HLT"  ]] 
then
   # save only two days of files per Brain request
   purgedepth=2
   echo "removing old data files from ${refdir}"
   find ${refdir}/  -mtime +${purgedepth} -name \*$1
   echo " find ${refdir}\  -mtime +${purgedepth} -name \*$1  -exec rm {} \;"
   find ${refdir}/  -mtime +${purgedepth} -name \*$1  -exec rm {} \;
   echo "copying to references area: ${refdir}"
   cp $outputRaw  ${refdir}/${datestr}${valstr}$1 
   ls -l  ${refdir}/${datestr}${valstr}$1
   exit 0
else
   echo "File not copied"
   exit 1
fi  
