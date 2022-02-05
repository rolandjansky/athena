#!/bin/bash 


# run the test
RecoSteeringTest.py --RAW 2>&1 > reco.log
export testStatus=$?
if [[ $testStatus -ne 0 ]]
    then
    echo "ERROR RecoSteering jobs faild, see: reco.log"
    exit -1
fi

DIGEST_FILE=new_jo_raw_reco_aod_digest

xAODDigest.py myAOD.pool.root ${DIGEST_FILE}.new 2>&1 > xaoddigest.log
digestStatus=$?
if [[ $digestStatus -ne 0 ]]
    then
    echo "ERROR xAODDigets execution failed, see: xaoddigest.log"
    exit -1
fi

#TODO replace with running rec-ex-common job and produce difference between the two, 
#     this is still marathon distance from now

rm -rf  ${DIGEST_FILE}.ref
get_files -data  ${DIGEST_FILE}.ref
diff ${DIGEST_FILE}.ref ${DIGEST_FILE}.new
diffStatus=$?
if [[ $diffStatus -ne 0 ]]
    then
    echo "ERROR Difference w.r.t. the reference, if expected update ${DIGEST_FILE}.ref"
    exit -1
fi
echo "All OK"


