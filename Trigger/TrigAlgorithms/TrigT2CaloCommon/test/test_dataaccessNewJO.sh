#!/bin/sh

rm -rf test.pkl
python -m TrigT2CaloCommon.TrigCaloDataAccessConfig
STA=$?
if [ ${STA} -ne 0 ]
then
    echo "Failed configuring job, status", ${STA} 
    exit -1
fi

echo "configuration stored in the pickle"
confTool.py --printConf test.pkl
echo "Executing the pickle file " 
CARunner.py test.pkl 10

