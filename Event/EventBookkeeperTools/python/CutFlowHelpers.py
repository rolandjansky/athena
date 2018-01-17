# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: Karsten Koeneke

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
    if inputStreamName == "" : inputStreamName = "unknownStream"
    return inputStreamName




def GetCurrentStreamName( msg, athFile=None ):
    """ Helper to decide where to get the input stream name from."""
    # First, try to get the info from the RecFlags
    try:
        from RecExConfig.RecFlags import rec
        msg.debug("Got the stream name from the RecFlags: %s" % rec.mergingStreamName())
        streamName = rec.mergingStreamName()
        if streamName == "" : streamName = "unknownStream"
        return streamName
    except ImportError:
        msg.info("Couldn't get input stream name from the RecFlags... trying AthFile directly.")
        pass
    # Import the reading of in-file metadata
    if athFile :
        return GetInputStreamNameFromMetaDataItemList( athFile.fileinfos["metadata_items"] )
    from PyUtils import AthFile
    af = AthFile.fopen( svcMgr.EventSelector.InputCollections[0] )
    return GetInputStreamNameFromMetaDataItemList( af.fileinfos["metadata_items"] )




def CreateCutFlowSvc( svcName="CutFlowSvc", athFile=None, seq=None, addAlgInPlace=False, addMetaDataToAllOutputFiles=True, SGkey="CutBookkeepers" ):
    """
    Helper to create the CutFlowSvc, extract the needed information from
    the input file, and also schedule all the needed stuff.
    """
    # Create a message logger
    from AthenaCommon.Logging import logging
    msg = logging.getLogger( "Create"+svcName )

    # Get the service manager
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    # Determine current input stream name
    inputStreamName = GetCurrentStreamName( msg=msg, athFile=athFile )
    msg.debug("CreateCutFlowSvc: Have inputStreamName = %s" % (inputStreamName) )

    # Create the CutFlowSvc instance(s)
    import AthenaCommon.CfgMgr as CfgMgr
    if not hasattr(svcMgr,"CutFlowSvc"): svcMgr += CfgMgr.CutFlowSvc()
    svcMgr.CutFlowSvc.InputStream   = inputStreamName
    #if not hasattr(svcMgr,"FileCutFlowSvc"): svcMgr += CfgMgr.FileCutFlowSvc()
    #svcMgr.FileCutFlowSvc.InputStream   = inputStreamName

    # Make sure MetaDataSvc is ready
    if not hasattr(svcMgr,'MetaDataSvc'):
      from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
      svcMgr += MetaDataSvc( "MetaDataSvc" )

    # Add BookkeeperTools
    from EventBookkeeperTools.EventBookkeeperToolsConf import BookkeeperTool

    # Standard event bookkeepers
    inname = "CutBookkeepers"
    outname = "CutBookkeepers"
    cutflowtool = BookkeeperTool(outname,
                                 InputCollName = inname,
                                 OutputCollName= outname) 
    svcMgr.ToolSvc += cutflowtool

    # Add tool to MetaDataSvc
    svcMgr.MetaDataSvc.MetaDataTools += [cutflowtool]

    # Add pdf sum of weights counts if appropriate
    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.DataSource() == 'geant4':
        #from PyUtils import AthFile
        #afc = AthFile.fopen( svcMgr.EventSelector.InputCollections[0] )

        # PDF
        name = "PDFSumOfWeights"
        pdfweighttool = BookkeeperTool(name,
                                       OutputCollName= name, 
                                       InputCollName = name)
        svcMgr.ToolSvc += pdfweighttool

        # Add tool to MetaDataSvc
        svcMgr.MetaDataSvc.MetaDataTools += [pdfweighttool]

    # Check if we have a sequence given
    if not seq :
        # Fetch the AthAlgSeq, i.e., one of the existing master sequences where one should attach all algorithms
        seq = CfgMgr.AthSequencer("AthAlgSeq")
        pass

    # First of all, schedule EventCounterAlg
    if not hasattr(seq,"AllExecutedEvents"):
        if not seq.isLocked():
            if addAlgInPlace:
                msg.debug("Adding EventCounterAlg with name AllExecutedEvents to sequence with name %s" % seq.getName())
                seq += CfgMgr.EventCounterAlg("AllExecutedEvents")
                pass
            else:
                # Need to schedule it after the xAODMaker::EventInfoCnvAlg such that xAOD::EventInfo is present
                index = 0
                if hasattr( seq, "xAODMaker::EventInfoCnvAlg" ):
                    for alg in seq:
                        index += 1
                        if alg.getName() == "xAODMaker::EventInfoCnvAlg": break
                        pass
                    pass
                msg.debug("Adding EventCounterAlg with name AllExecutedEvents to sequence with name %s at position %i" % (seq.getName(),index))
                seq.insert( index, CfgMgr.EventCounterAlg("AllExecutedEvents") )
                pass
            pass
        else :
            msg.info("Could NOT add EventCounterAlg with name AllExecutedEvents to locked sequence with name %s" % seq.getName())
            pass
        pass

    # If wanted, add the meta-data to all output files
    if addMetaDataToAllOutputFiles:
        msg.debug("Adding CutBookkeepers the the output meta data of all output streams")
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        # Explicitely add file metadata from input and from transient store,
        # but only the ones that we always create.
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#"+SGkey )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#"+SGkey+"Aux.*" )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#Incomplete"+SGkey )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#Incomplete"+SGkey+"Aux.*" )
        SGkey = "FileBookkeepers"
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#"+SGkey )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#"+SGkey+"Aux.*" )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#Incomplete"+SGkey )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#Incomplete"+SGkey+"Aux.*" )
        pass

    return

def CreateBookkeeperTool( name="CutBookkeepers" ):

  from AthenaCommon.AppMgr  import ServiceMgr as svcMgr

  # Make sure MetaDataSvc is ready
  if not hasattr(svcMgr,'MetaDataSvc'):
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
    svcMgr += MetaDataSvc( "MetaDataSvc" )

  # Add BookkeeperTools
  from EventBookkeeperTools.EventBookkeeperToolsConf import BookkeeperTool

  # Standard event bookkeepers
  cutflowtool = BookkeeperTool(name,
                               InputCollName=name,
                               OutputCollName = name)
  svcMgr.ToolSvc += cutflowtool

  # Add tool to MetaDataSvc
  svcMgr.MetaDataSvc.MetaDataTools += [cutflowtool]

  return
