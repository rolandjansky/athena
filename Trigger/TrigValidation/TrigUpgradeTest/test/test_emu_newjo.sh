#!/bin/sh
# art-type: build
# art-include: master/Athena

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
