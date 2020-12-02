#################################################################################
# Run arguments file auto-generated on Thu Aug 20 12:41:38 2009 by:
# JobTransform: ESDtoAOD
# Version: RecJobTransforms-00-08-33
# Authors: David Cote <david.cote@cern.ch>
#################################################################################

from PyJobTransformsCore.runargs import RunArguments

runArgs = RunArguments()

runArgs.conditionsTag = 'COMCOND-ES1C-001-01'

#runArgs.beamType = 'cosmics'

runArgs.outputDQMonitorFile = 'MonitorESD.root'

#runArgs.trigStream = 'IDCosmic'

runArgs.maxEvents = -1

runArgs.inputESDFile = ['myESD.pool.root']

runArgs.preInclude = ['RecExCommon/RecoUsefulFlags.py', 'RecExCond/MinimalCommissioningSetup.py', 'RecJobTransforms/UseOracle.py', 'RecJobTransforms/debugConfig.py']

runArgs.outputAODFile = 'myAOD.pool.root'

runArgs.postExec = ['svcMgr.MessageSvc.OutputLevel=INFO']

runArgs.preExec = ['rec.doTrigger=False;rec.doMuon=False',
                   'rec.doLArg=False',
                   'rec.doTile=False',
                   'rec.doInDet=False',
                   'rec.doCalo=False',
                   'rec.doPerfMon=True',
                   'rec.doDetailedPerfMon=True',
                   'DQMonFlags.doPixelMon=False',
                   'DQMonFlags.doSCTMon=False',
                   'DQMonFlags.doTRTMon=False',
                   'DQMonFlags.doTRTElectronMon=False',
                   'DQMonFlags.doInDetGlobalMon=False',
                   'DQMonFlags.doInDetAlignMon=False',
                   'DQMonFlags.doInDetPerfMon=False',
                   'DQMonFlags.doLArMon=False',
                   'DQMonFlags.doCaloMon=False',
                   'DQMonFlags.doTileMon=False',
                   'DQMonFlags.doEgammaMon=False',
                   'DQMonFlags.doMissingEtMon=False',
                   'DQMonFlags.doJetMon=False',
                   'DQMonFlags.doTauMon=False',
                   'DQMonFlags.doJetTagMon=False',
                   'DQMonFlags.doMuonRawMon=False',
                   'DQMonFlags.doMuonSegmentMon=False',
                   'DQMonFlags.doMuonTrackMon=False',
                   'DQMonFlags.doMuonAlignMon=False',
                   'DQMonFlags.doMuonPhysicsMon=False',
                   'DQMonFlags.doMuonCombinedMon=False',
                   'DQMonFlags.doLucidMon=True']

runArgs.geometryVersion = 'ATLAS-GEONF-08-00-00' 

