""" SLHC_Setup_Reco_TrackingGeometry
    python module to configure the TrackingGeometry for reconstruction
"""

from AthenaCommon.AppMgr import ToolSvc

        
# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'COOL'
TrkDetFlags.MaterialVersion          = 20
TrkDetFlags.SLHC_Geometry = True

include("InDetSLHC_Example/SLHC_Setup_ITk_TrackingGeometry.py")

