# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: xAODEventInfoCreator.py 634394 2014-12-08 11:10:34Z krasznaa $

# Import the configurable(s):
from xAODEventInfoCnv.xAODEventInfoCnvConf import *

## Helper function for creating xAOD::EventInfo
def xAODEventInfoCreator( sequence = None, stream = None, key = "" ):
    """
    Helper function that schedules the algorithm for creating an xAOD::EventInfo
    object out of the AOD's EventInfo one.

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
    logger = logging.getLogger( "xAODRoICreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD::EventInfo" )

    # Get the main sequence if necessary:
    if sequence == None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary:
    if stream == None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        stream = MSMgr.GetStream( "StreamXAOD" )
        pass

    # Add the converter algorithm:
    outkey = "EventInfo"
    alg = xAODMaker__EventInfoCnvAlg()
    alg.AODKey = key
    alg.xAODKey = outkey
    sequence += alg

    # Add the created objects to the output:
    stream.AddItem( "xAOD::EventInfo#%s" % outkey )
    stream.AddItem( "xAOD::EventAuxInfo#%sAux." % outkey )
    stream.AddItem( "xAOD::EventInfoContainer#PileUp%s" % outkey )
    stream.AddItem( "xAOD::EventInfoAuxContainer#PileUp%sAux." % outkey )

    return
