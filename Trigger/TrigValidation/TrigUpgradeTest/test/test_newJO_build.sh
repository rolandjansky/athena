#!/bin/sh
# art-description: athenaMT trigger test running the new-style job options from TrigUpgradeTest
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.root
# art-output: *.regtest.new

export NAME="TrigUpgradeTest_newJO_build"
export SKIPEVENTS=30
export EVENTS=20
export THREADS=1
export SLOTS=1
export JOBOPTION="newJOtest.pkl"
export FROMPICKLE=1
export REGTESTEXP="REGTEST|.*ERROR (?\!attempt to add a duplicate).*|.*FATAL.*|.*newJOtest.pkl.*|TrigSignatureMoniMT .*INFO.*"

rm -rf newJOtest.py bootstrap.pkl bootstrap.py

# this is a hack to pre-confgure scheduler and other MT services, 
#will be taken away once NEW system has better means to influence the bootstrap content
cat <<EOF >> bootstrap.py
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
svcMgr.AvalancheSchedulerSvc.ShowControlFlow=True
svcMgr.AvalancheSchedulerSvc.ShowDataDependencies=True
EOF

echo $(date "+%FT%H:%M %Z")"     Execute Athena config-only bootstrap for test ${NAME}"
athena --threads=${THREADS} --concurrent-events=${SLOTS} --config-only=bootstrap.pkl bootstrap.py &> athena.bootstrap.log

get_files -jo TrigUpgradeTest/newJOtest.py

echo $(date "+%FT%H:%M %Z")"     Generate pickle file for test ${NAME}"
python newJOtest.py &> athena.pickle.log # generate pickle
status=$?

if [ ${status} -ne 0 ]; then
    echo "ERROR in configuration generation stage, stopping"
    echo "art-result 1 Configuration"
    exit 1
else
    echo
    echo "JOs reading stage finished, launching Athena from pickle file"
    echo "art-result 0 Configuration"
    echo 
    source exec_TrigUpgradeTest_art_athenaMT.sh
    source exec_TrigUpgradeTest_art_post.sh
fi
