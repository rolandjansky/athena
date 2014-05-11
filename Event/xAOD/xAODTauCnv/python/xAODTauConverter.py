# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: xAODTauConverter.py 579184 2014-01-20 13:25:06Z janus $

# Import the package's configurables:
from xAODTauCnv.xAODTauCnvConf import *

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
    if "logger" in dir(): orig_logger = logger
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODTauConverter" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD TauJets from AOD objects" )

    # Get the main sequence if necessary:
    if sequence == None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary:
    if stream == None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        try:
            stream = MSMgr.GetStream( "StreamAOD" )
        except NameError:
            stream = None
            pass
        pass

    # Add the tau converter algorithm:
    outkey = "TauRecContainer"
    from xAODTauCnv.xAODTauCnvConf import xAODMaker__TauJetCnvAlg
    tauAlg = xAODMaker__TauJetCnvAlg()
    #tauAlg.OutputLevel = VERBOSE
    tauAlg.OutputLevel = 1
    sequence += tauAlg
    # Add the tau objects to the output:
    if stream != None:
        stream.AddItem( "xAOD::TauJetContainer_v1#%s" % outkey )
        stream.AddItem( "xAOD::TauJetAuxContainer_v1#%sAux." % outkey )

    # Reinstate the old logger if it existed:
    if "orig_logger" in dir(): logger = orig_logger

    pass
