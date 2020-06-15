#---------------------------------------------------------------
# Read RDO from BS
# --------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

# InputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
ByteStreamInputSvc = ByteStreamEventStorageInputSvc("ByteStreamInputSvc")
ServiceMgr            += ByteStreamInputSvc
print                    ByteStreamInputSvc

# get EventSelector
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
EventSelector  = EventSelectorByteStream("EventSelector",
                                         Input = ["RawEvent.re"],
                                         ByteStreamInputSvc = "ByteStreamInputSvc")
ServiceMgr    += EventSelector
# declare EventSelector to theApp
theApp.EvtSel  = "EventSelector"

# ROBDataProvider
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
ROBDataProviderSvc = ROBDataProviderSvc()
ServiceMgr        += ROBDataProviderSvc
print                ROBDataProviderSvc

# Conversion Service instance
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
ByteStreamCnvSvc           = ByteStreamCnvSvc("ByteStreamCnvSvc",
                                              IsSimulation = True)
ByteStreamCnvSvc.InitCnvs += ["PixelRDO_Container", "PixelRDO_TB04_Collection"]
ByteStreamCnvSvc.InitCnvs += ["SCT_RDO_Container", "SCT_RDO_Collection"]
ByteStreamCnvSvc.InitCnvs += ["TRT_RDO_Container", "InDetRawDataCollection<TRT_RDORawData>"]
ServiceMgr += ByteStreamCnvSvc
print         ByteStreamCnvSvc

# and ByteStreamCnvSvc persistency service
ServiceMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
print ServiceMgr.EventPersistencySvc

# set up ByteStreamAddressProvider
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
ByteStreamAddressProviderSvc = ByteStreamAddressProviderSvc("ByteStreamAddressProviderSvc")
ByteStreamAddressProviderSvc.TypeNames += ["PixelRDO_Container/PixelRDOs"]
ByteStreamAddressProviderSvc.TypeNames += ["SCT_RDO_Container/SCT_RDOs"]
ByteStreamAddressProviderSvc.TypeNames += ["TRT_RDO_Container/TRT_RDOs"]
ServiceMgr += ByteStreamAddressProviderSvc
print         ByteStreamAddressProviderSvc

# load proxy provider service
from SGComps.SGCompsConf import ProxyProviderSvc
ProxyProviderSvc                = ProxyProviderSvc()
ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
ServiceMgr                     += ProxyProviderSvc
print                             ProxyProviderSvc



#
