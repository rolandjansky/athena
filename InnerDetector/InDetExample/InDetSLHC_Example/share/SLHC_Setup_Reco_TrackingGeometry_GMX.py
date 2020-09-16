""" SLHC_Setup_Reco_TrackingGeometry_GMX
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
if globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-17') or  globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22') :
  TrkDetFlags.MaterialVersion         = 20
else:
  TrkDetFlags.MaterialVersion         = 21
TrkDetFlags.MaterialSubVersion      = ""
# change for GMX: also need splitGeo[4] and splitGeo[5]
TrkDetFlags.MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2] + '-' + splitGeo[3] + '-' + splitGeo[4] + '-' + splitGeo[5]

include("InDetSLHC_Example/SLHC_Setup_ITk_TrackingGeometry.py")
