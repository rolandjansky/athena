# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()


from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
from AthenaCommon.GlobalFlags import globalflags
# globalflags.DetDescrVersion = "ATLAS-P2-ITK-17-00-01"
# build GeoModel

# build GeoModel
if 'myGeo' not in dir():
  DetDescrVersion = 'ATLAS-P2-ITK-20-03-00'
  print "=== 'myGeo' not specified, defaulting to",DetDescrVersion," geometry! ==="
else: DetDescrVersion = myGeo

globalflags.DetDescrVersion = DetDescrVersion

## Need to set the layout option up front
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
if globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-20'):
   SLHC_Flags.LayoutOption="InclinedDuals"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-19'):
   SLHC_Flags.LayoutOption="InclinedQuads"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-17'):
   SLHC_Flags.LayoutOption="InclinedAlternative"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22'):
   SLHC_Flags.LayoutOption="InclinedAlternative"

SLHC_Flags.doGMX.set_Value_and_Lock(True)


include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel


from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = globalflags.DetDescrVersion()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += GeoModelSvc

svcMgr.MessageSvc.OutputLevel  = INFO
# to change the default limit on number of message
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'COOL'
TrkDetFlags.MaterialVersion          = 20
TrkDetFlags.ConfigurationOutputLevel = INFO
TrkDetFlags.PixelBuildingOutputLevel = INFO
TrkDetFlags.SCT_BuildingOutputLevel  = INFO
TrkDetFlags.TRT_BuildingOutputLevel  = INFO
TrkDetFlags.ConfigurationOutputLevel = INFO
TrkDetFlags.TRT_BuildStrawLayers     = False

TrkDetFlags.MaterialDatabaseLocal        = True
if TrkDetFlags.MaterialDatabaseLocal() is True :
    # prepare the magic tag
    MaterialMagicTag = DetDescrVersion
    # now say where the file is
    # Make sure the key and DB local name match what you prepared earlier
    TrkDetFlags.MaterialStoreGateKey        = '/GLOBAL/TrackingGeo/LayerMaterialITK'
    TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+DetDescrVersion+'.db'
    TrkDetFlags.MaterialMagicTag             = MaterialMagicTag
TrkDetFlags.MagneticFieldCallbackEnforced         = False

## The TrackingGeometrySvc
include("InDetSLHC_Example/SLHC_Setup_ITk_TrackingGeometry.py")
#ToolSvc.AtlasMaterialProvider.OutputLevel                 = VERBOSE

###############################################################

from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__MaterialMapper
MaterialMapper = Trk__MaterialMapper(name='MaterialMapper')
MaterialMapper.OutputLevel = INFO
ToolSvc += MaterialMapper

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil3')

# Add top algorithms to be run
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialValidation
MaterialValidation = Trk__MaterialValidation('MaterialValidation')
MaterialValidation.TrackingGeometrySvc         = AtlasTrackingGeometrySvc
MaterialValidation.MaterialMapper              = MaterialMapper
MaterialValidation.MinEta                      = -6.0
MaterialValidation.MaxEta                      = 6.0
MaterialValidation.OutputLevel                 = INFO
job += MaterialValidation   # 1 alg, named "MaterialValidation"

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 100000

#################################################################
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'

TempOutputFolder = '' #'/tmp/wlukas/'
ValidationOutputNameString = TempOutputFolder+'AtlasTrackingGeometry-'

if not hasattr(ServiceMgr, 'THistSvc'):
       from GaudiSvc.GaudiSvcConf import THistSvc
       ServiceMgr += THistSvc()
# add the G4 validation output stream
ServiceMgr.THistSvc.Output += [ "val DATAFILE='"+ValidationOutputNameString+DetDescrVersion+".root' TYPE='ROOT' OPT='RECREATE'" ]

#==============================================================
#
# End of job options file
#
###############################################################

include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")
