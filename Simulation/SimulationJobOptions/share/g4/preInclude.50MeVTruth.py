#########################################################
#
# SimulationJobOptions/postOptions.50MeVTruth.py
# Zach Marshall
#
# 50 MeV truth cuts
#
#########################################################

def reset_truthstrategies_50():
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

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitMCTruth", reset_truthstrategies_50)
