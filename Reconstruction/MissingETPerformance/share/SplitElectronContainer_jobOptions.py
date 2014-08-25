include.block( "MissingETPerformance/MissingETSplitElectronContainer_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     SplitElectronContainer as \
     ConfiguredSplitElectronContainer

SplitElectronContainer = ConfiguredSplitElectronContainer(

     # Flags
     inverseSelection   = False,
     doTruth            = False,
     # Input containers
     ElectronContainer  = "ElectronAODCollection",
     ZeeCollection      = "SelectedElectrons",
     TruthPartContainer = "SpclMC",
     # Output containers
     modElectronContainer = "modElectronCollection",
     remElectronContainer = "remElectronCollection",
     selElectronContainer = "selElectronCollection",
     remElectronTruthContainer = "remEleTruthCollection",
     selElectronTruthContainer = "selEleTruthCollection",
     )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence.ZeeSequence += SplitElectronContainer

print SplitElectronContainer
