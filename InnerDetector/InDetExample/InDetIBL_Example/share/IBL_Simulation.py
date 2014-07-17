#--------------------------------------------------------------
# Template jobOptions: IBL 
#  - Simulation
#--------------------------------------------------------------

## from AthenaCommon.DetFlags import DetFlags
## DetFlags.ID_setOn()
## DetFlags.Calo_setOff()
## DetFlags.Muon_setOff()
## DetFlags.simulate.Truth_setOn()

# SLHC Flags -----------------------------------------------------
from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "IBL-01"

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetIBL_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#MessageSvc = Service( "MessageSvc" )
#MessageSvc.OutputLevel      = INFO
#MessageSvc.defaultLimit     = 1000000
#MessageSvc.Format           = "% F%37W%S%7W%R%T %0W%M"
