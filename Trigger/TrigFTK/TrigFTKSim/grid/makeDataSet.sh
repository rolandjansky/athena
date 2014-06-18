#!/bin/sh

if [ $# -lt 2 ]; then
  echo usage: makeDataSet.sh dataset_name source_directory;
  exit -1;
fi

DSname=$1
DIRname=$2

# check that dq2-ls and d2-put exist
if [ ! -x `which dq2-ls` ]; then
    echo ERROR: dq2-ls not available;
    echo which dq2-ls
    which dq2-ls;
    exit -1;
fi
if [ ! -x `which dq2-put` ]; then
    echo ERROR: dq2-put not available;
    echo which dq2-put
    which dq2-put;
    exit -1;
fi

#check that soruce_directory exists
if [ ! -d $DIRname ]; then
    echo ERROR: $DIRname does not exist or is not a directory
    exit -1;
fi

#check DQ2_LOCAL_SITE_ID
if [ ! $DQ2_LOCAL_SITE_ID ]; then
    echo ERROR: \$DQ2_LOCAL_SITE_ID not defined;
    exit -1;
fi

#check that DSname conforms to standards
GRID_USER=AlbertoAnnovi
if [ ! `echo $DSname | grep ^user09\\\\.` ]; then
    echo dataset \"$DSname\" does NOT start with \"user09.\";
    exit -1;
fi
if [ ! `echo $DSname | grep ^user09\\\\.$GRID_USER\\\\.` ]; then
    echo dataset \"$DSname\" does NOT start with \"user09.$GRID_USER.\";
    exit -1;
fi
## if [ ! `echo $DSname | grep _200....._......\\$` ]; then
##     echo dataset \"$DSname\" does NOT end with dateID in form of \"_YYYYMMDD_HHMMSS\";
##     exit -1;
## fi

echo
echo All preliminary checks passed
echo
echo Now rename files
echo

cd $DIRname
for myfile in *; do
    if [ `echo $myfile | grep ^${DSname}\\\\._` ]; then
	echo $myfile is already renamed SKIPPING it;
    else
	mv $myfile ${DSname}._$myfile;
    fi
done
cd -
echo
echo Last 10 files:
echo
ls -lrta $DIRname | tail
echo
echo Total disk usage:
echo
du -sch .
echo
echo Now proceed to store them on $DQ2_LOCAL_SITE_ID.
echo
## echo
## echo You have 10 seconds to CTRL-C now if you don\'t like it
## echo
## sleep 10;

echo dq2-put -a -s $DIRname $DSname
dq2-put -a -s $DIRname $DSname
RETCODE=$?
echo RETCODE=$RETCODE
if [ $RETCODE -eq 0 ]; then
    echo
    echo dq2-put run successfully
    echo Now remember to freeze dataset and request replication as needed
    echo cmd is dq2-freeze-dataset $DSname
    echo;
else
    echo ERROR: RETCODE=$RETCODE;
fi
exit $RETCODE

