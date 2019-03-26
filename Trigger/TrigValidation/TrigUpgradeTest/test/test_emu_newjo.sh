#!/bin/sh
# art-type: build
# art-include: master/Athena

rm -rf EmuNewJOTest.py bootstrap.pkl bootstrap.py

# this is a hack to pre-confgure scheduler and other MT services,
#will be taken away once NEW system has better means to influence the bootstrap content
cat <<EOF >> bootstrap.py
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
svcMgr.AvalancheSchedulerSvc.ShowControlFlow=True
svcMgr.AvalancheSchedulerSvc.ShowDataDependencies=True
EOF

athena --threads=1 --config-only=bootstrap.pkl bootstrap.py


get_files -jo TrigUpgradeTest/EmuNewJOTest.py
python EmuNewJOTest.py # generate pickle
status=$?
if [ ${status} -ne 0 ]
then
    echo "ERROR in configuration generation stage, stopping"
    exit -1
else
    echo
    echo "JOs reading stage finished, launching Athena from pickle file"
    echo
    athena EmuNewJOTest.pkl
fi
