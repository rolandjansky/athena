
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/project/PAT/xAODs/r5534/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5534/AOD.01482225._000107.pool.root.1" ]

algseq = CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.CP__MuonSelectionAlg(Input="Muons",Output="SelectedMuons",OutputLevel=DEBUG)


theApp.EvtMax=10
