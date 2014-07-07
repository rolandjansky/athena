##############################################################
# Job options read a LArRampComplete object from a POOL file #
# and dump it to an ROOT ntuple                              #
# modified for commissioning                                 #
##############################################################

PoolFileNameList = ["RampPoolFileToRead.pool.root"]

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn()

from AthenaCommon.GlobalFlags  import GlobalFlags
GlobalFlags.DataSource.set_data()
GlobalFlags.DetGeo.set_commis()
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "AthenaCommon/Atlas.UnixStandardJob.py" )

# the Tile, LAr and Calo detector description package
DetDescrVersion = "ATLAS-CommNF-03-00-00"
include ("AtlasGeoModel/SetGeometryVersion.py")
include( "AtlasGeoModel/GeoModelInit.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )

theApp.EvtMax = 1

CondProxyProvider = Service( "CondProxyProvider" )
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]
CondProxyProvider.InputCollections += PoolFileNameList


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc 
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)


from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
LArRamps2Ntuple.ContainerKey = ["HIGH","MEDIUM","LOW"]
LArRamps2Ntuple.RawRamp=False
LArRamps2Ntuple.SaveAllSamples=False

topSequence+= LArRamps2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='LArRampFromPool.root' OPT='NEW'" ]


AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

theApp.EvtMax=1
svcMgr.MessageSvc.OutputLevel=DEBUG

#StoreGateSvc=Service("StoreGateSvc");
#StoreGateSvc.dump=TRUE

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE
