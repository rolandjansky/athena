################################################################################
####
###  unpackBS.py - simple jO to dump HLT result part of the BS,
##   put it in SG and optionally run EDM checker to dump it on screen
#
#
#  2009/09/09       Jiri.Masik@hep.manchester.ac.uk   
################################################################################

BSFileList = ["/afs/cern.ch/user/g/gencomm/w0/cote/NightlyTestInput/data09_cos.00121244.physics_IDCosmic.daq.RAW._lb0000._SFO-5._0017.data"]
edmCheck=True
MessageSvc.OutputLevel=INFO
EvtMax=-1

##############################
# Services
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
svcMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")

svcMgr.EventSelector.Input = BSFileList

from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
svcMgr += ROBDataProviderSvc()

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
svcMgr += EventSelectorByteStream("EventSelector")
theApp.EvtSel = "EventSelector"
theApp.EvtMax=EvtMax

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()

# Properties 
EventSelector = svcMgr.EventSelector
EventSelector.ByteStreamInputSvc     = "ByteStreamInputSvc"; 
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
#EventSelector.SkipEvents=262



# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
ByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2" ]
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_EF" ]
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_HLT" ]
# Address examples for DataScouting HLT results
#ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/DataScouting_03" ]
#ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/DataScouting_05" ]


# proxy provider
from SGComps.SGCompsConf import ProxyProviderSvc
svcMgr += ProxyProviderSvc()
ProxyProviderSvc = svcMgr.ProxyProviderSvc
ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]


from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
print job

from TrigBSExtraction.TrigBSExtractionConf import TrigBSExtraction
extr = TrigBSExtraction()
from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
extr.Navigation = HLTNavigationOffline()
# example for adding DataScouting results to decode
#extr.DSResultKeys += [ "DataScouting_03" ]
#extr.DSResultKeys += [ "DataScouting_05" ]



from TrigSerializeResult.TrigSerializeResultConf import TrigTSerializer

job += extr

svcMgr.StoreGateSvc.Dump=True



from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
TrigSerToolTP = TrigSerTPTool('TrigSerTPTool')
ToolSvc += TrigSerToolTP

from TrigEDMConfig.TriggerEDM import getL2BSTypeList,getEFBSTypeList, getTPList
TrigSerToolTP.ActiveClasses = getL2BSTypeList() + getEFBSTypeList()
TrigSerToolTP.TPMap = getTPList() 
                                                                                                

if edmCheck:
  from TrigValAlgs.TrigValAlgsConfig import TrigEDMChecker
  edmch = TrigEDMChecker()

  job+= edmch
