# $Id: CPAnalysisExamples_jobOptions.py 300492 2014-04-30 14:39:05Z krasznaa $

# Set up the file reading:
FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/19.0.X_rel_4/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.19.0.X_rel_4.pool.root"
FNAME = "/afs/cern.ch/user/i/ivivarel/work/DATA/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591_tid01494883_00/AOD.01494883._116110.pool.root.1"
#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/19.0.X_rel_4/mc12_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.AOD.19.0.X_rel_4.pool.root"
#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/19.0.X_rel_4/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.19.0.X_rel_4.pool.root"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from CPAnalysisExamples.CPAnalysisExamplesConf import CP__ToolTester
alg = CP__ToolTester()
alg.MuonSelectionToolExample.MaxEta = 2.4
alg.MuonSelectionToolExample.Author = 12
alg.MuonEfficiencyToolExample.DummyProperty = 5.5
alg.OutputLevel = DEBUG
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
