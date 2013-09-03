#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Sep 07, 2007                            #
#                                                      #
# Description: Second step of trigger rates analysis   #
########################################################

. $TriggerRateTools_PATH/config/config_common.sh
. $TriggerRateTools_PATH/config/config_files/$1

if [ $# -ne 2 ]; then
    echo "[!!] Wrong number of arguments. Please provide exactly 2 arguments."
    echo "     The first argument should be the filename of the configuration"
    echo "     file you wish to use for your sample (located in"
    echo "     TriggerRateTools/config/config_files/)."
    echo ""
    echo "     The second argument should be the full path to the data."
    echo ""
    echo "USAGE example:"
    echo "    ./step2.sh config_ttbar.sh /raid/gaia/users/dufourma/batchJobs/step1/ttbar_2007-Mar-19-12:48"
    echo ""
    echo "[!!] Please consult the documentation in ../../doc/."
    exit
fi

if [ ! -e "$TriggerRateTools_PATH/config/config_files/$1" ]; then
    echo "[!!] The configuration file specified does not exist."
    echo "     If you wish to create a new one it should be placed"
    echo "     in ../../config/config_files/."
    echo ""
    echo "USAGE example:"
    echo "    ./step2.sh config_ttbar.sh /raid/gaia/users/dufourma/batchJobs/step1/ttbar_2007-Mar-19-12:48"
    echo ""
    echo "[!!] Please consult the documentation in ../../doc/."
    exit
fi

if [ ! -e "$2" ]; then
    echo "[!!] The data file specified does not exist."
    echo ""
    echo "USAGE example:"
    echo "    ./step2.sh config_ttbar.sh /raid/gaia/users/dufourma/batchJobs/step1/ttbar_2007-Mar-19-12:48"
    echo ""
    echo "[!!] Please consult the documentation in ../../doc/."
    exit
fi


PYTHON_DIR=$PYTHON_DIR"_"$PREFIX
COUNT=0
MAX_COUNT=0

DUMMY=0

if [ $(( ((MAX_EVENT/NUM_EVENTS_PER_STEP2)*NUM_EVENTS_PER_STEP2)-MAX_EVENT )) -lt 0 ]; then
    MAX_COUNT=$(( (MAX_EVENT/NUM_EVENTS_PER_STEP2) ))
else
    MAX_COUNT=$(( (MAX_EVENT/NUM_EVENTS_PER_STEP2)-1 ))
fi
        

CURR_SKIP_EVENT=0
DATA_PATH=$2
export OUTPUT_PATH=$MAIN_OUTPUT_PATH/step2/$PREFIX"_"$FOLDER


cd $SCRIPT_PATH/step2
pwd
#rm -fr $PYTHON_DIR
mkdir -p $OUTPUT_PATH
mkdir -p $OUTPUT_PATH/$PYTHON_DIR

ls $DATA_PATH | grep AOD | grep .root > temp_data_$PREFIX

cd ../step1
touch $SCRIPT_PATH/step2/conv_data_$PREFIX
python convert_data_files.py $DATA_PATH $SCRIPT_PATH/step2/temp_data_$PREFIX $SCRIPT_PATH/step2/conv_data_$PREFIX
cd ../step2

NUM_THRESHOLDS=`python num_thresholds.py $CONFIG_PATH/thresholds.dat`
CURR_THRESHOLD=1

if [ $NUM_THRESHOLDS -lt 1 ]; then
    NUM_THRESHOLDS=1
    DUMMY=1
fi

if [ `echo $DO_BATCH | grep -c -i "true"` == 1 ]; then
    . $TriggerRateTools_PATH/config/plugins/$BATCH_SYS/setup.sh
fi  

while [ $CURR_THRESHOLD -le $NUM_THRESHOLDS ]; do
    echo ">>>   Working on threshold $((CURR_THRESHOLD))/$((NUM_THRESHOLDS))"
    COUNT=0
    THR_LBL=`echo $CURR_THRESHOLD | awk '{printf("%02d", int($1))}'`
    mkdir -p $OUTPUT_PATH/Thresholds_$THR_LBL
    
    while [ $COUNT -le $MAX_COUNT ]; do
        SUBMIT_JOB=$JOB_CMD
        COUNT_LBL=`echo $COUNT | awk '{printf("%03d", int($1))}'`
        RUN_DIR_SPECIFIC="$RUN_DIR/$PREFIX.$THR_LBL.$COUNT_LBL"
        CURR_SKIP_EVENT=$(((COUNT)*NUM_EVENTS_PER_STEP2))
        NUMBER_OF_EVENTS=$NUM_EVENTS_PER_STEP2
        if [ $((MAX_EVENT-CURR_SKIP_EVENT)) -lt $NUM_EVENTS_PER_STEP2 ]; then
            NUMBER_OF_EVENTS=$((MAX_EVENT-CURR_SKIP_EVENT))
        fi
        cp top.template.py $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "PoolAODInput = [" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        cat conv_data_$PREFIX >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "DetDescrVersion='$DET_DESCR'" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "TrigT1ConfigVersion = \"$TRIG_MENU\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "HLTconfigVersion = \"$TRIG_MENU\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        cat mid1.template.py >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "theApp.EvtMax=$NUMBER_OF_EVENTS" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "EventSelector.SkipEvents = $CURR_SKIP_EVENT" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        if [ $DUMMY -eq 0 ]; then
            python get_thresholds_config.py $CONFIG_PATH/thresholds.dat $CURR_THRESHOLD
            cat temp_thresholds >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        fi
        cat mid2.template.py >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        python get_signatures.py $CONFIG_PATH/signatures.dat
        cat temp_sigs >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "triggerEVAlg.EVTriggerDecisionUserData.CopyAllSignatures=False" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "include (\"EventViewUserData/EVUserDataDump.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "EVUserDataDumpInfAll(triggerEVAlg,Prefix=\"Trigger_\",Labels=[],SelectLabels=[\"Trigger\"])" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "include (\"EventViewUserData/EVScreenDump.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "EVScreenDump(triggerEVAlg,printUDVals=True)" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "include (\"EventViewBuilderAlgs/EventViewAANtupleCreation_jobOptions.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "CreateAANtupleFromUD.Add(triggerEVAlg, \"$OUTPUT_PATH/Thresholds_$THR_LBL/$PREFIX.$THR_LBL.$COUNT_LBL.TD.AAN.root\", CandTree=False, Prefix=\"Trigger\", Create=True)" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "MessageSvc.Format = \"% F%40W%S%7W%R%T %0W%M\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "Service(\"THistSvc\").OutputLevel=WARNING" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "Service(\"StatusCodeSvc\").OutputLevel=WARNING" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py
        echo "triggerEVAlg.OutputLevel=WARNING" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py

        if [ -e "$OUTPUT_PATH/log" ]; then
            echo "nothing" >> /dev/null
        else
            mkdir $OUTPUT_PATH/log
        fi

        if [ -e "$OUTPUT_PATH/out" ]; then
            echo "nothing" >> /dev/null
        else
            mkdir $OUTPUT_PATH/out
        fi

        export CONFIG_FILE="$TriggerRateTools_PATH/config/config_common.sh"
        export FILE="$OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$THR_LBL.$COUNT_LBL.py"
        export JOB_NAME="$PREFIX.$THR_LBL.$COUNT_LBL"
        export COUNT_LBL
        export RUN_DIR_SPECIFIC

        if [ `echo $DO_BATCH | grep -c -i "true"` == 1 ]; then
            . $TriggerRateTools_PATH/config/plugins/$BATCH_SYS/submit.sh 2
        else
            $CONFIG_PATH/job.sh | tee $OUTPUT_PATH/out/$PREFIX.$THR_LBL.$COUNT_LBL.out
        fi


        COUNT=$((COUNT+1))
        echo "Submitted $((COUNT+(MAX_COUNT+1)*(CURR_THRESHOLD-1)))/$(((MAX_COUNT+1)*NUM_THRESHOLDS))"
        sleep $SLEEP
        rm -f temp_thresholds
    done
    CURR_THRESHOLD=$((CURR_THRESHOLD+1))
done

rm -f temp_data_$PREFIX
rm -f conv_data_$PREFIX
rm -f temp_sigs
#cp -r $PYTHON_DIR $OUTPUT_PATH
