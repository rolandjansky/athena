# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: Karsten Koeneke



def GetCurrentSkimCycleFromMetaDataItemList( metaDataItemList ):
    """
    Get the current skimming cyclye from the input meta data from AthFile.
    """
    inputCycle=-1
    try:
        for item in metaDataItemList:
            if item[0] == "xAOD::CutBookkeeperContainer" :
                if item[1].startswith("ProcessingCycle"):
                    tmpCycle = int(item[1].lstrip("ProcessingCycle"))
                    if tmpCycle > inputCycle: inputCycle = tmpCycle
                    pass
                elif item[1].startswith("cycle"):
                    tmpCycle = int(item[1].lstrip("cycle"))
                    if tmpCycle > inputCycle: inputCycle = tmpCycle
                    pass
                pass
            pass
        pass
    except:
        print "WARNING Unable to determine skim cycle from AthFile metadata_item."
        pass
    if inputCycle<0:
        currentCycle=0
    else:
        currentCycle=inputCycle+1
        pass
    return currentCycle



def GetCurrentSkimCycleFromInputFileSummary( inputFileSummary ):
    """
    Get the current skimming cyclye from the input meta data from the inputFileSummary.
    """
    inputCycle=-1
    try:
        metaDataItemList = inputFileSummary['metadata_itemsList']
        for item in metaDataItemList:
            if item.startswith("xAOD::CutBookkeeperContainer#ProcessingCycle") :
                tmpCycle = int(item.lstrip("xAOD::CutBookkeeperContainer#ProcessingCycle"))
                if tmpCycle > inputCycle: inputCycle = tmpCycle
                pass
            pass
        if cycle == -1:
            for item in metaDataItemList:
                if item.startswith("EventBookkeeperCollection#cycle"):
                    tmpCycle = int(item.lstrip("EventBookkeeperCollection#cycle"))
                    if tmpCycle > inputCycle: inputCycle = tmpCycle
                    pass
                pass
            pass
        pass
    except:
        print "WARNING Unable to determine skim cycle from inputFileSummary."
        pass
    if inputCycle<0:
        currentCycle=0
    else:
        currentCycle=inputCycle+1
        pass
    return currentCycle




def GetInputStreamNameFromMetaDataItemList( metaDataItemList ):
    inputStreamName = ""
    try:
        #metaDataItemList=inputFileSummary['metadata_itemsList']
        for item in metaDataItemList:
            if item[0] == "EventStreamInfo" :
                inputStreamName = item[1]
                pass
            pass
        pass
    except:
        print "WARNING Unable to determine input stream name from metadata_itemList."
        pass
    return inputStreamName



def GetCurrentSkimCycleAndStreamName( msg, athFile=None ):
    """ Helper to decide where to get the skimming cycle number and the input stream name from."""

    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        from RecExConfig.RecFlags import rec
        return GetCurrentSkimCycleFromInputFileSummary( inputFileSummary ), rec.mergingStreamName()
    except ImportError:
        msg.info("Couldn't get the inputFileSummary... trying AthFile directly.")
        pass

    # Import the reading of in-file metadata
    if athFile :
        return GetCurrentSkimCycleFromMetaDataItemList( athFile.fileinfos["metadata_items"] ), GetInputStreamNameFromMetaDataItemList( athFile.fileinfos["metadata_items"] )
    from PyUtils import AthFile
    af = AthFile.fopen( svcMgr.EventSelector.InputCollections[0] )
    return GetCurrentSkimCycleFromMetaDataItemList( af.fileinfos["metadata_items"] ), GetInputStreamNameFromMetaDataItemList( af.fileinfos["metadata_items"] )






def CreateCutFlowSvc( svcName="CutFlowSvc", athFile=None, seq=None, addMetaDataToAllOutputFiles=True ):
    """
    Helper to create the CutFlowSvc, extract the needed information from
    the input file, and also schedule all the needed stuff.
    """
    # Create a message logger
    from AthenaCommon.Logging import logging
    msg = logging.getLogger( "Create"+svcName )

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr,svcName):
        msg.warning("We already have a CutFlowSvc... won't set up the rest")
        return

    # Determine current skimming cycle and input stream name
    currentCycle, inputStreamName = GetCurrentSkimCycleAndStreamName( msg=msg, athFile=athFile )

    # Create the CutFlowSvc instance
    import AthenaCommon.CfgMgr as CfgMgr
    if not hasattr(svcMgr,"CutFlowSvc"): svcMgr += CfgMgr.CutFlowSvc()
    svcMgr.CutFlowSvc.SkimmingCycle = currentCycle
    svcMgr.CutFlowSvc.InputStream   = inputStreamName
    #svcMgr.CutFlowSvc.OutputLevel=DEBUG

    # Check if we have a sequence given
    if not seq :
        # Fetch the AthAlgSeq, i.e., one of the existing master sequences where one should attach all algorithms
        seq = CfgMgr.AthSequencer("AthAlgSeq")
        pass

    # First of all, schedule EventCounterAlg
    from EventBookkeeperTools.EventBookkeeperToolsConf import EventCounterAlg
    # seq.insert( 0, EventCounterAlg("AllExecutedEvents") )
    seq += EventCounterAlg("AllExecutedEvents")

    #Create a separate EventBookkeeper list to persistify skimming cycle info
    from EventBookkeeperTools.BookkeepingInfoWriter import CutCycleWriter
    seq += CutCycleWriter("CutCycleWriter",
                          OutputName   = "ProcessingCycle",
                          CurrentCycle = currentCycle)

    # If wanted, add the meta-data to all output files
    if addMetaDataToAllOutputFiles:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        #Explicitely add file metadata from input and from transient store
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#*" )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#*" )
        MSMgr.AddMetaDataItemToAllStreams( "EventBookkeeperCollection#*" )
        pass

    return
