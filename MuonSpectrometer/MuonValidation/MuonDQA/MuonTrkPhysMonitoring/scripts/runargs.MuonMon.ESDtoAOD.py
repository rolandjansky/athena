# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DetGeo.set_Value_and_Lock('atlas')

from RecExConfig.RecFlags import rec
rec.readESD.set_Value_and_Lock(True)
rec.doMonitoring.set_Value_and_Lock(True)

rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False) 

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doGlobalMon.set_Value_and_Lock(False)
DQMonFlags.doLucidMon.set_Value_and_Lock(False)
DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
DQMonFlags.doCTPMon.set_Value_and_Lock(False)
DQMonFlags.doHLTMon.set_Value_and_Lock(False)
DQMonFlags.doPixelMon.set_Value_and_Lock(False)
DQMonFlags.doSCTMon.set_Value_and_Lock(False)
DQMonFlags.doTRTMon.set_Value_and_Lock(False)
DQMonFlags.doTRTElectronMon.set_Value_and_Lock(False)
DQMonFlags.doInDetGlobalMon.set_Value_and_Lock(False)
DQMonFlags.doInDetAlignMon.set_Value_and_Lock(False)
DQMonFlags.doInDetPerfMon.set_Value_and_Lock(False)
DQMonFlags.doLArMon.set_Value_and_Lock(False)
DQMonFlags.doCaloMon.set_Value_and_Lock(False)
DQMonFlags.doTileMon.set_Value_and_Lock(False)
DQMonFlags.doEgammaMon.set_Value_and_Lock(False)
DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
DQMonFlags.doJetMon.set_Value_and_Lock(False)
DQMonFlags.doTauMon.set_Value_and_Lock(False)
DQMonFlags.doJetTagMon.set_Value_and_Lock(False)
DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)

DQMonFlags.doMuonRawMon.set_Value_and_Lock(False)
DQMonFlags.doMuonSegmentMon.set_Value_and_Lock(False)
DQMonFlags.doMuonTrackMon.set_Value_and_Lock(False)
DQMonFlags.doMuonAlignMon.set_Value_and_Lock(False)
DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)

DQMonFlags.doMuonTrkPhysMon.set_Value_and_Lock(True)


from PyJobTransformsCore.runargs import RunArguments

runArgs = RunArguments()

# Maximum number of events to process
runArgs.maxEvents = 500
 
# Input file that contains ESD's
runArgs.inputESDFile = ['/mnt/data/nectar/NewMonitoringCode/2012Data/data12_8TeV.00201289.physics_Muons.recon.ESD.f436/data12_8TeV.00201289.physics_Muons.recon.ESD.f436._lb0445._SFO-5._0001.1']
  
# Basic string
#runArgs.AMITag = 'f414'
 
# Basic string for data
runArgs.beamType = 'collisions'

# Output file that contains AOD's
# runArgs.outputAODFile = '/tmp/nectar/AOD.pool.root'

####runArgs.preInclude = ['RecExCommon/RecoUsefulFlags.py', 'RecExCommission/MinimalCommissioningSetup.py', 'RecJobTransforms/UseOracle.py', 'RecJobTransforms/debugConfig.py']
###runArgs.outputAODFile = 'myAOD.pool.root'
runArgs.postExec = ['svcMgr.MessageSvc.OutputLevel=INFO']

# List of strings separated by commas
runArgs.autoConfiguration = ['everything']
runArgs.outputDQMonitorFile = 'MuonMonitorESD.root'
