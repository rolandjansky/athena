# Input file
# inputFile = "/data/atlas/19.X.Y/19.1.4.7_AtlasDerivation/mc14_13TeV/DAOD_TOPQ1.04987437._000035.pool.root.1"
#inputFile = "/home/dzhang/links/SAMPLES/R20/MC15/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s2576_s2132_r6630_tid05348608_00/AOD.05348608._004042.pool.root.1"
inputFile = "/afs/cern.ch/user/m/morrisj/scratch0/rtt/DAOD_TOPQ1.MC15.pool.root"

# n events ( use -1 to process all events )
nEvents = 20

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections= [ inputFile ]
algseq = CfgMgr.AthSequencer("AthAlgSeq")

## The Tool
isoTool_2 = CfgMgr.CP__IsolationSelectionTool( "IsolationSelectionTool" )
isoTool_2.CalibFileName = "IsolationSelection/v1/MC15_Z_Jpsi_cutMap.root"
isoTool_2.MuonWP = "Gradient"
isoTool_2.ElectronWP = "Gradient"
isoTool_2.PhotonWP = "Gradient"
svcMgr.ToolSvc += isoTool_2

## Test algorithm
from IsolationSelection.IsolationSelectionConf import CP__TestIsolationAthenaAlg 
algseq += CfgMgr.CP__TestIsolationAthenaAlg( IsoTool = isoTool_2 ) 

theApp.EvtMax = nEvents
