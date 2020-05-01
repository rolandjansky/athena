# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: Karsten Koeneke
def GetCurrentStreamName( msg ):
    """ Helper to decide where to get the input stream name from."""
    # First, try to get the info from the RecFlags
    try:
        from RecExConfig.RecFlags import rec
        msg.debug("Got the stream name from the RecFlags: %s" % rec.mergingStreamName())
        streamName = rec.mergingStreamName()
        if streamName == "":
            streamName = "unknownStream"
        return streamName
    except ImportError:
        msg.info("Couldn't get input stream name from the RecFlags... trying AthFile directly.")

    from PyUtils.MetaReader import read_metadata
    from AthenaCommon.AppMgr  import ServiceMgr as svcMgr
    input_file = svcMgr.EventSelector.InputCollections[0]
    metadata = read_metadata(input_file)
    metadata = metadata[input_file]  # promote all keys one level up

    for class_name, name in metadata['metadata_items'].items():
        if name == 'EventStreamInfo':
            return class_name
    return 'unknownStream'

def CreateCutFlowSvc( svcName="CutFlowSvc", seq=None, addMetaDataToAllOutputFiles=True ):
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
    inputStreamName = GetCurrentStreamName( msg=msg )
    msg.debug("CreateCutFlowSvc: Have inputStreamName = %s" % (inputStreamName) )

    # Create the CutFlowSvc instance
    import AthenaCommon.CfgMgr as CfgMgr
    if not hasattr(svcMgr,"CutFlowSvc"): svcMgr += CfgMgr.CutFlowSvc()
    svcMgr.CutFlowSvc.InputStream   = inputStreamName

    # Make sure MetaDataSvc is ready
    if not hasattr(svcMgr,'MetaDataSvc'):
      from AthenaServices.AthenaServicesConf import MetaDataSvc
      svcMgr += MetaDataSvc( "MetaDataSvc" )

    # Add BookkeeperTools
    from EventBookkeeperTools.EventBookkeeperToolsConf import BookkeeperTool

    # Standard event bookkeepers
    primary_name = "CutBookkeepers"
    cutflowtool = BookkeeperTool(primary_name + "Tool",
                                 InputCollName = primary_name,
                                 OutputCollName= primary_name)
    svcMgr.ToolSvc += cutflowtool

    # Add tool to MetaDataSvc
    svcMgr.MetaDataSvc.MetaDataTools += [cutflowtool]

    # Check if we have a sequence given
    if not seq :
        # Fetch the AthAlgSeq, i.e., one of the existing master sequences where one should attach all algorithms
        seq = CfgMgr.AthSequencer("AthAlgSeq")
        pass

    # First of all, schedule EventCounterAlg
    if not hasattr(seq,"AllExecutedEvents"):
        if not seq.isLocked():
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
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#"+primary_name )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#"+primary_name+"Aux.*" )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#Incomplete"+primary_name )
        MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#Incomplete"+primary_name+"Aux.*" )
        pass

    return

def CreateBookkeeperTool( name="CutBookkeepers" ):

  from AthenaCommon.AppMgr  import ServiceMgr as svcMgr

  # Make sure MetaDataSvc is ready
  if not hasattr(svcMgr,'MetaDataSvc'):
    from AthenaServices.AthenaServicesConf import MetaDataSvc
    svcMgr += MetaDataSvc( "MetaDataSvc" )

  # Add BookkeeperTools
  from EventBookkeeperTools.EventBookkeeperToolsConf import BookkeeperTool

  # Standard event bookkeepers
  cutflowtool = BookkeeperTool(name,
                               InputCollName=name,
                               OutputCollName = name)
  svcMgr.ToolSvc += cutflowtool

  # Add tool to MetaDataSvc
  #svcMgr.MetaDataSvc.MetaDataTools += [cutflowtool]

  return

def CreateBookkeeperDumperTool(name='BookkeeperDumperTool'):
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    # Make sure MetaDataSvc is ready
    if not hasattr(svcMgr, 'MetaDataSvc'):
        from AthenaServices.AthenaServicesConf import MetaDataSvc
        svcMgr += MetaDataSvc('MetaDataSvc')

    # Add BookkeeperDumperTool
    from EventBookkeeperTools.EventBookkeeperToolsConf import BookkeeperDumperTool
    tool = BookkeeperDumperTool(name)
    svcMgr.ToolSvc += tool

    # Add tool to MetaDataSvc
    svcMgr.MetaDataSvc.MetaDataTools += [tool]
