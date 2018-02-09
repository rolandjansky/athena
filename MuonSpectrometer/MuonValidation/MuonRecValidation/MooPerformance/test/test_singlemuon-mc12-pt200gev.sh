#!/bin/sh
#
# art-description: Art test job for MooPerformance package, migrated from RTT job: SingleMuons-mc12-Pt200GeV
# 
# art-type: grid
# art-input: user.zhidong.SingleMuon_200GeV_task12089651.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
# art-input-nfiles: 10
# art-output: trk*.txt

fileList="['${ArtInFile//,/', '}¡¯]"
set -e

athena.py -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.EvtMax=20000; athenaCommonFlags.FilesInput = $fileList" -b MooPerformance/MooPerformance_topOptions.py
