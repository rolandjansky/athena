# jobOptions for SLHC geometry display 

execfile("ID_only.py")
execfile("geometry.py")

# geant4 or data - affects which conditions database instance is used
globalflags.DataSource='geant4'

#--------------------------------------------------------------
# To display the tracking geometry
#--------------------------------------------------------------
# --- setup of tracking geometry
if False :
  from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
  TrkDetFlags.SLHC_Geometry                   = True
  TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
  TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
  TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
  TrkDetFlags.MagneticFieldCallbackEnforced   = False
  TrkDetFlags.TRT_BuildStrawLayers            = False
  TrkDetFlags.MaterialDatabaseLocal           =  SLHC_Flags.SLHC_Version() != ''
  TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/LayerMaterialITK'
  TrkDetFlags.MaterialTagBase                 = 'AtlasLayerMat_v'
  TrkDetFlags.MaterialVersion                 = 17
  if TrkDetFlags.MaterialDatabaseLocal() is True :
     TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
     TrkDetFlags.MaterialDatabaseLocalPath    = ''
     TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
  TrkDetFlags.MagneticFieldCallbackEnforced         = False

# Start VP1
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# VP1 setup
from VP1Algs.VP1AlgsConf import VP1Alg
topSequence += VP1Alg()

# Number of events to be processed (default is 10)
theApp.EvtMax = 1
