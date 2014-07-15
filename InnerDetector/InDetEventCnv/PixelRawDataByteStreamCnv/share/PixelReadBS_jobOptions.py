###############################
# from
# include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
# Services
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
ServiceMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
ServiceMgr += ROBDataProviderSvc()
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
ServiceMgr += EventSelectorByteStream("EventSelector")

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
ServiceMgr += ByteStreamCnvSvc()

# Properties 
ServiceMgr.EventSelector.ByteStreamInputSvc = "ByteStreamInputSvc"; 
ServiceMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
ServiceMgr += ByteStreamAddressProviderSvc()

# proxy provider
from SGComps.SGCompsConf import ProxyProviderSvc
ServiceMgr += ProxyProviderSvc()
ServiceMgr.ProxyProviderSvc = svcMgr.ProxyProviderSvc
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]

###############################
# from
# include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
        "Trk::PixelClusterContainer/PixelOnlineClusters"
#        "Trk::SCT_ClusterContainer/SCT_OnlineClusters",
#        "Trk::TRT_DriftCircleContainer/TRT_DriftCircle"
] 

###############################
# from 
# include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "PixelRDO_Container/PixelRDOs" 
#        "SCT_RDO_Container/SCT_RDOs", 
#        "TRT_RDO_Container/TRT_RDOs"
] 
