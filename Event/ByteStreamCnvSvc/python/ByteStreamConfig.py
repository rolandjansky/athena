#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ByteStreamReadCfg( inputFlags, typeNames=[] ):
    """
    Creates accumulator for BS reading
    """

    acc = ComponentAccumulator()
    
    ByteStreamCnvSvc, ByteStreamEventStorageInputSvc, EventSelectorByteStream=CompFactory.getComps("ByteStreamCnvSvc","ByteStreamEventStorageInputSvc","EventSelectorByteStream",)

    if inputFlags.Input.SecondaryFiles:
        filenames = inputFlags.Input.SecondaryFiles
        eventSelector = EventSelectorByteStream("SecondaryEventSelector", IsSecondary=True)
        acc.addService( eventSelector )
    else:
        filenames = inputFlags.Input.Files
        xAODMaker__EventInfoSelectorTool = CompFactory.xAODMaker.EventInfoSelectorTool
        xconv = xAODMaker__EventInfoSelectorTool()
        eventSelector = EventSelectorByteStream("EventSelector")
        eventSelector.HelperTools += [xconv]
        eventSelector.SkipEvents=inputFlags.Exec.SkipEvents
        acc.addService( eventSelector )
        acc.setAppProperty( "EvtSel", eventSelector.name )

    bsInputSvc = ByteStreamEventStorageInputSvc( "ByteStreamInputSvc" )
    bsInputSvc.FullFileName = filenames
    if inputFlags.Overlay.DataOverlay:
        bsInputSvc.EventInfoKey = inputFlags.Overlay.BkgPrefix + "EventInfo"
    acc.addService( bsInputSvc )

    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    eventPersistencySvc = EvtPersistencySvc( "EventPersistencySvc" )
    acc.addService( eventPersistencySvc )
    
    bsCnvSvc = ByteStreamCnvSvc()
    eventSelector.ByteStreamInputSvc = bsInputSvc.name
    eventPersistencySvc.CnvServices = [ bsCnvSvc.name ]
    acc.addService( bsCnvSvc )

    ROBDataProviderSvc=CompFactory.ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc ) 

    ByteStreamAddressProviderSvc=CompFactory.ByteStreamAddressProviderSvc
    bsAddressProviderSvc = ByteStreamAddressProviderSvc(TypeNames=typeNames)
    acc.addService( bsAddressProviderSvc )
    
    ProxyProviderSvc=CompFactory.ProxyProviderSvc

    from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
    acc.merge(MetaDataSvcCfg(inputFlags, ["IOVDbMetaDataTool", "ByteStreamMetadataTool"]))
    
    proxy = ProxyProviderSvc()
    proxy.ProviderNames += [ bsAddressProviderSvc.name ]
    acc.addService( proxy )

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


def ByteStreamWriteCfg( flags, typeNames=[] ):
    acc = ComponentAccumulator("AthOutSeq")
    outputSvc = CompFactory.ByteStreamEventStorageOutputSvc()
    outputSvc.MaxFileMB = 15000
    # event (beyond which it creates a new file)
    outputSvc.MaxFileNE = 15000000
    outputSvc.OutputDirectory = "./"
    outputSvc.AppName = "Athena"
    # release variable depends the way the env is configured
    #FileTag = release
    allRuns = set(flags.Input.RunNumber)
    assert len(allRuns) == 1, "The input is from multiple runs, do not know which one to use {}".format(allRuns)
    outputSvc.RunNumber = allRuns.pop()

    bsCnvSvc  = CompFactory.ByteStreamCnvSvc("ByteStreamCnvSvc")

    bsCnvSvc.ByteStreamOutputSvcList = [ outputSvc.getName() ]
    streamAlg = CompFactory.AthenaOutputStream( "BSOutputStreamAlg",
                                                EvtConversionSvc = bsCnvSvc.getName(),
                                                OutputFile = "ByteStreamEventStorageOutputSvc",
                                                ItemList = typeNames )

    acc.addService( outputSvc )
    acc.addService( bsCnvSvc )
    acc.addEventAlgo( streamAlg, primary = True )
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
