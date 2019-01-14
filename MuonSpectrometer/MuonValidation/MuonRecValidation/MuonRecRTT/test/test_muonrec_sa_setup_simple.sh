#!/bin/sh
#
# art-description: Test a standalone muon setup which is not dependent on any other pieces of the detector, with the --threads=1 option. 
#
# art-type: grid
# art-include: master/Athena
# art-input: user.zhidong.SingleMuon_5GeV_task12089629.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
# art-input-nfiles: 1


art.py createpoolfile

set -x

athena -c "InputRdoFile='user.zhidong.12100112.EXT0._000001.RDO.pool.root'" --threads=1 MuonRecExample/MuonRec_myTopOptions.py
