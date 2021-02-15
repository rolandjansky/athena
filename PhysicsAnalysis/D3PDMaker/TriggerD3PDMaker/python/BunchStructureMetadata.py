# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @package BunchStructureMetadata
#
# Package holding the code to help users easily add the bunch structure metadata
# to their D3PDs.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#

##
# @short Function adding the bunch structure metadata to the D3PD
#
# This function works pretty much the same way as addTrigConfMetadata(...).
# You need to give it the D3PD::Maker algorithm as a parameter, and it can
# figure out everything else from there. There's also the possibility to force
# it to take the bunch structure configuration from a specific source, for which
# you have to follow the naming rules set by
# <code>TrigBunchCrossingTool.BunchCrossingConfProvider.BunchCrossingConfProvider</code>
#
# @param d3pdalg The D3PD::Maker algorithm saving your event-level data
# @param source Optional/specific source of the configuration information
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
def addBunchStructureMetadata( d3pdalg = None, source = "" ):

    # Create a logger for the function:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "addBunchStructureMetadata" )

    # Let the user know what we're doing:
    logger.info( "Adding bunch configuration metadata to the D3PD" )

    # The tool needs a special D3PDSvc in which the indexing is turned off
    # for the TTree-s:
    _d3pdSvcName = "BCD3PDSvc"
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr( ServiceMgr, _d3pdSvcName ):
        from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
        ServiceMgr += D3PD__RootD3PDSvc( _d3pdSvcName )
        _d3pdSvc = getattr( ServiceMgr, _d3pdSvcName )
        _d3pdSvc.MasterTree = ""
        _d3pdSvc.IndexMajor = ""
    else:
        logger.info( "The private D3PDSvc for the metadata TTree already exists" )
    _d3pdSvc = getattr( ServiceMgr, _d3pdSvcName )

    # If no D3PD::MakerAlg has been provided, create a dummy one:
    if d3pdalg is None:
        logger.warning( "No D3PD MakerAlg given to function!" )
        logger.warning( "The bunch configuration will be saved into file: "
                        "\"BunchConfig.root\"" )
        from AthenaCommon.AlgSequence import AlgSequence
        theJob = AlgSequence()
        import D3PDMakerCoreComps
        d3pdalg = D3PDMakerCoreComps.MakerAlg( "bunchConf", theJob,
                                               file = "BunchConfig.root",
                                               D3PDSvc = _d3pdSvc )

    # Add the metadata tool:
    _d3pdToolName = "BunchStructureMetadataTool"
    if _d3pdToolName not in [ t.name() for t in d3pdalg.MetadataTools ]:
        import TriggerD3PDMaker
        from TrigBunchCrossingTool.BunchCrossingConfProvider import BunchCrossingConfProvider
        d3pdalg.MetadataTools += [
            TriggerD3PDMaker.BunchStructureMetadataTool( _d3pdToolName,
                                                         D3PDSvc = _d3pdSvc,
                                                         ConfigDir = d3pdalg.TuplePath + \
                                                                     "Meta",
                                                         BCConfProvider = \
                                                         BunchCrossingConfProvider( source ) )
            ]
    else:
        logger.info( "BunchStructureMetadataTool was already added to the D3PD::MakerAlg" )

    # Add the DB key filler object:
    _configIdFillerName = "BunchConfigIDFiller"
    if not hasattr( d3pdalg, _configIdFillerName ):
        from TriggerD3PDMaker.BunchConfigIDD3PDObject import BunchConfigIDD3PDObject
        d3pdalg += BunchConfigIDD3PDObject( source )( 0 )
    else:
        logger.info( "BunchConfigIDD3PDObject already added to the D3PD::MakerAlg" )

    return
