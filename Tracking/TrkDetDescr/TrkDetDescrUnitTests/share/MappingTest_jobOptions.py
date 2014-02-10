###############################################################
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

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVerstion = 'ATLAS-GEO-20-00-00'
from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SIM-00-00-00")

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialFromCool 			    = False
TrkDetFlags.ConfigurationOutputLevel        = INFO
TrkDetFlags.PixelBuildingOutputLevel 		= INFO
TrkDetFlags.SCT_BuildingOutputLevel 		= INFO
TrkDetFlags.TRT_BuildingOutputLevel 		= INFO
TrkDetFlags.InDetBuildingOutputLevel 		= INFO

# load the tracking geometry service
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

# Add top algorithms to be run
from TrkDetDescrUnitTests.TrkDetDescrUnitTestsConf import Trk__MappingTest
MappingTest = Trk__MappingTest('MappingTest')
MappingTest.TrackingGeometrySvc         = AtlasTrackingGeometrySvc
MappingTest.HighestVolume =  'InDet::Detectors::BpPixSctTrt'
MappingTest.NumberOfTestsPerEvent       = 1000000
job += MappingTest   # 1 alg, named "MappingTest"

# load the THistSvc
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc() 
ServiceMgr.THistSvc.Output = [ "val DATAFILE='MappingTest.root' TYPE='ROOT' OPT='RECREATE'" ]

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for HelloAlg only (note name: instance, not type)
MappingTest = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 1

#==============================================================
#
# End of job options file
#
###############################################################
