#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# smaller beam pipe
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.BeamPipeRadius.set_Value_and_Lock(28.5)

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_ExtBrl_4_33mm import SLHC_Setup_XMLReader
SLHC_Setup_XMLReader = SLHC_Setup_XMLReader()
