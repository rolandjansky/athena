# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the required algorithms to the sequence
job += LVL1__CPMSim( 'CPMSim' )
job += LVL1__JEMJetSim( 'JEMJetSim' )
job += LVL1__JEMEnergySim( 'JEMEnergySim' )
job += LVL1__CPCMX( 'CPCMX' )
job += LVL1__JetCMX( 'JetCMX' )
job += LVL1__EnergyCMX( 'EnergyCMX' )
job += LVL1__RoIROD( 'RoIROD' )



