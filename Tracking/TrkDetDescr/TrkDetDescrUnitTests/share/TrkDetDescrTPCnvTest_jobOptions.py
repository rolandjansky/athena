###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Cnviguration options
#--------------------------------------------------------------
WriteFile = False

# Use McEventSelector so we can run with AthenaMP
if WriteFile :
    import AthenaCommon.AtlasUnixGeneratorJob
else :
    import AthenaCommon.AtlasUnixStandardJob
    import AthenaPoolCnvSvc.ReadAthenaPool 
    ServiceMgr.EventSelector.InputCollections = [ 'TrkDetDescrTPCnvTest.root' ]

#--------------------------------------------------------------
# Private Application Cnviguration options
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
ElementTableName                 = 'ElementTableName'

# Add top algorithms to be run
from TrkDetDescrUnitTests.TrkDetDescrUnitTestsConf import Trk__TrkDetDescrTPCnvTest
TrkDetDescrTPCnvTest = Trk__TrkDetDescrTPCnvTest('TrkDetDescrTPCnvTest')
TrkDetDescrTPCnvTest.NumberOfTestsPerEvent         = 100
TrkDetDescrTPCnvTest.MaterialStepCollection        = MaterialStepCollectionName
TrkDetDescrTPCnvTest.LayerMaterialMap              = LayerMaterialCollectionName
TrkDetDescrTPCnvTest.ElementTable                  = ElementTableName
TrkDetDescrTPCnvTest.WriteMode                     = WriteFile
TrkDetDescrTPCnvTest.OutputLevel                   = VERBOSE
job += TrkDetDescrTPCnvTest   # 1 alg, named "     TrkDetDescrTPCnvTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

#import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr
svcMgr = theApp.serviceMgr()
svcMgr.MessageSvc.OutputLevel 	= VERBOSE
svcMgr.MessageSvc.Format 	= "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit	= 1000000

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
theApp.EvtMax = 1

#==============================================================
#
# End of job options file
#
###############################################################


############### The Material hit collection ##################
if WriteFile :
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    # --- commit interval (test)
    ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
    MaterialStream              = AthenaPoolOutputStream ( 'TrkDetDescrTPCnv' )
    MaterialStream.OutputFile   =   'TrkDetDescrTPCnvTest.root' 
    MaterialStream.ItemList    += [ 'Trk::MaterialStepCollection#'+MaterialStepCollectionName ]
    MaterialStream.ItemList    += [ 'Trk::LayerMaterialMap#'+LayerMaterialCollectionName ]
    MaterialStream.ItemList    += [ 'Trk::ElementTable#'+ElementTableName ]
