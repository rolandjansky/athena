#!/usr/bin/env bash

## Prepend script name to all our printouts
shopt -s expand_aliases
alias echo='echo [`basename $0`] '

if [ $# -ne 2 ]; then
   echo "Syntax: `basename $0` <test name>  <job options>"
   exit 1
fi

test_name=$1
part_name=${test_name}_${AtlasProject}_${AtlasVersion}
jobOptions=$2

## Cleanup from previous run
rm -rf /tmp/${part_name}

## Add current directory if no absolute path given
if [ "${jobOptions:0:1}" != "/" ]; then
    jobOptions=${PWD}/${jobOptions}
fi

echo "$0: Partition=$part_name, jobOptions=$jobOptions"

## Additional setup to run partitions
export PYTHONPATH=.:$PYTHONPATH
export TDAQ_DB_PATH=.:$TDAQ_DB_PATH
export TDAQ_JAVA_HOME=/afs/cern.ch/sw/lcg/external/Java/JDK/1.6.0/ia32

## Setup private IPC domain (but different for release branches)
ipcref=${PWD}/ipc_${NICOS_NIGHTLY_NAME}_root.ref
touch ${ipcref}
export TDAQ_IPC_INIT_REF=file:${ipcref}

## Prepare file links, etc.
HLTTesting_links.sh
get_joboptions part_lhfarm.py
get_joboptions part_run_opt.py
get_joboptions ${test_name}_opt.py
get_joboptions fixedRunNumber.py
get_joboptions addPatchRepository.py

##getting the data11_7TeV file with 100 events in it
data_file="data12_8TeV.00200863.physics_eb_zee_zmumu._0001.data"
echo "Copying data file from eos..."
xrdcp root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/${data_file} .

status=$?
if [ ${status} != 0 ]; then
    echo "[${data_file}] ERROR: Could not copy data file"
    exit ${status};
fi

ls ${PWD}
ls ${PWD}/${data_file}
status=$?
if [ ${status} != 0 ]; then
    echo "[${PWD}/${data_file}] data file does not exist"
    exit ${status};
fi

## Create rob list
echo "Creating ROS configuration..."
rosconf-from-data.py --ignore '^0x007[35bc]' --py  ${PWD}/${data_file}  > robhit.py
echo "\n "

## Get run number from input file
echo "Getting run number from file..."
part_get_runnumber.py   --L2Input  ${PWD}/${data_file} > ${PWD}/runnumber.txt
## Create the partition
echo "Creating partition..."
hltOpt="{'jobOptionsPath':'${jobOptions}','logLevel':['INFO'],'preCommand':['testPhysicsV4=True','HLTOutputLevel=DEBUG','doValidation=True','rerunLVL1=True']}"
hltpm_part_l2ef.py --option-file=${test_name}"_opt.py" --partition-name=${part_name} --l2-hlt=${hltOpt} --ef-hlt=${hltOpt} -Z fixedRunNumber -Z addPatchRepository
status=$?
if [ ${status} != 0 ]; then
    echo "[${part_name}] ERROR: Could not create partition"
    exit ${status};
fi

## Some additional fixups
part_lhl2ef_fix.py ${part_name}.data.xml
status=$?
if [ ${status} != 0 ]; then
    echo "[${part_name}] ERROR: Could not fixup partition"
    exit ${status};
fi

## Run the partition
echo "Running partition..."
which run-test.py

run-test.py --option-file=part_run_opt.py ${part_name}.data.xml
status=$?
if [ ${status} != 0 ]; then
  cleanup.py --remove-produced-data ${part_name}.data.xml
  echo "[${part_name}] ERROR: Could not run partition"
fi

## Run post-test analysis
part_checkTest.sh ${part_name}

ls ${PWD}/${data_file}
sts=$?
if [ ${sts} == 0 ]; then
    echo "Deleting data file : [${PWD}/${data_file}]"
    rm ${PWD}/${data_file}
fi

## The runner used to do the log file removal but this got temporarily disabled (#89585)
rm -rf /tmp/${part_name}
rm -f /tmp/*${part_name}*.tar.gz

## Final cleanup
pmg_kill_partition -p ${part_name}
pmg_kill_partition -p initial
rm -rf /tmp/${TDAQ_VERSION}/initial

exit ${status};
