#!/bin/sh

# art-description: BSOverlayFilter_tf legacy test
# art-type: grid
# art-include: master/Athena

# art-output: trigs_tar.txt
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*

#creating empty PoolFileCatalog.xml required by ART
art.py createpoolfile

BSOverlayFilter_tf.py \
--jobNumber 23 \
--inputBSCONFIGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/test23.tar \
--inputFileValidation False \
--outputBS_TRIGSKIMFile trigskim.RAW \
--triggerBit 240 \
--inputFilterFile trigs_tar.txt
echo "art-result: $? BSFilter"

rm -f ./*.RAW
echo "art-result: $? rmRAW"


grep 'Events processed: 625' log.BSTrigFilter
echo "art-result: $? proc625"

grep 'number of events written: 400' log.BSTrigFilter
echo "art-result: $? wrote400"

grep 'leaving with code 0' log.BSTrigFilter
echo "art-result: $? code0"

grep 'Events processed: 625' log.BSFilter
echo "art-result: $? proc625_2"

grep 'leaving with code 0' log.BSFilter
echo "art-result: $? code0_2"

