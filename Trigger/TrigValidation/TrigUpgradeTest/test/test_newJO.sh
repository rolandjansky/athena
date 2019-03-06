#!/bin/sh
# art-type: build
# art-include: master/Athena

get_files -jo TrigUpgradeTest/newJOtest.py
python newJOtest.py # generate pickle
status=$?
if [ ${status} -ne 0 ] 
then
    echo "ERROR in configuration generation stage, stopping"
    exit -1
else
    echo
    echo "JOs reading stage finished, launching Athena from pickle file"
    echo 
    athena --skipEvents=30 --evtMax=20 newJOtest.pkl
fi
