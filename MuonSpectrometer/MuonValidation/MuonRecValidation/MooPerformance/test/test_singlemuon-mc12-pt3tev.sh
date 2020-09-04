#!/bin/sh
#
# art-description: Art test job for MooPerformance package, migrated from RTT job: SingleMuons-mc12-Pt3TeV
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-memory: 4096
# art-input: user.zhidong.SingleMuon_3TeV_task12089649.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
# art-input-nfiles: 3
# art-output: trk*.txt

set -e
fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

athena.py -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.EvtMax=6000; athenaCommonFlags.FilesInput = $fileList" -b MooPerformance/MooPerformance_topOptions.py
