# Input file
inputFile = "/data/atlas/19.X.Y/DC14/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.merge.AOD.e1727_s1933_s1911_r5591_r5625_tid01507241_00/AOD.01507241._010089.pool.root.1"

# n events ( use -1 to process all events )
nEvents = 20

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections= [ inputFile ]
algseq = CfgMgr.AthSequencer("AthAlgSeq")

## The Tool
isoTool = CfgMgr.CP__ElectronIsolationSelectionTool(" ElectronIsolationSelectionTool" )
svcMgr.ToolSvc += isoTool

## Test algorithm
from ElectronIsolationSelection.ElectronIsolationSelectionConf import CP__TestElectronIsoAthena 
algseq += CfgMgr.CP__TestElectronIsoAthena( IsoTool = isoTool ) 

theApp.EvtMax = nEvents
