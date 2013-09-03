#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Sep 07, 2007                            #
#                                                      #
# Description: First step of trigger rates analysis    #
########################################################


. $TriggerRateTools_PATH/config/config_common.sh
. $TriggerRateTools_PATH/config/config_files/$1

if [ $# -ne 2 ]; then
    if [ $# -ne 1 ]; then
        echo "[!!] Wrong number of arguments. Please provide exactly 2 arguments."
        echo "     The first argument should be the filename of the configuration"
        echo "     file you wish to use for your sample (located in"
        echo "     TriggerRateTools/config/config_files/)."
        echo ""
        echo "     The second argument should be the full path to the data."
        echo ""
        echo "USAGE example:"
        echo "    ./step1.sh config_ttbar.sh /raid/gaia/atlas/data/mc/cern\_ttbar"
        echo ""
        echo "[!!] Please consult the documentation in ../../doc/."
        exit
    fi
fi

if [ ! -e "$TriggerRateTools_PATH/config/config_files/$1" ]; then
    echo "[!!] The configuration file specified does not exist."
    echo "     If you wish to create a new one it should be placed"
    echo "     in ../../config/config_files/."
    echo ""
    echo "USAGE example:"
    echo "    ./step1.sh config_ttbar.sh /raid/gaia/atlas/data/mc/cern\_ttbar"
    echo ""
    echo "[!!] Please consult the documentation in ../../doc/."
    exit
fi

if [ $# -ne 1 ]; then
    if [ ! -e "$2" ]; then
        echo "[!!] The data file specified does not exist."
        echo ""
        echo "USAGE example:"
        echo "    ./step1.sh config_ttbar.sh /raid/gaia/atlas/data/mc/cern\_ttbar"
        echo ""
        echo "[!!] Please consult the documentation in ../../doc/."
        exit
    fi
    DATA_PATH=$2
fi

PYTHON_DIR=$PYTHON_DIR"_"$PREFIX
export MAX_COUNT=0

export COUNT=0
#CURR_SKIP_EVENT=0

export OUTPUT_PATH=$MAIN_OUTPUT_PATH/step1/$PREFIX"_"$FOLDER


cd $SCRIPT_PATH/step1
#rm -fr $PYTHON_DIR
mkdir -p $OUTPUT_PATH
mkdir -p $OUTPUT_PATH/$PYTHON_DIR
mkdir -p $OUTPUT_PATH/TriggerRates
#mkdir -p $OUTPUT_PATH/CBNT

if [ `echo $DO_BATCH | grep -c -i "true"` == 1 ]; then
    . $TriggerRateTools_PATH/config/plugins/$BATCH_SYS/setup.sh
fi      

echo "Acquiring list of files"
if [ -n "$DATA_FILE" ]; then
    echo "DATA_FILE = $DATA_FILE"
        cat $DATA_FILE > temp_data_$PREFIX
elif [ -n "$DATA_PATH" ]; then
    echo "DATA_PATH = $DATA_PATH"
    ls $DATA_PATH | grep ".pool.root" > temp_data_$PREFIX
elif [ -n "$NON_LOCAL_ID" ]; then
    echo "NON_LOCAL_ID = $NON_LOCAL_ID"
    retrieve_data="$LS_CMD $NON_LOCAL_ID"
    $retrieve_data | grep ".pool.root" > temp_data_$PREFIX
fi

NUM_FILES=`cat temp_data_$PREFIX | wc -l | awk '{print $1}'`
MIN_FILE=0
MAX_FILE=$NUM_RDOS_PER_STEP1

if [ $NUM_FILES -gt $MAX_NUM_FILES ]; then
    NUM_FILES=$MAX_NUM_FILES
fi

if [ $NUM_RDOS_PER_STEP1 -ge 1 ]; then

    if [ $(( ((NUM_FILES/NUM_RDOS_PER_STEP1)*NUM_RDOS_PER_STEP1)-NUM_FILES )) -lt 0 ]; then
        export MAX_COUNT=$(( (NUM_FILES/NUM_RDOS_PER_STEP1) ))
    else
        export MAX_COUNT=$(( (NUM_FILES/NUM_RDOS_PER_STEP1)-1 ))
    fi

fi

# Main loop over all events or files
while [ $COUNT -le $MAX_COUNT ] 
do
    touch conv_data_$PREFIX
    COUNT_LBL=`echo $COUNT | awk '{printf("%03d", int($1))}'`

    RUN_DIR_SPECIFIC="$RUN_DIR/$PREFIX.$COUNT_LBL"
    
    cp top.template.py $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py

    echo "PoolRDOInput = [" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    
    MIN_FILE=$(( COUNT*NUM_RDOS_PER_STEP1 ))
    MAX_FILE=$(( (COUNT+1)*NUM_RDOS_PER_STEP1 ))
    if [ $MAX_FILE -gt $NUM_FILES ]; then
        MAX_FILE=$(( NUM_FILES))
    fi
    if [ -n "$DATA_PATH" ]; then
        if [ -n "$PATH_PREFIX" ]; then
    	#echo "data path; prefix"
        	python convert_data_files.py $DATA_PATH $PATH_PREFIX temp_data_$PREFIX conv_data_$PREFIX $MIN_FILE $MAX_FILE
        else
    	#echo "data path; no prefix"
        	python convert_data_files.py $DATA_PATH none temp_data_$PREFIX conv_data_$PREFIX $MIN_FILE $MAX_FILE
        fi
    else
        if [ -n "$PATH_PREFIX" ]; then
    	#echo "no data path; prefix"
            if [ `echo $USE_FULL_PATH | grep -c -i "true"` == 1 ]; then
                python convert_data_files.py $NON_LOCAL_ID $PATH_PREFIX temp_data_$PREFIX conv_data_$PREFIX $MIN_FILE $MAX_FILE
            else
                python convert_data_files.py none $PATH_PREFIX temp_data_$PREFIX conv_data_$PREFIX $MIN_FILE $MAX_FILE
            fi
        else  
    	#echo "no data path; no prefix"
            if [ `echo $USE_FULL_PATH | grep -c -i "true"` == 1 ]; then
        	python convert_data_files.py $NON_LOCAL_ID none temp_data_$PREFIX conv_data_$PREFIX $MIN_FILE $MAX_FILE
            else
        	python convert_data_files.py none none temp_data_$PREFIX conv_data_$PREFIX $MIN_FILE $MAX_FILE
            fi
        fi
    fi

    cat conv_data_$PREFIX >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    #echo "PoolESDOutput= \"$OUTPUT_PATH/$PREFIX.$COUNT_LBL.ESD.root\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "#PoolAODOutput= \"$OUTPUT_PATH/$PREFIX.$COUNT_LBL.AOD.root\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    #echo "RootNtupleOutput= \"$OUTPUT_PATH/CBNT/$PREFIX.$COUNT_LBL.CBNT.root\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "DetDescrVersion=\"$DET_DESCR\"" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "include (\"RecExCommon/RecExCommon_flags.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.readHLTconfigFromXML=False" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.readLVL1configFromXML=False" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.Slices_all_setOff()" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.doLVL1=$DO_LVL1" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.doLVL2=$DO_LVL2" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.doEF=$DO_EF" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.disableRandomPrescale=$DISABLE_RANDOM" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "TriggerFlags.triggerMenuSetup = '$TRIGGER_MENU_SETUP'" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    if [ -n "$TRIGGER_PRESCALES_LVL1" ]; then
        echo "TriggerFlags.L1PrescaleSet = '$TRIGGER_PRESCALES_LVL1'" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    fi
    if [ -n "$TRIGGER_PRESCALES_HLT" ]; then
        echo "TriggerFlags.HLTPrescaleSet = '$TRIGGER_PRESCALES_HLT'" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    fi
    echo "TriggerFlags.Lvl1.RemapThresholdsAsListed = True" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py

    echo "def addAlwaysAcceptChain():" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "    TriggerFlags.MinBiasSlice.enableSignature(\"always\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "from TriggerMenuPython.GenerateMenu import GenerateMenu" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
        
    #echo "include(\"TriggerRateTools/TriggerRateTools_jetTruthVetoConfigRDO.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "include(\"RecExCommon/RecExCommon_topOptions.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "include(\"TriggerRateTools/TriggerRateTools_common.py\")" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "ServiceMgr.THistSvc.Output += [\"TriggerRateTools DATAFILE='$OUTPUT_PATH/TriggerRates/$PREFIX.$COUNT_LBL.TD.AAN.root' OPT='RECREATE'\"]" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py

    #echo "triggerRateTools.doRawTD = False" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    #echo "triggerRateTools.doTextOutput = False" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    #echo "triggerRateTools.doSlicesInfo = False" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py

    echo "triggerRateTools.xSection = $XSECTION" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    echo "triggerRateTools.Luminosity = $LUMINOSITY" >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py

    cat cplx_sigs_config.py >> $OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py
    
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
    export FILE="$OUTPUT_PATH/$PYTHON_DIR/$PREFIX.$COUNT_LBL.py"
    export JOB_NAME="$PREFIX.$COUNT_LBL"
    export ROOT_FILE="$OUTPUT_PATH/TriggerRates/$PREFIX.$COUNT_LBL.TD.AAN.root"
    export COUNT_LBL
    export RUN_DIR_SPECIFIC

    if [ `echo $DO_BATCH | grep -c -i "true"` == 1 ]; then
        . $TriggerRateTools_PATH/config/plugins/$BATCH_SYS/submit.sh 1
    else
        $CONFIG_PATH/job.sh | tee $OUTPUT_PATH/out/$PREFIX.$COUNT_LBL.out
    fi

    export COUNT=$((COUNT+1))
    echo "Submitted $((COUNT))/$((MAX_COUNT+1))"
    sleep $SLEEP
    rm -f conv_data_$PREFIX
done

cp $TriggerRateTools_PATH/examples/ROOT_to_XML.C $OUTPUT_PATH
cp $TriggerRateTools_PATH/examples/ROOT_to_HTML.C $OUTPUT_PATH
cp $TriggerRateTools_PATH/examples/CombineJobs.C $OUTPUT_PATH/TriggerRates
cp $TriggerRateTools_PATH/steps/step3/split_combine.sh $OUTPUT_PATH/TriggerRates
cp $CONFIG_PATH/xslt/trigger_rates.xsl $OUTPUT_PATH
        

touch $OUTPUT_PATH/finalize.sh
echo "#!/bin/bash" >> $OUTPUT_PATH/finalize.sh
echo "rm -f TriggerRates.root" >> $OUTPUT_PATH/finalize.sh
echo "rm -f TriggerRates.xml" >> $OUTPUT_PATH/finalize.sh
echo "cd TriggerRates" >> $OUTPUT_PATH/finalize.sh
echo "rm -f TriggerRates.root" >> $OUTPUT_PATH/finalize.sh
#echo "rm -fr temp_combine" >> $OUTPUT_PATH/finalize.sh
echo "./split_combine.sh" >> $OUTPUT_PATH/finalize.sh
#echo "cp TriggerRates.root ../" >> $OUTPUT_PATH/finalize.sh
#echo "rm -f TriggerRates.root" >> $OUTPUT_PATH/finalize.sh
echo "cd .." >> $OUTPUT_PATH/finalize.sh
echo "#root -b -q -l ROOT_to_XML.C" >> $OUTPUT_PATH/finalize.sh
echo "#root -b -q -l ROOT_to_HTML.C" >> $OUTPUT_PATH/finalize.sh

chmod a+x $OUTPUT_PATH/finalize.sh


rm -f temp_data_$PREFIX
