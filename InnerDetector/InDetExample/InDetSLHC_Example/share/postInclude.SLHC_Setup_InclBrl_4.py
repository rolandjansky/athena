#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_InclBrl_4 import SLHC_Setup
SLHC_Setup = SLHC_Setup()

include("InDetSLHC_Example/postInclude.SLHC_Setup_Common.py")

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
if (SLHC_Flags.LayoutOption == "InclinedQuads" or SLHC_Flags.LayoutOption == "InclinedDuals"):
    if hasattr(ServiceMgr, 'PixelLorentzAngleSvc'):
        # using LorentzAngleSvc
        print "--> Setting Lorentz angle to zero for the innermost pixel layer, as a rough modelling of 3D sensors"
        ServiceMgr.PixelLorentzAngleSvc.ITkL03D = True
