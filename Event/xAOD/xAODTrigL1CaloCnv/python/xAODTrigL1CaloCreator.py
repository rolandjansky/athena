# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: xAODTrigL1CaloCreator.py 576327 2013-12-19 16:08:56Z morrisj $

# Import the package's configurables:
from xAODTrigL1CaloCnv.xAODTrigL1CaloCnvConf import *

## Helper function for creating xAODTrigL1Calo objects/containers
def xAODTrigL1CaloCreator( sequence = None, stream = None ):
    """
    Helper function that schedules all the components that create xAOD
    LVL1 RoI objects out of existing AOD objects.

    Arguments:
      sequence: Optional sequence to which the components should be added.
                If not specified, the main algorithm sequence is used.
      stream: Optional stream that the objects should be written to.
              If not specified, the objects are added to StreamXAOD.
    """

    # Create a logger for the function:
    if "logger" in dir(): orig_logger = logger
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODTrigL1CaloCreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD L1Calo objects from ESD L1Calo objects" )

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

    ### CMM Section    
    # Add the CMMCPHits converter algorithm:
    SGkey = "CMMCPHits"
    alg = xAODMaker__CMMCPHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CMMCPHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMCPHitsAuxContainer_v1#%sAux." % SGkey )
    
    # Add the CMMEtSums converter algorithm:
    SGkey = "CMMEtSums"
    alg = xAODMaker__CMMEtSumsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMEtSums objects to the output:
    stream.AddItem( "xAOD::CMMEtSumsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMEtSumsAuxContainer_v1#%sAux." % SGkey )   
    
    # Add the CMMJetHits converter algorithm:
    SGkey = "CMMJetHits"
    alg = xAODMaker__CMMJetHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMJetHits objects to the output:
    stream.AddItem( "xAOD::CMMJetHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMJetHitsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the CMMRoIs converter algorithm:
    SGkey = "CMMRoIs"
    alg = xAODMaker__CMMRoICnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMRoI objects to the output:
    stream.AddItem( "xAOD::CMMRoI_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMRoIAuxInfo_v1#%sAux." % SGkey )  
    
    
    ### CPM Section    
    # Add the CPMHits converter algorithm:
    SGkey = "CPMHits"
    alg = xAODMaker__CPMHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CPMHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CPMHitsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the CPMTower converter algorithm:
    SGkey = "CPMTowers"
    alg = xAODMaker__CPMTowerCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CPMTowerContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CPMTowerAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the CPMRoI converter algorithm:
    SGkey = "CPMRoIs"
    alg = xAODMaker__CPMRoICnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CPMRoIContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CPMRoIAuxContainer_v1#%sAux." % SGkey )    
    
    
    ### JEM Section    
    # Add the JEMHits converter algorithm:
    SGkey = "JEMHits"
    alg = xAODMaker__JEMHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JEMHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JEMHitsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the JEMEtSums converter algorithm:
    SGkey = "JEMEtSums"
    alg = xAODMaker__JEMEtSumsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JEMEtSumsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JEMEtSumsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the JEMRoI converter algorithm:
    SGkey = "JEMRoIs"
    alg = xAODMaker__JEMRoICnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JEMRoIContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JEMRoIAuxContainer_v1#%sAux." % SGkey )  
    
    
   
    
    # Add the JetElement converter algorithm:
    SGkey = "JetElements"
    alg = xAODMaker__JetElementCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JetElementContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JetElementAuxContainer_v1#%sAux." % SGkey )
    
    # Add the RODHeaders converter algorithm:
    SGkey = "RODHeaders"
    alg = xAODMaker__RODHeaderCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::RODHeaderContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::RODHeaderAuxContainer_v1#%sAux." % SGkey )     
    
    # Add the TriggerTower converter algorithm:
    SGkey = "TriggerTowers"
    alg = xAODMaker__TriggerTowerCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::TriggerTowerContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::TriggerTowerAuxContainer_v1#%sAux." % SGkey )  
    
    # Reinstate the old logger if it existed:
    if "orig_logger" in dir(): logger = orig_logger

    pass
