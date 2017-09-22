###############################################################
#
# Job options 
#
# This is for validation of ITk tracking geometry.
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
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py")

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
if 'DetDescrVersion' not in dir():
       DetDescrVersion = 'ATLAS-P2-ITK-08-00-00'
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

# Material information and AtlasTrackingGeometrySvc loaded here
include("InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax           = 4000
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
ExtrapolationEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas', ToolOutputLevel=ExToolOutputLevel)
ToolSvc += ExtrapolationEngine

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
ExtrapolationEngineTest.BackExtrapolation       = False
# Smear the production vertex - standard primary vertex paramters
ExtrapolationEngineTest.SmearOrigin             = True
ExtrapolationEngineTest.SimgaOriginD0           = 0.015
ExtrapolationEngineTest.SimgaOriginZ0           = 55.6
ExtrapolationEngineTest.SmearFlatOriginZ0       = True
ExtrapolationEngineTest.Z0Min                   = -150.0
ExtrapolationEngineTest.Z0Max                   =  150.0
ExtrapolationEngineTest.SmearFlatOriginD0       = True
ExtrapolationEngineTest.D0Min                   = -2.0
ExtrapolationEngineTest.D0Max                   =  2.0
# pT range for testing                        
ExtrapolationEngineTest.PtMin                   = 1000
ExtrapolationEngineTest.PtMax                   = 1000
# The test range in Eta                      
ExtrapolationEngineTest.EtaMin                  =   -5
ExtrapolationEngineTest.EtaMax                  =   5.
# Configure how you wanna run                  
ExtrapolationEngineTest.CollectSensitive        = True
ExtrapolationEngineTest.CollectPassive          = True
ExtrapolationEngineTest.CollectBoundary         = True
ExtrapolationEngineTest.CollectMaterial         = True
# the path limit to test                        
ExtrapolationEngineTest.PathLimit               = -1.
# give it the engine
ExtrapolationEngineTest.ExtrapolationEngine     = ExtrapolationEngine
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

include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")

#==============================================================
#
# End of job options file
#
###############################################################

