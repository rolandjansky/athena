# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Import the package's configurables:
from xAODTauCnv.xAODTauCnvConf import xAODMaker__TauJetCnvAlg

## Helper function for creating xAOD RoI objects/containers
def xAODTauConverter( sequence = None, stream = None, key = "TauRecContainer" ):
    """
    Helper function that schedules all the components that create xAOD
    tau objects out of existing AOD objects.

    Arguments:
      sequence: Optional sequence to which the components should be added.
                If not specified, the main algorithm sequence is used.
      stream: Optional stream that the objects should be written to.
              If not specified, the objects are added to StreamXAOD.
      key: Optional StoreGate key for the AOD object that is supposed to be
           translated.
    """

    # Create a logger for the function:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODTauConverter" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD TauJets from AOD objects" )

    # Get the main sequence if necessary:
    if sequence is None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary:
    if stream is None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        try:
            stream = MSMgr.GetStream( "StreamAOD" )
        except NameError:
            stream = None
            pass
        pass

    # Add the tau converter algorithm:
    outkey = "TauRecContainer"
    tauAlg = xAODMaker__TauJetCnvAlg()
    #tauAlg.OutputLevel = VERBOSE
    tauAlg.OutputLevel = 1
    sequence += tauAlg
    # Add the tau objects to the output:
    if stream is not None:
        stream.AddItem( "xAOD::TauJetContainer_v1#%s" % outkey )
        stream.AddItem( "xAOD::TauJetAuxContainer_v1#%sAux." % outkey )

    pass
