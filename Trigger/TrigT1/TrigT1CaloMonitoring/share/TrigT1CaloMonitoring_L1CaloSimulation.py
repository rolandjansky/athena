####################################################################################################################
########################################## L1Calo Simulation #######################################################
####################################################################################################################
# Import the configurable algorithms for TrigT1Calo
#from AthenaCommon.GlobalFlags  import globalflags

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EmTauTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ROD
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMMMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEPCMMMaker

#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Tester
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__DumpTriggerObjects

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the required algorithms to the sequence
job += LVL1__CPMTowerMaker( 'CPMTowerMaker' )
job += LVL1__EmTauTrigger( 'EmTauTrigger' )
job += LVL1__EnergyTrigger( 'EnergyTrigger' )
job += LVL1__JetTrigger( 'JetTrigger' )
job += LVL1__ROD( 'ROD' )
job += LVL1__JEPCMMMaker( 'JEPCMMMaker' )
job += LVL1__CPCMMMaker( 'CPCMMMaker' )


#*************************************************************
# Set input and output locations  
#*************************************************************


# CPMTowerMaker
#input (from BS)
job.CPMTowerMaker.TriggerTowerLocation = "TriggerTowers"
##output
job.CPMTowerMaker.CPMTowerLocation = "Sim_CPMTowers"
##job.CPMTowerMaker.OutputLevel = DEBUG

# EmTauTrigger
#input (from BS)
#job.EmTauTrigger.TriggerTowerLocation = "TriggerTowers"
#output
job.EmTauTrigger.CPMHitsLocation = "Sim_CPMHits"
job.EmTauTrigger.EmTauROILocation = "Sim_EmTauRoIs"
#job.EmTauTrigger.LVL1ConfigSvc = ""
#job.EmTauTrigger.OutputLevel = DEBUG

# JetTrigger
#input (from BS)
#job.JetTrigger.JetElementLocation="JetElements"
#output (from BS)
job.JetTrigger.JEMHitsLocation = "Sim_JEMHits"
job.JetTrigger.JetROIOutputLocation= "Sim_JetRoIs"
##job.JetTrigger.LVL1ConfigSvc = ""
##job.JetTrigger.NumberOfThreshSets = ""
##job.JetTrigger.DefaultClusterThresholds = ""
##job.JetTrigger.DefaultMultiplicities = ""
##job.JetTrigger.DefaultAlgorithms = ""
##job.JetTrigger.JetElementThreshold = ""
##job.JetTrigger.OutputLevel=DEBUG

## EnergyTrigger
##input (from BS)
#job.EnergyTrigger.JetElementLocation="JetElements"
##output
job.EnergyTrigger.JEMEtSumsLocation="Sim_JEMEtSums"
job.EnergyTrigger.EnergyRoILocation = "Sim_EnergyRoIs"
##job.EnergyTrigger.EnergyCTPLocation = ""
##job.EnergyTrigger.LVL1ConfigSvc = ""
##job.EnergyTrigger.EtSumJEThresh = ""
##job.EnergyTrigger.EtMissJEThresh = ""
##job.EnergyTrigger.ExEyRanges = ""
##job.EnergyTrigger.OutputLevel=DEBUG

##ROD
##input (from Simulation - intermediate data)
job.ROD.EmTauRoILocation = "Sim_EmTauRoIs"
job.ROD.JetRoILocation = "Sim_JetRoIs"
job.ROD.EnergyRoILocation = "Sim_EnergyRoIs"
##output
job.ROD.JetEtRoILocation = "Sim_JetEtRoIs"
##job.ROD.LVL1ConfigSvc = ""
##job.ROD.EmTauCTPLocation = "Sim_EmTauCTP"
##job.ROD.EmTauSlinkLocation = "Sim_EmTauSlink"
##job.ROD.JEPSlinkLocation = "Sim_JEPSlink"
##job.ROD.EnergySlinkLocation = "Sim_EnergySlink"
##job.ROD.JetCTPLocation = "Sim_JetCTP"
##job.ROD.OutputLevel = DEBUG

## JEP CMMs
##input (from BS)
job.JEPCMMMaker.JetElementLocation="JetElements"
job.JEPCMMMaker.JEMHitsLocation = "JEMHits"
job.JEPCMMMaker.JEMEtSumsLocation = "JEMEtSums"
####input (from Simulation - intermediate data)
job.JEPCMMMaker.JetEtRoILocation = "Sim_JetEtRoIs"
job.JEPCMMMaker.EnergyRoILocation = "Sim_EnergyRoIs"
job.JEPCMMMaker.EtMapsLocation = "Sim_CMMEtSumsMAPS"
job.JEPCMMMaker.JetRoILocation = "Sim_JetRoIs"
##output
job.JEPCMMMaker.JEMRoILocation = "Sim_JEMRoIs"
job.JEPCMMMaker.CMMJetHitsLocation = "Sim_CMMJetHits"
job.JEPCMMMaker.CMMEtSumsLocation = "Sim_CMMEtSums"
job.JEPCMMMaker.CMMRoILocation = "Sim_CMMRoIs"
job.JEPCMMMaker.JEPBSCollectionLocation = "Sim_JEPBS"
job.JEPCMMMaker.JEPRoIBSCollectionLocation = "Sim_JEPRoIBS"
job.JEPCMMMaker.OutputLevel = ERROR

# CP CMMs
#input (from BS)
job.CPCMMMaker.CPMTowerLocation = "CPMTowers"
job.CPCMMMaker.CPMHitsLocation = "CPMHits"
job.CPCMMMaker.CPMRoILocation = "Sim_CPMRoIs"
#input (from Simulation - intermediate data)
job.CPCMMMaker.EmTauROILocation = "Sim_EmTauRoIs"
#output
job.CPCMMMaker.CMMCPHitsLocation = "Sim_CMMCPHits"
job.CPCMMMaker.CPBSCollectionLocation = "Sim_CPBS"
#job.CPCMMMaker.OutputLevel = DEBUG

