###############################################################
#
# Job options file to read input POOL file using CondProxyProvider
# and output a new pool file and sqlite file
#
#==============================================================

inputCollections = ["alignment_in.root"]
outputFile = "indetalignment_cscalign_test.root"
objectList = [ "AlignableTransformContainer#/Indet/Align",
               "AlignableTransformContainer#/TRT/Align"]
tagList = ["IndetAlign_cscalign_test",
           "TRTAlign_cscalign_test"]

#==============================================================

from AthenaCommon.GlobalFlags import GlobalFlags

#GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()

GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()

# If you need the geometry
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.ID_setOn()
#from AtlasGeoModel import GeoModelInit
#from AtlasGeoModel import SetGeometryVersion
#from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion = 'ATLAS-CSC-02-00-00'


from IOVDbSvc.CondDB import conddb

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += CondProxyProvider()
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
# set this to the file containing AlignableTransform objects
ServiceMgr.CondProxyProvider.InputCollections += inputCollections
ServiceMgr.CondProxyProvider.OutputLevel=DEBUG
print ServiceMgr.CondProxyProvider

# this preload causes callbacks for read in objects to be activated,
# allowing GeoModel to pick up the transforms
ServiceMgr.IOVSvc.preLoadData=True
ServiceMgr.IOVSvc.OutputLevel=DEBUG

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg(outputFile=outputFile,
                                        ObjectList=objectList,
                                        IOVTagList=tagList)
#OutputConditionsAlg.Run1=3
#OutputConditionsAlg.Event1=4
#OutputConditionsAlg.Run2=9999
#OutputConditionsAlg.Event2=999999

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# To set global output level use
# athena -l DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

#==============================================================
#
# End of job options file
#
###############################################################
