# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# a number of configuration jobOptions to be used to build transient BS to
# be used for accessing cells during RDO accessing step

def configure_transBS():

             from AthenaCommon.AppMgr import ServiceMgr as svcMgr
             from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
             svcMgr += ByteStreamCnvSvc()
             # OutputSvc
             from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
             svcMgr += ROBDataProviderSvc()
             from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamRDP_OutputSvc
             svcMgr += ByteStreamRDP_OutputSvc()
             # Properties
             ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
             ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamRDP_OutputSvc"
             from AthenaServices.AthenaServicesConf import AthenaOutputStream
             StreamBS = AthenaOutputStream("StreamBS",EvtConversionSvc = "ByteStreamCnvSvc")
             StreamBS.ItemList +=["2721#*"]
             StreamBS.ItemList +=["2927#*"]
             from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
             svcMgr += ByteStreamAddressProviderSvc()

             svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
                  "TileCellIDC/TileCellIDC"
                  ]
             from AthenaCommon.AppMgr import ToolSvc
             from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
             ToolSvc+=LArRawDataContByteStreamToolConfig(InitializeForWriting=True)
             from SGComps.SGCompsConf import ProxyProviderSvc
             svcMgr += ProxyProviderSvc()
             svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
             return StreamBS
