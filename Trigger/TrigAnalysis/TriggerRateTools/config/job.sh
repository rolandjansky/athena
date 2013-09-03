#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Aug 23, 2007                            #
#                                                      #
# Description: Bash script that is submitted to the    #
#              queue as a job.                         #
########################################################

if [ $# -eq 1 ]; then
    . $1
fi

. $CONFIG_FILE

mkdir -p $RUN_DIR_SPECIFIC

OLD_IFS=$IFS
IFS=:

for cmd in $TRT_ENV; do
    IFS=$OLD_IFS
    echo $cmd
    $cmd
    IFS=:
done

IFS=$OLD_IFS

echo "FILE=$FILE"
OUTPUT_FOLDER=`echo $FILE | awk -F "/$PYTHON_DIR" '{print $1}'`

cd $RUN_DIR_SPECIFIC
date >&2
#athena.py --loglevel ERROR $FILE #| egrep 'AthenaEventLoopMgr|REGTEST'
athena.py $FILE; athena_ec=$? #| egrep 'AthenaEventLoopMgr|REGTEST'
date >&2

if [ ! -e "$OUTPUT_FOLDER/outputLVL1config.xml" ]; then
    if [ -e "./outputLVL1config.xml" ]; then
        cp ./outputLVL1config.xml $OUTPUT_FOLDER/
    fi
fi

if [ ! -e "$OUTPUT_FOLDER/outputHLTconfig.xml" ]; then
    if [ -e "./outputHLTconfig.xml" ]; then
        cp ./outputHLTconfig.xml $OUTPUT_FOLDER/
    fi
fi

sleep 60

cd /
rm -fr $RUN_DIR_SPECIFIC

if [ $athena_ec -ne 0 ]; then

    NEW_ROOT_FILE=`echo $ROOT_FILE | awk -F ".root" '{print $1 ".failed"}' `
    mv $ROOT_FILE $NEW_ROOT_FILE

    echo "Job seems to have failed, ROOT output stored in $NEW_ROOT_FILE as precaution" >&2

fi
