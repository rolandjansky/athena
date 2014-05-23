### switchMon.py
###
### written by Marcello Barisonzi
### marcello.barisonzi(at)cern.ch
###
### Usage: switch off all other monitoring
### when testing with a transform
###
### Example:
###
### Reco_trf.py inputESDFile=/tmp/mbarison/data10_7TeV.00166142.physics_JetTauEtmiss.merge.ESD.r1774_p327_tid203218_00/ESD.203218._000059.pool.root.1,/tmp/mbarison/data10_7TeV.00166142.physics_JetTauEtmiss.merge.ESD.r1774_p327_tid203218_00/ESD.203218._000542.pool.root.1 autoConfiguration=everything outputHISTFile=myMergedMonitoring.root preExec="execfile(os.path.expanduser('~/switchMon.py'))"
###
###
###
###

from AthenaMonitoring.DQMonFlags import DQMonFlags

DQMonFlags.doStreamAwareMon = False

DQMonFlags.doCTPMon = False 
DQMonFlags.doCaloMon = False 
DQMonFlags.doEgammaMon = False 
DQMonFlags.doGlobalMon = True 
DQMonFlags.doHLTMon = False
DQMonFlags.doInDetAlignMon = False 
DQMonFlags.doInDetGlobalMon = False 
DQMonFlags.doInDetPerfMon = False 
DQMonFlags.doJetMon = False 
DQMonFlags.doJetTagMon = True
DQMonFlags.doLArMon = False 
DQMonFlags.doLVL1CaloMon = False 
DQMonFlags.doLucidMon = False 
DQMonFlags.doMissingEtMon = False 
DQMonFlags.doMuonAlignMon = False 
DQMonFlags.doMuonCombinedMon = False 
DQMonFlags.doMuonPhysicsMon = False 
DQMonFlags.doMuonRawMon = False 
DQMonFlags.doMuonSegmentMon = False 
DQMonFlags.doMuonTrackMon = False 
DQMonFlags.doMuonTrkPhysMon = False
DQMonFlags.doPixelMon = False 
DQMonFlags.doSCTMon = False 
DQMonFlags.doTRTElectronMon = False 
DQMonFlags.doTRTMon = False 
DQMonFlags.doTauMon = False 
DQMonFlags.doTileMon = False 
