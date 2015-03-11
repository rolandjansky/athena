#########################################################
#
# SimulationJobOptions/postInclude.50MeVTruth.py
# Zach Marshall
#
# 50 MeV truth cuts (for MC15 ISF only)
#
#########################################################

from AthenaCommon.SystemOfUnits import MeV
from AthenaCommon.AppMgr import ToolSvc
ToolSvc.ISF_MCTruthStrategyGroupID_MC15.PrimaryMinPt = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupID_MC15.SecondaryMinPt = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupIDHadInt_MC15.PrimaryMinPt = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupIDHadInt_MC15.SecondaryMinPt = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupCaloMuBrem.PrimaryMinEkin = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupCaloMuBrem.SecondaryMinEkin = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupCaloDecay_MC15.PrimaryMinEkin = 50.*MeV
ToolSvc.ISF_MCTruthStrategyGroupCaloDecay_MC15.SecondaryMinEkin = 50.*MeV

