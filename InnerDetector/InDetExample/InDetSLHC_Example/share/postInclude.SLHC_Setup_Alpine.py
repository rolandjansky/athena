#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_Alpine import SLHC_Setup
SLHC_Setup = SLHC_Setup()

include("InDetSLHC_Example/postInclude.SLHC_Setup_Common.py")
