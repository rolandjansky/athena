# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: xAODBunchConfCreator.py 583381 2014-02-14 15:58:20Z krasznaa $

# Import the needed configurables:
from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__BunchConfCnvAlg
from TrigBunchCrossingTool.BunchCrossingConfProvider import \
    BunchCrossingConfProvider

## Helper function for creating xAOD bunch configuration metadata
def xAODBunchConfCreator( sequence = None, stream = None, source = "" ):
    """
    Helper function that schedules all the components that create bunch
    configuration metadata for xAOD files.

    Arguments:
      sequence: Optional sequence to which the components should be added.
                If not specified, the main algorithm sequence is used.
      stream: Optional stream that the objects should be written to.
              If not specified, the objects are added to StreamAOD.
      source: Optional source specifying where the bunch configuration
              information should be taken from.
    """

    # Create a logger for the function:
    if "logger" in dir(): orig_logger = logger
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODBunchConfCreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD bunch configuration metadata" )

    # Get the main sequence if necessary:
    if sequence == None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary:
    if stream == None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        stream = MSMgr.GetStream( "StreamAOD" )
        pass

    # Schedule the converter algorithm:
    alg = xAODMaker__BunchConfCnvAlg()
    alg.BCConfProvider = BunchCrossingConfProvider( source )
    sequence += alg

    # Add the created objects to the output:
    stream.AddItem( "xAOD::BunchConfKey_v1#BunchConfKey" )
    stream.AddMetaDataItem( "xAOD::BunchConfContainer_v1#BunchConfiguration" )
    stream.AddMetaDataItem(
        "xAOD::BunchConfAuxContainer_v1#BunchConfigurationAux." )

    # Reinstate the old logger if it existed:
    if "orig_logger" in dir(): logger = orig_logger

    pass
