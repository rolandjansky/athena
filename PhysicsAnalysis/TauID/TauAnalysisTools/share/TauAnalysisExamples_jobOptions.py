# Set up the file reading:
# FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/19.0.X_rel_4/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.19.0.X_rel_4.pool.root"
FNAME = "/ZIH.fast/users/duschi/xAOD/valid2.147818.Pythia8_AU2CTEQ6L1_Ztautau.digit.AOD.e2657_s1933_s1964_r5539_tid01483611_00/AOD.01483611._000064.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

#Add the test algorithm:
from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauAnalysisToolsExampleAthena
# TauSelTool = TauAnalysisTools__TauSelectionTool()
TauSelTool = TauAnalysisTools__TauSelectionTool()
TauSelTool.PtMin = 20
TauSelTool.SelectionCuts = 1
TauSelTool.OutputLevel = DEBUG
theJob += TauSelTool

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

