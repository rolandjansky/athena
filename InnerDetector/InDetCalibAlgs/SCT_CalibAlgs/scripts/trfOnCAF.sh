#!/bin/bash

debug=False

#--- Arguments to trfOnCAF.sh
if [ $1 ]; then
    TAG=$1
else
    echo "No tag specified"
fi
if [ $2 ]; then
    DATASET=$2
else
    echo "No input specified"
fi
if [ $3 ]; then
    PART=$3
else
    echo "No routine specified"
    exit -1
fi
if [ $4 ]; then
    CONFIG=$4
else
    CONFIG="NONE"
fi
if [ "$5" == "force" ]; then
    FORCE=0
else
    FORCE=-1
fi

#--- Set input files location
i=0
for part in `echo $DATASET| tr "." "\n"`; do 
    [ $i == 0 ] &&  PROJ=$part
    [ $i == 1 ] &&  RUN=`echo $part|sed 's,^0,,g'`
    [ $i == 2 ] &&  STREAM=$part
    [ $i == 5 ] &&  INPUTTAG=$part
    i=$(( $i+1 ))
done

#strip year value from projectTag
YEAR="20"${PROJ:4:2}

if [ $STREAM == "calibration_SCTNoise" -o $STREAM == "calibration_IDTracks" ]; then
    CASTOR="/castor/cern.ch/grid/atlas/DAQ/"${YEAR}
#    CASTOR="/castor/cern.ch/grid/atlas/DAQ/2012"
    LOC=$CASTOR/0$RUN/$STREAM
else
    CASTOR="/castor/cern.ch/grid/atlas/tzero/prod1/perm"
    LOC=$CASTOR/$PROJ/$STREAM/0$RUN/$DATASET
fi

#--- Check if sample exist
nsls $LOC >/dev/null
if [ $? -ne 0 ]; then
    echo "$DATASET does not exist"
    exit -1
fi

#--- Remove duplicate files
FILES=( `nsls $LOC| tr '\n' ' '` )
NFILES=${#FILES[@]}
let 'NFILES=NFILES-1'
for i in `seq 1 $NFILES`; do
    FILE0=${FILES[i-1]}
    FILE1=${FILES[i]}
    len=${#FILE0}
    BASE0=`echo ${FILE0:0:$len-2}`
    BASE1=`echo ${FILE1:0:$len-2}`
    if [ ${BASE0} == ${BASE1} ]; then
        FILES[i-1]=''
    fi
done
FILES=`echo ${FILES[@]} | tr " " ","`

#--- Check if files are found
if [ ${FILES} == '' ]; then
    echo "Error no input files found on castor"
    exit -1
else
    #--- Check if files are staged
    TMP=''
    MISS=1
    for file in `echo ${FILES}|tr ',' '\n'`;do
        res=`stager_qry -S atlcal -M $LOC/$file | tr ' ' '\n'|tail -1`
        if [ $res == "STAGED" ]; then
            TMP=`echo $TMP,$file`
        else
            echo "file $file not STAGED"
            MISS=0
        fi
    done
    if [ $MISS == 0 ]; then
        if [ $FORCE != 0 ]; then
            echo "Execute the following command to run without these files:"
            echo "./tfOnCAF.sh $TAG $DATASET $PART $CONFIG force"
            exit -1
        else
            echo "WARNING: run transformation on incomplete dataset"
            FILES=${TMP}
        fi
    fi
fi

#--- Input w/ full path
INPUT=""
for file in `echo ${FILES}| tr "," "\n"`; do
    file=$LOC/$file
    INPUT=`echo "${INPUT} $file"`
done
INPUT=`echo ${INPUT}|sed 's; ;,;g'`


#--- Define output tag and task name
if [ $INPUTTAG ]; then
    TAG=${INPUTTAG}_${TAG}
fi
TASKNAME=$PROJ.0$RUN.$STREAM.sct_${PART:2}.SCTCALIB.$TAG

#--- Prefix
PREFIX=$TASKNAME

#--- Define output
if [ $PART == "doNoisyStrip" ]; then
    OUTFILES="${PREFIX}.SCTHitMaps.root,${PREFIX}.SCTLB.root,${PREFIX}.BadStripsAllFile.xml,${PREFIX}.BadStripsNewFile.xml,${PREFIX}.BadStripsSummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doHV" ]; then
    OUTFILES="${PREFIX}.BadModulesFile.xml,${PREFIX}.log"
elif [ $PART == "doDeadChip" ]; then
    OUTFILES="${PREFIX}.SCTHitMaps.root,${PREFIX}.SCTBSErrors.root,${PREFIX}.DeadChipsFile.xml,${PREFIX}.DeadSummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doDeadStrip" ]; then
    OUTFILES="${PREFIX}.SCTHitMaps.root,${PREFIX}.SCTBSErrors.root,${PREFIX}.DeadStripsFile.xml,${PREFIX}.DeadSummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doNoiseOccupancy" ]; then
    OUTFILES="${PREFIX}.NoiseOccupancyFile.xml,${PREFIX}.NoiseOccupancySummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doLorentzAngle" ]; then
    OUTFILES="${PREFIX}.LorentzAngleFile.xml,${PREFIX}.LorentzAngleSummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doRawOccupancy" ]; then
    OUTFILES="${PREFIX}.RawOccupancySummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doEfficiency" ]; then
    OUTFILES="${PREFIX}.EfficiencySummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
elif [ $PART == "doBSErrorDB" ]; then
    OUTFILES="${PREFIX}.BSErrorSummaryFile.xml,${PREFIX}.mycool.db,${PREFIX}.log"
else
    echo "error in specifying routing"
    exit -1
fi

#--- Check if previous job script exists
JOB_SCRIPT=${TASKNAME}.sh
JOB_OUT=${TASKNAME}.o
JOB_ERR=${TASKNAME}.e
if [ -f ${JOB_SCRIPT} ]; then
    echo "removing previous job script"
    rm ${JOB_SCRIPT}
fi

#--- Setting output dir
OUTDIR=/afs/cern.ch/user/s/sctcalib/scratch0/tmp/results/$PROJ/$STREAM/0$RUN/$TASKNAME

#--- Set package info
WORKDIR=/afs/cern.ch/user/s/sctcalib/testarea/AtlasProduction-19.2.0
MYPATH=$WORKDIR/InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/scripts

#--- Set path to CONFIG
if [ $CONFIG == "NONE" ]; then
    CONFIGFILE=$WORKDIR/InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/share/SCTCalibConfig.py
else
    CONFIGFILE=$WORKDIR/InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/run/$CONFIG
fi

#--- Creating job script
cat<<EOF > ${JOB_SCRIPT}
#!/bin/bash
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
source $AtlasSetup/scripts/asetup.sh --inputfile ${WORKDIR}/.asetup
#--- Set path to conditions
export ATLAS_POOLCOND_PATH=/afs/cern.ch/atlas/conditions/poolcond/catalogue
#--- Staging in
export STAGE_SVCCLASS=atlcal
#--- Run sct_calib_trf.py : turn on runInfo.py and runSelector.py in trf
echo "Now running sct_calib_trf.py"
$MYPATH/sct_calib_trf.py input=$INPUT prefix=$PREFIX maxevents=-1 part=$PART doRunSelector=False doRunInfo=True SCTCalibConfig=$CONFIGFILE
ls
#--- Staging out
if [ -d $OUTDIR ]; then
     echo "output dir exists"
else
     mkdir -p $OUTDIR
fi
mv `echo $OUTFILES|tr "," " "` $OUTDIR/
if [ \$? -ne 0 ]; then
     echo "Staging out failed"
     exit -1
else
     echo "Staging out successful"
fi

EOF

chmod +x ${JOB_SCRIPT}
if [ $debug == True ]; then
    echo "********** JOB SCRIPT **************"
    cat ${JOB_SCRIPT}
    echo "**********  THE END   **************"
    rm ${JOB_SCRIPT}
else
#    bsub -q atlasidcal -J SCT.CALIB.$DATASET -eo ${JOB_ERR} -oo ${JOB_OUT} ${JOB_SCRIPT}
    bsub -q atlasb1 -J SCT.CALIB.$DATASET -eo ${JOB_ERR} -oo ${JOB_OUT} ${JOB_SCRIPT}

fi
