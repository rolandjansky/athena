################################################################################
####
###  unpackFTKBS.py - simple jO to decode FTK part of the BS
##   
#
#  2017/03/16       Jiri.Masik@manchester.ac.uk   
################################################################################

BSFileList = ["data_test.00222510.Single_Stream.daq.RAW._lb0050._Athena._0001.data"]
edmCheck=True
MessageSvc.OutputLevel=DEBUG
EvtMax=-1

##############################

AthenaSealSvc = Service( "AthenaSealSvc" )
# Set up service
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )

# Services
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
svcMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")

svcMgr.ByteStreamInputSvc.FullFileName = BSFileList

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
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_HLT" ]
ByteStreamAddressProviderSvc.TypeNames += [ "FTK_RawTrackContainer/FTK_RDO_Tracks"]
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

# from TrigFTK_RecExample.TrigFTK_DataProviderSvc_Config import TrigFTK_DataProviderSvc
# theFTK_DataProviderSvc = TrigFTK_DataProviderSvc("TrigFTK_DataProviderSvc")
# ServiceMgr += theFTK_DataProviderSvc

# from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_ReaderAlgo

# FTK_RDO_Reader = FTK_RDO_ReaderAlgo( "FTK_RDO_ReaderAlgo")
# FTK_RDO_Reader.FTK_DataProvider=theFTK_DataProviderSvc
# FTK_RDO_Reader.RDO_CollectionName="FTK_RDO_Tracks"


# job += FTK_RDO_Reader

from TrigFTKByteStream.TrigFTKByteStreamConf import FTKDump
theDecoder = FTKDump()
job += theDecoder

svcMgr.StoreGateSvc.Dump=True



