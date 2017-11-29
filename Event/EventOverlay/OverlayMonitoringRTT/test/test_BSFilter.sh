#!/bin/sh

# art-description: BSOverlayFilter_tf legacy test
# art-type: grid
# art-ci: 21.0, master

# art-output: trigs_tar.txt
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*

BSOverlayFilter_tf.py --jobNumber 23 --inputBSCONFIGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/test23.tar --inputFileValidation False --outputBS_TRIGSKIMFile trigskim.RAW --triggerBit 240 --inputFilterFile trigs_tar.txt
JOBSTATUS_1=$?

echo "art-result: [${JOBSTATUS_1}]"

rm -f *.RAW

grep 'Events processed: 625' log.BSFilter
grep 'number of events written: 400' log.BSFilter
grep 'leaving with code 0' log.BSFilter
