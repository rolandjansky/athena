#!/bin/sh

python MuonRpcDataDecodeConfig.py # generate pickle
status=$?
if [ ${status} -ne 0 ] 
then
    echo "ERROR in configuration generation stage, stopping"
    exit -1
else
    echo
    echo "JOs reading stage finished, launching Athena from pickle file"
    echo 
    athena --threads=1 --evtMax=20 MuonRpcDataDecode.pkl
fi
