# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: xAODRoICreator.py 578784 2014-01-16 16:56:39Z krasznaa $

# Import the package's configurables:
from xAODTriggerCnv.xAODTriggerCnvConf import *

## Helper function for creating xAOD RoI objects/containers
def xAODRoICreator( sequence = None, stream = None, key = "LVL1_ROI" ):
    """
    Helper function that schedules all the components that create xAOD
    LVL1 RoI objects out of existing AOD objects.

    Arguments:
      sequence: Optional sequence to which the components should be added.
                If not specified, the main algorithm sequence is used.
      stream: Optional stream that the objects should be written to.
              If not specified, the objects are added to StreamAOD.
      key: Optional StoreGate key for the AOD object that is supposed to be
           translated.
    """

    # Create a logger for the function:
    if "logger" in dir(): orig_logger = logger
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODRoICreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD LVL1 RoIs from AOD objects" )

    # Get the main sequence if necessary:
    if sequence == None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary (and possible):
    if stream == None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        try:
            stream = MSMgr.GetStream( "StreamAOD" )
        except NameError:
            stream = None
            pass
        pass

    # Add the muon RoI converter algorithm:
    outkey = "LVL1MuonRoIs"
    alg = xAODMaker__MuonRoICnvAlg()
    alg.AODKey = key
    alg.xAODKey = outkey
    sequence += alg
    # Add the muon RoI objects to the output:
    if stream != None:
        stream.AddItem( "xAOD::MuonRoIContainer_v1#%s" % outkey )
        stream.AddItem( "xAOD::MuonRoIAuxContainer_v1#%sAux." % outkey )
        pass

    # Add the jet RoI converter algorithm:
    outkey = "LVL1JetRoIs"
    alg = xAODMaker__JetRoICnvAlg()
    alg.AODKey = key
    alg.xAODKey = outkey
    sequence += alg
    # Add the jet RoI objects to the output:
    if stream != None:
        stream.AddItem( "xAOD::JetRoIContainer_v1#%s" % outkey )
        stream.AddItem( "xAOD::JetRoIAuxContainer_v1#%sAux." % outkey )
        pass

    # Add the em/tau RoI converter algorithm:
    outkey = "LVL1EmTauRoIs"
    alg = xAODMaker__EmTauRoICnvAlg()
    alg.AODKey = key
    alg.xAODKey = outkey
    sequence += alg
    # Add the em/tau RoI objects to the output:
    if stream != None:
        stream.AddItem( "xAOD::EmTauRoIContainer_v1#%s" % outkey )
        stream.AddItem( "xAOD::EmTauRoIAuxContainer_v1#%sAux." % outkey )
        pass

    # Add the energy-sum RoI converter algorithm:
    outkey = "LVL1EnergySumRoI"
    alg = xAODMaker__EnergySumRoICnvAlg()
    alg.AODKey = key
    alg.xAODKey = outkey
    sequence += alg
    # Add the energy-sum RoI objects to the output:
    if stream != None:
        stream.AddItem( "xAOD::EnergySumRoI_v1#%s" % outkey )
        stream.AddItem( "xAOD::EnergySumRoIAuxInfo_v1#%sAux." % outkey )
        pass

    # Add the jet-Et RoI converter algorithm:
    outkey = "LVL1JetEtRoI"
    alg = xAODMaker__JetEtRoICnvAlg()
    alg.AODKey = key
    alg.xAODKey = outkey
    sequence += alg
    # Add the jet-Et RoI objects to the output:
    if stream != None:
        stream.AddItem( "xAOD::JetEtRoI_v1#%s" % outkey )
        stream.AddItem( "xAOD::JetEtRoIAuxInfo_v1#%sAux." % outkey )
        pass

    # Reinstate the old logger if it existed:
    if "orig_logger" in dir(): logger = orig_logger

    pass
