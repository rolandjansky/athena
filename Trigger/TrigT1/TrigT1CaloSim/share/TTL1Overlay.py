# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( "TTL1Overlay" )

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__OverlayTTL1


# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

job += LVL1__OverlayTTL1( 'OverlayTTL1' )

