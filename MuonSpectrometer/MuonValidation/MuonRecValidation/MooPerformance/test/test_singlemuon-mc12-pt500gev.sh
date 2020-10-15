#!/bin/sh
#
# art-description: Art test job for MooPerformance package, migrated from RTT job: SingleMuons-mc12-Pt500GeV
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-memory: 4096
# art-input: user.zhidong.SingleMuon_500GeV_task12089642.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
# art-input-nfiles: 3
# art-output: trk*.txt

set -e
fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

athena.py -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.EvtMax=6000; athenaCommonFlags.FilesInput = $fileList" -b MooPerformance/MooPerformance_topOptions.py
