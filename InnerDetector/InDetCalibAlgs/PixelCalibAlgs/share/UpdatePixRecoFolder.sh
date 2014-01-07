#!/bin/bash

#####################################################################
#
# these should be edited to reflect the current situation,
# or provided directly at command line with
#             -start=STARTDBTAG -end=FINALDBTAG
#
# this is the tag used until this moment
STARTDBTAG="PixReco-BLKP-UPD4-000-00"
# this is the new tag to be created
FINALDBTAG="PixReco-BLKP-UPD4-000-00"
#
#####################################################################
#
# this is set automatically
#
FILES="PixelChargeInterpolationData PixelClusterOnTrackErrorData PixelClusterErrorData"
#
#####################################################################

## check if tags have been provided at command line
for OPTS in $*
do
	if [ ${OPTS%=*} == "-start" ]
	then
		STARTDBTAG=${OPTS#*=}
	elif [ ${OPTS%=*} == "-end" ]
	then
		FINALDBTAG=${OPTS#*=}
	fi
done

if [ ${STARTDBTAG} == ${FINALDBTAG} ]
then
	FINALDBTAG_FILENAME=${FINALDBTAG}-new
else
	FINALDBTAG_FILENAME=${FINALDBTAG}
fi

## reading start tag from database
echo -e "\n Reading tag $STARTDBTAG from database..."
athena.py PixelCalibAlgs/ReadPixRecoFolder.py -c "MYTAG='$STARTDBTAG'" \
       	&> ReadPixRecoFolder.log

## checking which constants are unchanged between start and final tags
echo -e "\n Writing tag $FINALDBTAG to local mysql database..."
for FILE in $FILES
do
	OLDFILE=$FILE-${STARTDBTAG#PixelOfflineReco-}
	NEWFILE=$FILE-${FINALDBTAG_FILENAME#PixelOfflineReco-}
	if [ ! -s $NEWFILE ]
	then
		echo -e " ... Copying $OLDFILE to $NEWFILE"
		cp $OLDFILE $NEWFILE
	else	
		echo -e " ... $NEWFILE has been found"
	fi
	if [ ${FINALDBTAG_FILENAME} != ${FINALDBTAG} ]
	then
		echo -e " ... Copying $OLDFILE to $OLDFILE-old"
		cp $OLDFILE $OLDFILE-old
		echo -e " ... Copying $NEWFILE to $FILE-${FINALDBTAG#PixelOfflineReco-}"
		cp $NEWFILE $FILE-${FINALDBTAG#PixelOfflineReco-}
	fi
done	

## writing new tag to local database
athena.py PixelCalibAlgs/WritePixRecoFolder.py -c "MYTAG='$FINALDBTAG'" \
       	&> WritePixRecoFolder.log

## create directory for checks and copy necessary files 
CHECKDIR=ReadFromDB
OLDDIR=$PWD
[ -d $CHECKDIR ] || mkdir $CHECKDIR
cp condobjs.root mycool.db PoolFileCatalog.xml $CHECKDIR
cd $CHECKDIR

## check that local database contains correct constants
echo -e "\n Reading back tag $FINALDBTAG from local mysql database..."
athena.py PixelCalibAlgs/ReadPixRecoFolder.py -c "MYTAG='$FINALDBTAG';DATABASE='LOCAL'" \
       	&> ReadPixRecoFolder.log

## return to start directory 
cd $OLDDIR
echo -e "\n DONE! These files have been used to create tag $FINALDBTAG:\n"
for FILE in $FILES
do
	echo -e " --> $FILE-${FINALDBTAG#PixelOfflineReco-} \n"
done	

## looking for instruction and printing them
for CURRPATH in `echo $DATAPATH | tr ':' ' '`
do
	INSTRUCTION_FILE=$CURRPATH/WritePixRecoFolderINSTRUCTIONS.txt
	if [ -r $INSTRUCTION_FILE ]
	then
		cat $INSTRUCTION_FILE
		break
	fi
done
