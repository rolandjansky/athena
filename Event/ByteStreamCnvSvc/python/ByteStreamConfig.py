#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def TrigBSReadCfg( inputFlags ):
    """
    Creates accumulator for BS reading
    """
    filenames = inputFlags.Input.Files
    
    acc = ComponentAccumulator()
    
    from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc, ByteStreamEventStorageInputSvc, EventSelectorByteStream

    from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoSelectorTool 
    xconv = xAODMaker__EventInfoSelectorTool()

    eventSelector = EventSelectorByteStream("EventSelector")
    eventSelector.HelperTools += [xconv]
    acc.addService( eventSelector )
    acc.setAppProperty( "EvtSel", eventSelector.name() )

    bsInputSvc = ByteStreamEventStorageInputSvc( "ByteStreamInputSvc" )
    bsInputSvc.FullFileName = filenames
    acc.addService( bsInputSvc )

    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    eventPersistencySvc = EvtPersistencySvc( "EventPersistencySvc" )
    acc.addService( eventPersistencySvc )
    
    bsCnvSvc = ByteStreamCnvSvc()
    eventSelector.ByteStreamInputSvc = bsInputSvc.name();
    eventPersistencySvc.CnvServices = [ bsCnvSvc.name() ]
    acc.addService( bsCnvSvc )

    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc ) 

    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    bsAddressProviderSvc = ByteStreamAddressProviderSvc()
    acc.addService( bsAddressProviderSvc )

    from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
    iovMetaDataTool = IOVDbMetaDataTool()
    acc.addPublicTool( iovMetaDataTool )
    
    from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
    bsMetaDataTool = ByteStreamMetadataTool()
    bsMetaDataTool.InputCollName  = "ByteStreamMetadata"
    bsMetaDataTool.OutputCollName = "ByteStreamMetadata"
    acc.addPublicTool( bsMetaDataTool )
    
    from StoreGate.StoreGateConf import ProxyProviderSvc, StoreGateSvc
    metaDataStore = StoreGateSvc("MetaDataStore")   
    acc.addService( metaDataStore )
    inputMetaDataStore = StoreGateSvc("InputMetaDataStore")   
    acc.addService( inputMetaDataStore )

    from AthenaServices.AthenaServicesConf import MetaDataSvc
    metaDataSvc = MetaDataSvc()
    acc.addService( metaDataSvc )

    metaDataSvc.MetaDataContainer = "MetaDataHdr"
    metaDataSvc.MetaDataTools = [ iovMetaDataTool, bsMetaDataTool ]    
    
    proxy = ProxyProviderSvc()
    proxy.ProviderNames += [ bsAddressProviderSvc.name(), metaDataSvc.name() ]
    acc.addService( proxy )

    from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamAttListMetadataSvc
    acc.addService( ByteStreamAttListMetadataSvc() )
    
           
    # this is trigger specific and should only be loaded if some doTrigger flags is set
    # or it should be moved elsewhere, however, since there is no better location now let is stick here
    bsCnvSvc.InitCnvs += [ "EventInfo",
                        "ROIB::RoIBResult",
                        "HLT::HLTResult" ]
    
    bsAddressProviderSvc.TypeNames += [
        "TileCellIDC/TileCellIDC",
        "MdtDigitContainer/MDT_DIGITS",
        "RpcDigitContainer/RPC_DIGITS",
        "TgcDigitContainer/TGC_DIGITS",
        "CscDigitContainer/CSC_DIGITS",
        "ROIB::RecRoIBResult/RecRoIBResult",
        "MuCTPI_RIO/MUCTPI_RIO",
        "CTP_RIO/CTP_RIO"
    ]

    bsAddressProviderSvc.TypeNames += [
        "LArRawChannelContainer/LArRawChannels",
        "TileRawChannelContainer/TileRawChannelCnt",
        "ROIB::RoIBResult/RoIBResult",
        "MuCTPI_RDO/MUCTPI_RDO",
        "HLT::HLTResult/HLTResult_L2",
        "HLT::HLTResult/HLTResult_EF",
        "CTP_RDO/CTP_RDO",
        "L1TopoRDOCollection/L1TopoRDOCollection"
    ]


    
    if inputFlags.Input.isMC == False:        
        bsCnvSvc.GetDetectorMask=True
        # still need to figure out how conditions are setup in new system
        #from IOVDbSvc.CondDB import conddb
        #conddb.addFolder( 'TDAQ', '/TDAQ/RunCtrl/SOR_Params' )
        #acc.addService( conddb )    

    return acc

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW

    acc = TrigBSReadCfg( ConfigFlags )
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
