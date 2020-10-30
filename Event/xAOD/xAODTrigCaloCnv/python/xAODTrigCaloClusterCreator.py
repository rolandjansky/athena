# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Import the package's configurables:
from xAODTrigCaloCnv.xAODTrigCaloCnvConf import xAODMaker__TrigCaloClusterCnvAlg

## Helper function for creating xAOD TrigCaloCluster objects
def xAODTrigCaloClusterCreator( sequence = None, stream = None,
                                key = "HLT_TrigT2Calo" ):
    """
    Helper function that schedules all the components that create xAOD
    TrigCaloCluster objects out of existing AOD objects.

    Arguments:
      sequence: Optional sequence to which the components should be added.
                If not specified, the main algorithm sequence is used.
      stream: Optional stream that the objects should be written to.
              If not specified, the objects are added to StreamAOD.
      key: Optional StoreGate key for the AOD object that is supposed to be
           translated.
    """

    # Create a logger for the function:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODTrigCaloClusterCreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD::TrigCaloClusterContainer from TrigCaloClusterContainer" )

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
    alg = xAODMaker__TrigCaloClusterCnvAlg()
    alg.AODKey = key
    alg.xAODKey = key
    sequence += alg

    # Add the created objects to the output:
    stream.AddItem( "xAOD::TrigCaloClusterContainer_v1#%s" % key )
    stream.AddItem( "xAOD::TrigCaloClusterAuxContainer_v1#%sAux." % key )

    pass
