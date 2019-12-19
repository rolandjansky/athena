#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ByteStreamReadCfg( inputFlags, typeNames=[] ):
    """
    Creates accumulator for BS reading
    """
    filenames = inputFlags.Input.Files

    

    acc = ComponentAccumulator()
    
    ByteStreamCnvSvc, ByteStreamEventStorageInputSvc, EventSelectorByteStream=CompFactory.getComps("ByteStreamCnvSvc","ByteStreamEventStorageInputSvc","EventSelectorByteStream",)

    xAODMaker__EventInfoSelectorTool, =CompFactory.getComps("xAODMaker__EventInfoSelectorTool",)
    xconv = xAODMaker__EventInfoSelectorTool()

    eventSelector = EventSelectorByteStream("EventSelector")
    eventSelector.HelperTools += [xconv]
    acc.addService( eventSelector )
    acc.setAppProperty( "EvtSel", eventSelector.name() )

    bsInputSvc = ByteStreamEventStorageInputSvc( "ByteStreamInputSvc" )
    bsInputSvc.FullFileName = filenames
    acc.addService( bsInputSvc )

    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    eventPersistencySvc = EvtPersistencySvc( "EventPersistencySvc" )
    acc.addService( eventPersistencySvc )
    
    bsCnvSvc = ByteStreamCnvSvc()
    eventSelector.ByteStreamInputSvc = bsInputSvc.name()
    eventPersistencySvc.CnvServices = [ bsCnvSvc.name() ]
    acc.addService( bsCnvSvc )

    ROBDataProviderSvc=CompFactory.ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc ) 

    ByteStreamAddressProviderSvc=CompFactory.ByteStreamAddressProviderSvc
    bsAddressProviderSvc = ByteStreamAddressProviderSvc(TypeNames=typeNames)
    acc.addService( bsAddressProviderSvc )

    IOVDbMetaDataTool=CompFactory.IOVDbMetaDataTool
    iovMetaDataTool = IOVDbMetaDataTool()
    acc.addPublicTool( iovMetaDataTool )
    
    ByteStreamMetadataTool=CompFactory.ByteStreamMetadataTool
    bsMetaDataTool = ByteStreamMetadataTool()
    acc.addPublicTool( bsMetaDataTool )
    
    StoreGateSvc=CompFactory.StoreGateSvc
    ProxyProviderSvc=CompFactory.ProxyProviderSvc
    metaDataStore = StoreGateSvc("MetaDataStore")   
    acc.addService( metaDataStore )
    inputMetaDataStore = StoreGateSvc("InputMetaDataStore")   
    acc.addService( inputMetaDataStore )

    MetaDataSvc=CompFactory.MetaDataSvc
    metaDataSvc = MetaDataSvc()
    acc.addService( metaDataSvc )

    metaDataSvc.MetaDataContainer = "MetaDataHdr"
    metaDataSvc.MetaDataTools = [ iovMetaDataTool, bsMetaDataTool ]    
    
    proxy = ProxyProviderSvc()
    proxy.ProviderNames += [ bsAddressProviderSvc.name(), metaDataSvc.name() ]
    acc.addService( proxy )

    ByteStreamAttListMetadataSvc=CompFactory.ByteStreamAttListMetadataSvc
    acc.addService( ByteStreamAttListMetadataSvc() )
    
    bsCnvSvc.InitCnvs += [ "EventInfo",]

    return acc

def TrigBSReadCfg(inputFlags):

    acc=ByteStreamReadCfg( inputFlags )

    bsCnvSvc=acc.getService("ByteStreamCnvSvc")
    bsCnvSvc.InitCnvs += ["HLT::HLTResult" ]
    
    bsAddressProviderSvc=acc.getService("ByteStreamAddressProviderSvc")

    bsAddressProviderSvc.TypeNames += [
        "TileCellIDC/TileCellIDC",
        "MdtDigitContainer/MDT_DIGITS",
        "RpcDigitContainer/RPC_DIGITS",
        "TgcDigitContainer/TGC_DIGITS",
        "CscDigitContainer/CSC_DIGITS",
        "MuCTPI_RIO/MUCTPI_RIO",
        "CTP_RIO/CTP_RIO"
    ]

    bsAddressProviderSvc.TypeNames += [
        "LArRawChannelContainer/LArRawChannels",
        "TileRawChannelContainer/TileRawChannelCnt",
        "MuCTPI_RDO/MUCTPI_RDO",
        "HLT::HLTResult/HLTResult_L2",
        "HLT::HLTResult/HLTResult_EF",
        "CTP_RDO/CTP_RDO",
        "L1TopoRDOCollection/L1TopoRDOCollection"
    ]


    
    if inputFlags.Input.isMC is False:
        bsCnvSvc.GetDetectorMask=True
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge(addFolders(inputFlags,'/TDAQ/RunCtrl/SOR_Params','TDAQ' ))
        # still need to figure out how conditions are setup in new system
        #from IOVDbSvc.CondDB import conddb
        #conddb.addFolder( 'TDAQ', '/TDAQ/RunCtrl/SOR_Params' )
        #acc.addService( conddb )    

    return acc

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    ConfigFlags.Input.Files = defaultTestFiles.RAW

    acc = TrigBSReadCfg( ConfigFlags )
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")
