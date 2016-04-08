from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- no TRT for SLHC
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOff()
DetFlags.makeRIO.TRT_setOff()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# --- printout
DetFlags.Print()

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSLHC = True

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = ''

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry                   = True
TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
TrkDetFlags.MaterialFromCool                = True
TrkDetFlags.MaterialDatabaseLocal           = False and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
if SLHC_Flags.SLHC_Version() is '' :
 TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
else :
 TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version() 
if TrkDetFlags.MaterialDatabaseLocal() is True :
   TrkDetFlags.MaterialDatabaseLocalPath    = ''
   TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False

