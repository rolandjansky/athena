
# Get simulation flags
from G4AtlasApps.SimFlags import SimFlags
SimFlagsSimLayout.set_On()

# detector geometry (without this the default is used)
#SimFlags.SimLayout='ATLAS-CSC-02-00-00'

# Write out output:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput="larhits.pool.root"

# Turn on parameterizations for the active volumes, barrel stack, endcaps.
SimFlags.LArParameterization=2

# Sample commands to change single cuts and turn models on and off
#from LArG4FastSimSvc.LArG4FastSimSvcConf import LArG4EMBFastSimSvc,LArG4EMECFastSimSvc,LArG4FCALFastSimSvc,LArG4FCAL2FastSimSvc,LArG4HECFastSimSvc
 
# LArG4EMBFastSimSvc.EFlagToParametrise(False)
# LArG4EMBFastSimSvc.EMinE(1000)
# LArG4EMBFastSimSvc.EMaxE(5000000)
# LArG4EMBFastSimSvc.EFlagToKill(True)
# LArG4EMBFastSimSvc.EEtoKill(10)
# LArG4EMBFastSimSvc.EFlagShowerLib(True)
# LArG4EMBFastSimSvc.EMinEneShowerLib(10)
# LArG4EMBFastSimSvc.EMaxEneShowerLib(1000)

# LArG4EMECFastSimSvc.EFlagToParametrise(False)
# LArG4EMECFastSimSvc.EMinE(1000)
# LArG4EMECFastSimSvc.EMaxE(5000000)
# LArG4EMECFastSimSvc.EFlagToKill(True)
# LArG4EMECFastSimSvc.EEtoKill(10) 
# LArG4EMECFastSimSvc.EFlagShowerLib(True)
# LArG4EMECFastSimSvc.EMinEneShowerLib(10) 
# LArG4EMECFastSimSvc.EMaxEneShowerLib(1000) 

# LArG4FCALFastSimSvc.EFlagShowerLib(False)
# LArG4FCAL2FastSimSvc.EFlagShowerLib(False)

