#########################################################
#
# SimulationJobOptions/postOptions.50MeVTruth.py
# Zach Marshall
#
# 50 MeV truth cuts
#
#########################################################

def reset_truthstrategies_50():
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.ISFRun:
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("BremsPrimaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("BremsSecondaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("ComptonPrimaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("ComptonSecondaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("ConversionPrimaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("ConversionSecondaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("DecayPrimaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("DecaySecondaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("IonizationPrimaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("IonizationSecondaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("HadronicPrimaryMinEnergy", 50)
        AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("HadronicSecondaryMinEnergy", 50)
    else:
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.SystemOfUnits import MeV
        ToolSvc.ISF_MCTruthStrategyGroupID_MC15.PrimaryMinPt = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupID_MC15.SecondaryMinPt = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupIDHadInt_MC15.PrimaryMinPt = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupIDHadInt_MC15.SecondaryMinPt = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupCaloMuBrem.PrimaryMinEkin = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupCaloMuBrem.SecondaryMinEkin = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupCaloDecay_MC15.PrimaryMinEkin = 50.*MeV
        ToolSvc.ISF_MCTruthStrategyGroupCaloDecay_MC15.SecondaryMinEkin = 50.*MeV

the_stage = 'preInitMCTruth'
if simFlags.ISFRun: the_stage = 'preInit'

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function(the_stage, reset_truthstrategies_50)
