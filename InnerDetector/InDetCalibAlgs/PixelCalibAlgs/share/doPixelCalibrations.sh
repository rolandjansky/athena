#!/bin/zsh

#####################################################################
#
#   Configuration 
#
#####################################################################

# if true will use the relevant testarea (instead of shifter area)
# and will output the job script instead of automatically submit it
debug=False

QUEUE="atlasb1"

CASTOR="/castor/cern.ch/grid/atlas/tzero/prod1/perm"
STREAM="express_express"
#STREAM="physics_JetTauEtmiss"
STAGE_CLASS="atlcal"

#the tagtype can be specified as well as a specific tag. 
#If the tagtype only is given, the latest tag of that type will be considered
TAGTYPE="x"
#TAGTYPE="f"

#track collection to be considered
COLLECTION="Validation/ResolvedTracks"
#COLLECTION="Validation/CombinedInDetTracks"
#COLLECTION="Validation/RefittedTracks"

STARTDBTAG="PixelOfflineReco-7TeV-000-05"
FINALDBTAG="PixelOfflineReco-7TeV-000-05-new"

WORKDIR=${PWD}
RELDIR=${CMTPATH%%:*}
if [ $debug = True ]; then
    CALIBHOME=${CMTPATH%%AtlasProduction*}
else
    CALIBHOME="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration"
fi
SETUPTAG="$AtlasVersion,$AtlasProject,here"

#####################################################################
#
# Computing datasets and paths
#
#####################################################################

RUNNUMBERS=("$@")
if [ ${#RUNNUMBERS} -eq 0 ];then
	echo "usage: ./doPixelCalibrations.sh <RUNNUMBER> <RUNNUMBER> ..."
	echo "Possibly available runs: "
	for ProjectTag in `nsls $CASTOR`; do 
		echo $ProjectTag 
		nsls $CASTOR/$ProjectTag/$STREAM/
	done
	exit 127
fi

for RUN in $RUNNUMBERS; do
	for ProjectTag in `nsls $CASTOR`; do
    	if [[ `nsls $CASTOR/$ProjectTag/$STREAM/00$RUN/ 2>/dev/null` != "" ]]; then
				SAMPLE=`nsls $CASTOR/$ProjectTag/$STREAM/00$RUN/|grep  --line-number NTUP_TRKVALID.$TAGTYPE`
				SAMPLE=${SAMPLE##*\:}
				SAMPLES=(${SAMPLES[@]-} $(echo "$SAMPLE"))
    	fi
  done
done 

if  [ ${#SAMPLES} -ne ${#RUNNUMBERS} ]; then
	echo 'Check your run numbers! Data was not found for some of them!'
	unset SAMPLES
	unset RUNNUMBERS
	exit 127
fi

#####################################################################
#
#    Checking for earlier jobs
#
#####################################################################

OUTDIR=$WORKDIR/${SAMPLES[1]}
JOB_SCRIPT=${SAMPLES[1]}.sh
JOB_OUT=${SAMPLES[1]}.out
JOB_ERR=${SAMPLES[1]}.error
JOB_DIR=$WORKDIR/.tmp_${SAMPLES[1]}
#echo $JOB_SCRIPT
if [ -f ${JOB_SCRIPT} ]; then
    echo "moving previous job script ${JOB_SCRIPT} to ${JOB_SCRIPT}.bak"
    mv ${JOB_SCRIPT} ${JOB_SCRIPT}.bak
fi

#####################################################################
#
#      Getting input files
#
#####################################################################

ALLINFILES=""
	for SAMPLE in $SAMPLES; do

		## compute dataset path
		i=0
		for part in `echo $SAMPLE| tr "." "\n"`; do 
	  	  [ $i = 0 ] &&  PROJ=$part
	    	[ $i = 1 ] &&  RUN=`echo $part|sed 's,^,,g'`
	    	[ $i = 2 ] &&  STREAM=$part
	    	[ $i = 5 ] &&  TAG=$part
	    	i=$(( $i+1 ))
		done
		LOC=$CASTOR/$PROJ/$STREAM/$RUN
		
		nsls $LOC/$SAMPLE >/dev/null
		if [ $? -ne 0 ]; then
	  	echo "Sample $SAMPLE does not exist"
			echo "usage: ./doPixelCalibrations.sh <RUNNUMBER> <RUNNUMBER> ..."
	    exit -1
		fi
		
		## remove duplicate files
		FILES=( `nsls $LOC/$SAMPLE| tr '\n' ' '` )
		NFILES=${#FILES[@]}
		let 'NFILES=NFILES-1'
		for i in `seq 1 $NFILES`; do
	    FILE0=${FILES[i-1]}
	    FILE1=${FILES[i]}
	    len=${#FILE0}
	    BASE0=`echo ${FILE0[0,$len-2]}`
	    BASE1=`echo ${FILE1[0,$len-2]}`
	    if [[ ${BASE0} = ${BASE1} ]]; then
				FILES[i-1]=''
	    fi
		done

		## check if files are found
		FILES=`echo ${FILES[@]} | tr " " ","`
		if [[ ${FILES} = '' ]]; then
	  	  echo "Error no input files found on castor"
	    	exit -1
		else
	    ## check that files are staged
	    TMP=''
	    MISS=1
			ONE=0
	    for file in `echo ${FILES}|tr ',' '\n'`;do
				res=`stager_qry -S $STAGE_CLASS -M $LOC/$SAMPLE/$file | tr ' ' '\n'|tail -1`
				if [ $res = "STAGED" ]; then
		    	TMP=`echo $TMP,$file`
					ONE=1
				else
		    	echo "file $file not STAGED on $STAGE_CLASS"
		    	MISS=0
				fi
	    done
	    if [ $MISS = 0 -a $ONE = 1 ]; then
		    echo "WARNING: run on incomplete dataset"
		    FILES=${TMP}
	    fi
			if [ $MISS = 0 -a $ONE = 0 ]; then
		    echo "ERROR: no files staged for this run"
				exit 127
			fi
		fi

		## get a list of all files (with full path)
		for file in `echo ${FILES}| tr "," "\n"`; do
	  	  #ALLINFILES=`echo "${ALLINFILES} $LOC/$SAMPLE/$file"`
	    	ALLINFILES=`echo "$LOC/$SAMPLE/$file\n${ALLINFILES}"`
	done
done

#####################################################################
#
#      Creating script
#
#####################################################################

cat<<EOF > ${JOB_SCRIPT}
#!/bin/bash

#configuring correct release and package
cd $RELDIR
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
source $AtlasSetup/scripts/asetup.sh $SETUPTAG
if [ -d $JOB_DIR ]; then
     echo "Job already running!"
		 exit 127
else
     mkdir -p $JOB_DIR
fi
cd $JOB_DIR

#getting old tag constants 
athena.py PixelCalibAlgs/ReadPixRecoFolder.py -c "MYTAG='$STARTDBTAG'" &> ReadPixRecoFolder.log

#prepare filelist
export STAGE_SVCCLASS=$STAGE_CLASS
echo "${ALLINFILES}" > filelist

#run program
doPixelOfflineCalib.exe $STARTDBTAG $FINALDBTAG $COLLECTION filelist

#staging out
if [ -d $OUTDIR ]; then
     echo "output dir exists"
else
     mkdir -p $OUTDIR
fi
mv *Pixel* PoolFileCatalog*  ReadPixRecoFolder.log filelist logfile $OUTDIR
rm -rf $JOB_DIR

EOF

chmod +x ${JOB_SCRIPT}
if [ $debug = True ]; then
    #echo "********** JOB SCRIPT **************"
    #cat ${JOB_SCRIPT}
    #echo "**********  THE END   **************"
		echo "To continue check jobscript ${JOB_SCRIPT} carefully, then submit"
    echo "bsub -q $QUEUE -J Pixel.CALIB.$SAMPLE -eo ${JOB_ERR} -oo ${JOB_OUT} ${JOB_SCRIPT}"
else
    bsub -q $QUEUE -J Pixel.CALIB.$SAMPLE -eo ${JOB_ERR} -oo ${JOB_OUT} ${JOB_SCRIPT}
fi

unset SAMPLES
unset RUNNUMBERS

