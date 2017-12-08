# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Run arguments file auto-generated on Tue Mar 10 17:11:50 2015 by:
# JobTransform: ESDtoAOD
# Version: $Id: trfExe.py 636429 2014-12-17 09:48:38Z graemes $
# Import runArgs class
#inputfile = '/afs/cern.ch/user/b/btong/work/testdata/data12_8TeV.00204240.express_express.recon.ESD.r6461_tid04972921_00/ESD.04972921._2.pool.root'
from PyJobTransforms.trfJobOptions import RunArguments
runArgs = RunArguments()
runArgs.trfSubstepName = 'ESDtoAOD' 

#runArgs.conditionsTag = 'COMCOND-BLKPA-RUN1-06' old tag
runArgs.preExec = ['rec.doTrigger.set_Value_and_Lock(True);\
rec.doJetMissingETTag.set_Value_and_Lock(False);\
from CaloRec.CaloCellFlags import jobproperties;\
jobproperties.CaloCellFlags.doLArHVCorr=False;\
jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);\
from InDetRecExample.InDetJobProperties import InDetFlags;\
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);\
InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);\
DQMonFlags.doStreamAwareMon=False;\
DQMonFlags.enableLumiAccess.set_Value_and_Lock(True);\
from JetRec.JetRecFlags import jetFlags;\
jetFlags.useTracks=False;\
from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags;\
MuonDQADetFlags.doMDTTGCL1Mon.set_Value_and_Lock(False);\
MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(False);\
MuonDQADetFlags.doTGCMon.set_Value_and_Lock(False);\
DQMonFlags.doMuonRawMon=False;\
DQMonFlags.doLVL1CaloMon=False;\
DQMonFlags.doCTPMon=False;\
jobproperties.CaloCellFlags.doLArDeadOTXCorr=False;\
DQMonFlags.doPixelMon = False;\
DQMonFlags.doSCTMon = False;\
DQMonFlags.doStreamAwareMon = False;\
DQMonFlags.doTRTElectronMon = False;\
DQMonFlags.doTRTMon = False;\
DQMonFlags.doTauMon = False;\
DQMonFlags.doTileMon = False;\
DQMonFlags.doCTPMon = False;\
DQMonFlags.doCaloMon = False;\
DQMonFlags.doEgammaMon = False;\
DQMonFlags.doGlobalMon = False;\
DQMonFlags.doHLTMon = False;\
DQMonFlags.doInDetAlignMon = False;\
DQMonFlags.doInDetGlobalMon = False;\
DQMonFlags.doInDetPerfMon = False;\
DQMonFlags.doJetMon = False;\
DQMonFlags.doJetTagMon = False;\
DQMonFlags.doLArMon = False;\
DQMonFlags.doLVL1CaloMon = False;\
DQMonFlags.doMuonTrackMon=True;\
DQMonFlags.doMuonTrkPhysMon=False;\
DQMonFlags.useTrigger.set_Value_and_Lock(True);']

runArgs.autoConfiguration = ['everything']
#runArgs.AMITag = 'x331'
#runArgs.athenaopts = ['--preloadlib=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.0/sw/IntelSoftware/linux/x86_64/xe2013/composer_xe_2013.3.163/compiler/lib/intel64/libintlc.so.5:/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.0/sw/IntelSoftware/linux/x86_64/xe2013/composer_xe_2013.3.163/compiler/lib/intel64/libimf.so']
runArgs.beamType = 'collisions'
#runArgs.geometryVersion = 'ATLAS-R1-2012-02-00-00'

# Explicitly added to process all events in this step
runArgs.maxEvents = 100 #-1

# Input data
#runArgs.inputESDFile = ['myESD.pool.root']
runArgs.inputESDFile = ['root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00267638/data15_13TeV.00267638.express_express.recon.ESD.x342/data15_13TeV.00267638.express_express.recon.ESD.x342._lb0523._SFO-3._0001.1']
#runArgs.inputESDFile = ['root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00266904/data15_13TeV.00266904.physics_Main.recon.DESD_ZMUMU.f594_m1440_f594/data15_13TeV.00266904.physics_Main.recon.DESD_ZMUMU.f594_m1440_f594._0003.1']
#runArgs.inputESDFile = ['root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00266904/data15_13TeV.00266904.physics_Main.recon.ESD.f594/data15_13TeV.00266904.physics_Main.recon.ESD.f594._lb0522._SFO-3._0001.1']
#runArgs.inputESDFile = ['root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00266904/data15_13TeV.00266904.express_express.recon.ESD.x339/data15_13TeV.00266904.express_express.recon.ESD.x339._lb0605._SFO-4._0001.1']
#runArgs.inputESDFile = ['root://eosatlas.cern.ch//eos/atlas/user/b/btong/data/216432_muons/ESD.01566165._000054.pool.root.1']
#runArgs.inputESDFile = ['root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00266919/data15_13TeV.00266919.physics_Main.merge.DRAW_ZMUMU.f594_m1440/data15_13TeV.00266919.physics_Main.merge.DRAW_ZMUMU.f594_m1440._0002.1']
runArgs.inputESDFileType = 'ESD'
#runArgs.inputESDFileNentries = 131L


# Output data
#runArgs.outputAODFile = 'myAOD.pool.root'
#runArgs.outputAODFileType = 'AOD'
runArgs.outputHIST_AOD_INTFile = 'Tony_hist.root'
runArgs.outputHIST_AOD_INTFileType = 'hist_aod_int'

# Extra runargs

# Extra runtime runargs

# Literal runargs snippets
