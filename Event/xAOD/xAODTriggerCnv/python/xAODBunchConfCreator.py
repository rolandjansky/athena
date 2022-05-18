# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Import the needed configurables:
from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__BunchConfCnvAlg
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool

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
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODBunchConfCreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD bunch configuration metadata" )

    # Get the main sequence if necessary:
    if sequence is None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary:
    if stream is None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        stream = MSMgr.GetStream( "StreamAOD" )
        pass

    # Schedule the converter algorithm:
    alg = xAODMaker__BunchConfCnvAlg()
    alg.BCConfProvider = BunchCrossingTool( source )
    sequence += alg

    # Add the created objects to the output:
    stream.AddItem( "xAOD::BunchConfKey_v1#BunchConfKey" )
    stream.AddMetaDataItem( "xAOD::BunchConfContainer_v1#BunchConfiguration" )
    stream.AddMetaDataItem(
        "xAOD::BunchConfAuxContainer_v1#BunchConfigurationAux." )

    pass
