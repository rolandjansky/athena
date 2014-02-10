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

# output collection name
MaterialStepCollectionName       = 'RandomMaterialSteps'
LayerMaterialCollectionName      = 'RandomLayerMaterial'

# Add top algorithms to be run
from TrkDetDescrUnitTests.TrkDetDescrUnitTestsConf import Trk__TrkDetDescrTPCnvTest
TrkDetDescrTPConfTest = Trk__TrkDetDescrTPCnvTest('TrkDetDescrTPConfTest')
TrkDetDescrTPConfTest.NumberOfTestsPerEvent         = 100
TrkDetDescrTPConfTest.MaterialStepCollection   = MaterialStepCollectionName
TrkDetDescrTPConfTest.LayerMaterialMap         = LayerMaterialCollectionName
TrkDetDescrTPConfTest.OutputLevel              = VERBOSE
job += TrkDetDescrTPConfTest   # 1 alg, named "BinUtilityTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

#import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr
svcMgr = theApp.serviceMgr()
svcMgr.MessageSvc.OutputLevel 	= VERBOSE
svcMgr.MessageSvc.Format 	= "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit	= 1000000
svcMgr.AthenaSealSvc.OutputLevel = VERBOSE

#  level for HelloAlg only (note name: instance, not type)
BinUtilityTest = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 5

#==============================================================
#
# End of job options file
#
###############################################################


############### The Material hit collection ##################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# --- check dictionary
ServiceMgr.AthenaSealSvc.CheckDictionary   = True
# --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10
MaterialStream              = AthenaPoolOutputStream ( 'TrkDetDescrTPCnv' )
MaterialStream.OutputFile   =   "TrkDetDescrTPCnvTest.root" 
MaterialStream.ItemList    += [ 'Trk::MaterialStepCollection#'+MaterialStepCollectionName ]
MaterialStream.ItemList    += [ 'Trk::LayerMaterialMap#'+LayerMaterialCollectionName ]
