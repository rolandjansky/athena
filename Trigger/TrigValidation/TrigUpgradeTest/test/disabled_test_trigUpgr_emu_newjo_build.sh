#!/bin/bash
# art-description: athenaMT trigger test running the new-style job options EmuNewJOTest.py
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export JOBOPTION="EmuNewJOTest.pkl"
export FROMPICKLE=1
export REGTESTEXP="TrigSignatureMo.*INFO HLT_.*"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/emu_newjo.ref`

get_files -remove -jo TrigUpgradeTest/EmuNewJOTest.py

echo $(date "+%FT%H:%M %Z")"     Generate pickle file ${JOBOPTION}"
python EmuNewJOTest.py &> athena.pickle.log # generate pickle
status=$?

if [ ${status} -ne 0 ]; then
    echo "ERROR in configuration generation stage, stopping"
    echo "art-result: 1 Configuration"
    export ATH_RETURN=1
    export JOB_LOG="athena.pickle.log"
else
    echo
    echo "JOs reading stage finished, launching Athena from pickle file"
    echo "art-result: 0 Configuration"
    echo 
    source exec_TrigUpgradeTest_art_athenaMT.sh

    # Merge log files for post-processing
    JOB_LOG_MERGED="athena.merged.log"
    echo "### athena.pickle.log ###" > ${JOB_LOG_MERGED}
    cat athena.pickle.log >> ${JOB_LOG_MERGED}
    echo "### ${JOB_LOG} ###"
    cat ${JOB_LOG} >> ${JOB_LOG_MERGED}
    export JOB_LOG=${JOB_LOG_MERGED}
fi

source exec_TrigUpgradeTest_art_post.sh
