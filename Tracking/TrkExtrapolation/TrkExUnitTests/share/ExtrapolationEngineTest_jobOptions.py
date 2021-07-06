##############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------


#--------------------------------------------------------------
# Geometry section
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
#DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVersion = 'ATLAS-R2-2016-01-00-03'
#DetDescrVersion = 'ATLAS-PX-ITK-00-00-00'
from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.PixelBuildingOutputLevel = INFO
TrkDetFlags.SCT_BuildingOutputLevel  = INFO
TrkDetFlags.TRT_BuildingOutputLevel  = INFO
TrkDetFlags.ConfigurationOutputLevel = INFO
TrkDetFlags.TRT_BuildStrawLayers     = False
#TrkDetFlags.TRT_BuildStrawLayers     = True

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource              = 'COOL'
#TrkDetFlags.MaterialSource              = 'Input'
TrkDetFlags.MaterialVersion             = 20
TrkDetFlags.MaterialSubVersion          = ''
#TrkDetFlags.MaterialSubVersion          = 'f'
TrkDetFlags.ConfigurationOutputLevel    = VERBOSE

TrkDetFlags.MaterialDatabaseLocal       = False
if TrkDetFlags.MaterialDatabaseLocal() is True :
    # prepare the magic tag
    splitGeo = DetDescrVersion.split('-')
    MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]
    # now say where the file is
    TrkDetFlags.MaterialStoreGateKey        = '/GLOBAL/TrackingGeo/BinnedLayerMaterial'
    TrkDetFlags.MaterialDatabaseLocalPath    = '' # '/tmp/wlukas/'
    TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+DetDescrVersion+'.db'
    TrkDetFlags.MaterialMagicTag             = MaterialMagicTag
    TrkDetFlags.MaterialSubVersion           = ''
TrkDetFlags.MagneticFieldCallbackEnforced    = False

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax           = 1000
ExToolOutputLevel       = INFO # VERBOSE # INFO #
ExAlgorithmOutputLevel  = INFO # VERBOSE #

from AthenaCommon.AppMgr import ServiceMgr
# output level
ServiceMgr.MessageSvc.OutputLevel  = INFO
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#--------------------------------------------------------------
# Tool setup
#--------------------------------------------------------------

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil3')

from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
ExtrapolationEninge = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas', ToolOutputLevel=ExToolOutputLevel)
ToolSvc += ExtrapolationEninge

from TrkValTools.TrkValToolsConf import Trk__PositionMomentumWriter as PmWriter
PmWriter = PmWriter()
ToolSvc += PmWriter


#--------------------------------------------------------------
# Algorithm setup
#--------------------------------------------------------------

# Add top algorithms to be run
from TrkExUnitTests.TrkExUnitTestsConf import Trk__ExtrapolationEngineTest
ExtrapolationEngineTest = Trk__ExtrapolationEngineTest('ExtrapolationEngineTest')
# how many tests you want per event 
ExtrapolationEngineTest.NumberOfTestsPerEvent   = 100
# parameters mode: 0 - neutral tracks, 1 - charged particles 
ExtrapolationEngineTest.ParametersMode          = 1
# do the full test backwards as well            
ExtrapolationEngineTest.BackExtrapolation       = True
# Smear the production vertex - standard primary vertex paramters
ExtrapolationEngineTest.SmearOrigin             = False   
ExtrapolationEngineTest.SimgaOriginD0           = 0.015 
ExtrapolationEngineTest.SimgaOriginZ0           = 55.6
# pT range for testing                        
ExtrapolationEngineTest.PtMin                   = 1000
ExtrapolationEngineTest.PtMax                   = 1000
# The test range in Eta                      
ExtrapolationEngineTest.EtaMin                  =  -3.5
ExtrapolationEngineTest.EtaMax                  =   3.5
# Configure how you wanna run                  
ExtrapolationEngineTest.CollectSensitive        = True
ExtrapolationEngineTest.CollectPassive          = True
ExtrapolationEngineTest.CollectBoundary         = True
ExtrapolationEngineTest.CollectMaterial         = True
# the path limit to test                        
ExtrapolationEngineTest.PathLimit               = -1.
# give it the engine
ExtrapolationEngineTest.ExtrapolationEngine     = ExtrapolationEninge
# validation tool 
ExtrapolationEngineTest.PositionMomentumWriter  = PmWriter
# output formatting
ExtrapolationEngineTest.OutputLevel             = ExAlgorithmOutputLevel
job += ExtrapolationEngineTest   # 1 alg, named 'ExtrapolationEngineTest'


#################################################################
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'

# --- load AuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
# --- write out summary of the memory usage
#   | number of events to be skip to detect memory leak
#   | 20 is default. May need to be made larger for complete jobs.
ServiceMgr.AuditorSvc += getConfigurable('ChronoAuditor')()
# --- write out a short message upon entering or leaving each algorithm
#
theApp.AuditAlgorithms = True
theApp.AuditServices   = True
# 
# --- Display detailed size and timing statistics for writing and reading
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True


if not hasattr(ServiceMgr, 'THistSvc'):
       from GaudiSvc.GaudiSvcConf import THistSvc
       ServiceMgr += THistSvc()
# add the G4 validation output stream
ServiceMgr.THistSvc.Output += [ "val DATAFILE='ExtrapolationEngineTest.root' TYPE='ROOT' OPT='RECREATE'" ]

#==============================================================
#
# End of job options file
#
###############################################################
