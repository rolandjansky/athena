# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigConfMetadata.py 618376 2014-09-24 15:33:35Z tamartin $
#
# This module holds the code that should be used to add trigger configuration
# metadata to the D3PD.
#

def addTrigConfMetadata( d3pdalg = None, useTrigConfEventSummaries = False, doCostL2 = False, doCostEF = False, doCostHLT = False, saveKeys = True, tuplePath = "" ):

    """Helper function that adds the necessary tool(s) and service(s) to the
       job to save the trigger configuration metadata to the output D3PD
       file.

        Arguments:
          d3pdalg: The D3PD::MakerAlg that is creating the D3PD. If not specified,
                   the configuration is saved in a file called TrigConfig.root
          useTrigConfEventSummaries:	Set true when running in AtlasP1HLT or trigger BS
                   to gather detailed configuration from TrigConfEvent
                   summary objects (TrigCost running). 
    """

    # Create a logger for the function:
    if "logger" in dir(): orig_logger = logger
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "addTrigConfMetadata" )

    # Let the user know what we're doing:
    logger.info( "Adding trigger configuration metadata to the D3PD" )

    # The tool needs a special D3PDSvc in which the indexing is turned off
    # for the TTree-s:
    _d3pdSvcName = "TrigConfD3PDSvc"
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr( ServiceMgr, _d3pdSvcName ):
        from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
        ServiceMgr += D3PD__RootD3PDSvc( _d3pdSvcName )
        ServiceMgr.TrigConfD3PDSvc.MasterTree = ""
        ServiceMgr.TrigConfD3PDSvc.IndexMajor = ""
    else:
        logger.info( "The private D3PDSvc for the metadata TTree already exists" )
    _d3pdSvc = getattr( ServiceMgr, _d3pdSvcName )

    # If no D3PD::MakerAlg has been provided, create a dummy one:
    if d3pdalg == None:
        logger.warning( "No D3PD MakerAlg given to function!" )
        logger.warning( "The trigger configuration will be saved into file: " +
                        "\"TrigConfig.root\"" )
        from AthenaCommon.AlgSequence import AlgSequence
        theJob = AlgSequence()
        import D3PDMakerCoreComps
        d3pdalg = D3PDMakerCoreComps.MakerAlg( "trigConf", theJob, file = "TrigConfig.root",
                                               D3PDSvc = _d3pdSvc )

    # Add the metadata tool:
    _d3pdToolName = "TrigConfMetadataTool"
    if not _d3pdToolName in [ t.name() for t in d3pdalg.MetadataTools ]:
        import TriggerD3PDMaker
        if (tuplePath == ""):
          tuplePath = d3pdalg.TuplePath
        _trigConfTool = TriggerD3PDMaker.TrigConfMetadataTool( _d3pdToolName,
                                                               D3PDSvc = _d3pdSvc,
                                                               ConfigDir = tuplePath + "Meta" )
        _trigConfTool.UseTrigConfEventSummaries = useTrigConfEventSummaries
        if useTrigConfEventSummaries:
            # Figure out if old or new style HLT if using CostMon to get correct storegate key
            # Old key fomat was HLT_OPI_HLT_monitoring_config
            if (doCostL2 == True or doCostEF == True or doCostHLT == True):
              logger.info( "TrigConfMetadataTool will use passed arguments [L2="+str(doCostL2)+",EF="+str(doCostEF)+",HLT="+str(doCostHLT)+"]" )
              if (doCostL2 == True or doCostEF == True):
                _trigConfTool.keyConfig = "HLT_TrigMonConfigCollection_OPI_EF_monitoring_config"
              elif (doCostHLT == True):
                _trigConfTool.keyConfig = "HLT_TrigMonConfigCollection_OPI_HLT_monitoring_config"
            else: 
              logger.info( "TrigConfMetadataTool will use TriggerFlags flags for config" )
              from TriggerJobOpts.TriggerFlags import TriggerFlags
              if TriggerFlags.doHLT() and not (TriggerFlags.doEF() or TriggerFlags.doLVL2()):
                _trigConfTool.keyConfig = "HLT_TrigMonConfigCollection_OPI_HLT_monitoring_config"
              elif TriggerFlags.doEF() or TriggerFlags.doLVL2():
                _trigConfTool.keyConfig = "HLT_TrigMonConfigCollection_OPI_EF_monitoring_config"
            logger.info( "TrigConfMetadataTool will use the StoreGate key " + _trigConfTool.keyConfig )
            d3pdalg.MetadataTools += [ _trigConfTool ]
    else:
      logger.info( "TrigConfMetadataTool was already added to the D3PD::MakerAlg" )

    # Add the DB key filler object:
    if saveKeys == True:
      _dbKeysFillerName = "TrigDBKeysFiller"
      if not hasattr( d3pdalg, _dbKeysFillerName ):
          from TriggerD3PDMaker.TrigDBKeysD3PDObject import TrigDBKeysD3PDObject
          d3pdalg += TrigDBKeysD3PDObject( 0 )
      else:
          logger.info( "TrigDBKeysD3PDObject already added to the D3PD::MakerAlg" )

    # Restore the original logger if necessary:
    if "orig_logger" in dir(): logger = orig_logger

    return
