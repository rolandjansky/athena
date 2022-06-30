#!/bin/bash

#### Simple script to test the reproducibility of the database read-in of the NSWCondAlg
python -m MuonCondTest.NswPassivationAlgTest -i /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/WorkflowReferences/22.0/q445/v6/myESD.pool.root
status_code=$?
if [ ${status_code} -ne 0 ]; then
    exit 1
fi
mv passivationDump.txt refrence.txt
python -m MuonCondTest.NswPassivationAlgTest -i /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/WorkflowReferences/22.0/q445/v6/myESD.pool.root
status_code=$?
if [ ${status_code} -ne 0 ]; then
    exit 1
fi

diff passivationDump.txt refrence.txt
status_code=$?
if [ ${status_code} -ne 0 ]; then
    exit 1
fi


