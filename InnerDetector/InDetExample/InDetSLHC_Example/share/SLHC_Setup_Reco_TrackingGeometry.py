""" SLHC_Setup_Reco_TrackingGeometry
    Python module to hold storegate keys for material in case of LTF layouts
    Author: Noemi Calace    
"""

# taking the material information
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()
splitGeo        = DetDescrVersion.split('-')
TrkDetFlags.SLHC_Geometry           = True
TrkDetFlags.MaterialStoreGateKey    = "/GLOBAL/TrackingGeo/LayerMaterialITK"
TrkDetFlags.MaterialVersion         = 20
TrkDetFlags.MaterialSubVersion      = ""
TrkDetFlags.MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2] + '-' + splitGeo[3]

include("InDetSLHC_Example/SLHC_Setup_ITk_TrackingGeometry.py")
