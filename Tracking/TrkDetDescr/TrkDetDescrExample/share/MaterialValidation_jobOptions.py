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

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVersion = 'ATLAS-R1-2012-02-01-00'
from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil3')

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'COOL'
TrkDetFlags.MaterialVersion          = 17
TrkDetFlags.ConfigurationOutputLevel = VERBOSE

TrkDetFlags.MaterialDatabaseLocal        = True
if TrkDetFlags.MaterialDatabaseLocal() is True :
    # prepare the magic tag
    splitGeo = DetDescrVersion.split('-')
    MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]
    # now say where the file is
    TrkDetFlags.MaterialStoreGateKey        = '/GLOBAL/TrackingGeo/BinnedLayerMaterial'
    TrkDetFlags.MaterialDatabaseLocalPath    = '' # '/tmp/wlukas/'
    TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+DetDescrVersion+'.db'
    TrkDetFlags.MaterialMagicTag             = MaterialMagicTag
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False

## The TrackingGeometrySvc
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

###############################################################

from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__MaterialMapper
MaterialMapper = Trk__MaterialMapper(name='MaterialMapper')
MaterialMapper.OutputLevel = INFO
ToolSvc += MaterialMapper

# Add top algorithms to be run
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialValidation
MaterialValidation = Trk__MaterialValidation('MaterialValidation')
MaterialValidation.TrackingGeometrySvc         = AtlasTrackingGeometrySvc
MaterialValidation.MaterialMapper              = MaterialMapper
MaterialValidation.MinEta                      = -3.0
MaterialValidation.MaxEta                      = 3.0
MaterialValidation.OutputLevel                 = VERBOSE
job += MaterialValidation   # 1 alg, named "MaterialValidation"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 50000

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
