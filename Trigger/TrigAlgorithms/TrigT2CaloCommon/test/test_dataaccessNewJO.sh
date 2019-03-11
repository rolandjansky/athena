#!/bin/sh
# art-type: build
# art-ci: master
rm -rf test.pkl
python -m TrigT2CaloCommon.TrigCaloDataAccessConfig
STA=$?
if [ ${STA} -ne 0 ]
then
    echo "Failed configuring job, status", ${STA} 
    exit -1
fi

echo "configuration stored in the pickle"
confTool.py --print test.pkl
echo "running athena " 
athena test.pkl