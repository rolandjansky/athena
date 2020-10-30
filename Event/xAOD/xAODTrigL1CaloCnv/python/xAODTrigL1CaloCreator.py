# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Import the package's configurables:
import xAODTrigL1CaloCnv.xAODTrigL1CaloCnvConf as conf

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
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "xAODTrigL1CaloCreator" )

    # Tell the user what's happening:
    logger.info( "Creating xAOD L1Calo objects from ESD L1Calo objects" )

    # Get the main sequence if necessary:
    if sequence is None:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()
        pass

    # Access the stream if necessary:
    if stream is None:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        stream = MSMgr.GetStream( "StreamXAOD" )
        pass

    ### CMM Section    
    # Add the CMMCPHits converter algorithm:
    SGkey = "CMMCPHits"
    alg = conf.xAODMaker__CMMCPHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CMMCPHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMCPHitsAuxContainer_v1#%sAux." % SGkey )
    
    # Add the CMMEtSums converter algorithm:
    SGkey = "CMMEtSums"
    alg = conf.xAODMaker__CMMEtSumsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMEtSums objects to the output:
    stream.AddItem( "xAOD::CMMEtSumsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMEtSumsAuxContainer_v1#%sAux." % SGkey )   
    
    # Add the CMMJetHits converter algorithm:
    SGkey = "CMMJetHits"
    alg = conf.xAODMaker__CMMJetHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMJetHits objects to the output:
    stream.AddItem( "xAOD::CMMJetHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMJetHitsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the CMMRoIs converter algorithm:
    SGkey = "CMMRoIs"
    alg = conf.xAODMaker__CMMRoICnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMRoI objects to the output:
    stream.AddItem( "xAOD::CMMRoI_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CMMRoIAuxInfo_v1#%sAux." % SGkey )  
    
    
    ### CPM Section    
    # Add the CPMHits converter algorithm:
    SGkey = "CPMHits"
    alg = conf.xAODMaker__CPMHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CPMHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CPMHitsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the CPMTower converter algorithm:
    SGkey = "CPMTowers"
    alg = conf.xAODMaker__CPMTowerCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CPMTowerContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CPMTowerAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the CPMRoI converter algorithm:
    SGkey = "CPMRoIs"
    alg = conf.xAODMaker__CPMRoICnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::CPMRoIContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::CPMRoIAuxContainer_v1#%sAux." % SGkey )    
    
    
    ### JEM Section    
    # Add the JEMHits converter algorithm:
    SGkey = "JEMHits"
    alg = conf.xAODMaker__JEMHitsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JEMHitsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JEMHitsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the JEMEtSums converter algorithm:
    SGkey = "JEMEtSums"
    alg = conf.xAODMaker__JEMEtSumsCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JEMEtSumsContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JEMEtSumsAuxContainer_v1#%sAux." % SGkey ) 
    
    # Add the JEMRoI converter algorithm:
    SGkey = "JEMRoIs"
    alg = conf.xAODMaker__JEMRoICnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JEMRoIContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JEMRoIAuxContainer_v1#%sAux." % SGkey )  
    
    
   
    
    # Add the JetElement converter algorithm:
    SGkey = "JetElements"
    alg = conf.xAODMaker__JetElementCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::JetElementContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::JetElementAuxContainer_v1#%sAux." % SGkey )
    
    # Add the RODHeaders converter algorithm:
    SGkey = "RODHeaders"
    alg = conf.xAODMaker__RODHeaderCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::RODHeaderContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::RODHeaderAuxContainer_v1#%sAux." % SGkey )     
    
    # Add the TriggerTower converter algorithm:
    SGkey = "TriggerTowers"
    alg = conf.xAODMaker__TriggerTowerCnvAlg()
    alg.ESDKey = SGkey
    alg.xAODKey = SGkey
    sequence += alg
    # Add the CMMCPHits objects to the output:
    stream.AddItem( "xAOD::TriggerTowerContainer_v1#%s" % SGkey )
    stream.AddItem( "xAOD::TriggerTowerAuxContainer_v1#%sAux." % SGkey )  
    
    pass
