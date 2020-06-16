# Defining a python module with helpers for Physics Validation

from AthenaCommon.Logging import logging
logger = logging.getLogger('PhysValUtils')

################################################################################################

def addPhysValAODContent(algseq,doJets,doTopoCluster):
    '''
    Schedule the addition of collections needed for validation of
    primary xAODs: AntiKt4TruthJets and LC/EMOriginTopoClusters
    '''

    logger.info( '****************** Adding content for AOD PhysVal *****************' )
    
    # Check some flags for steering
    from RecExConfig.AutoConfiguration import IsInInputFile
    requiresTruthJets  = IsInInputFile('xAOD::TruthParticleContainer','TruthParticles') and not IsInInputFile('xAOD::JetContainer','AntiKt4TruthJets')
    requiresLCOriginTC = not IsInInputFile('xAOD::CaloClusterContainer','LCOriginTopoClusters')
    requiresEMOriginTC = not IsInInputFile('xAOD::CaloClusterContainer','EMOriginTopoClusters')

    jettools_PhysVal = []
    # Truth jets
    if doJets and requiresTruthJets:
        jettools_PhysVal += addAntiKt4TruthJets(algseq)

    # Origin-corrected topoclusters
    if doTopoCluster and (requiresLCOriginTC or requiresEMOriginTC):
        jettools_PhysVal += addOriginCorrectedClusters(algseq,requiresLCOriginTC,requiresEMOriginTC)

    from AthenaCommon import CfgMgr
    # Only add the algorithm if there is a need for it
    if jettools_PhysVal:  
        from JetRec.JetRecStandard import jtm
        jtm += CfgMgr.JetToolRunner("jetrun_PhysVal",
                             EventShapeTools=[],
                             Tools=jettools_PhysVal,
                             Timer=0 # No timing information
                             )

        from JetRec.JetRecConf import JetAlgorithm
        algseq += CfgMgr.JetAlgorithm("jetalgPhysVal",Tools=[jtm.jetrun_PhysVal])

    logger.info( '******************              Done              *****************' )
    
################################################################################################

def addAntiKt4TruthJets(algseq):
    '''
    Use the standard AntiKt4TruthJets configuration helpers
    and return the jet finder
    '''
    logger.info( 'Configuring AntiKt4TruthJets' )
    
    truthtools = []
    from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
    truthtools += scheduleCopyTruthParticles()
    from JetRec.JetRecStandard import jtm
    truthtools.append( jtm.truthpartcopy )
    from AthenaCommon import CfgMgr
    algseq += CfgMgr.JetAlgorithm("jetalgTruthPartCopy",Tools=truthtools)

    for getter in jtm.gettersMap["truth"]:
        algseq += getter

    from JetRec.JetRecStandard import jtm
    return [ jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", ptmin= 5000) ]

################################################################################################

def addOriginCorrectedClusters(algseq,doLC,doEM):
    '''
    Create and return the origin-corrected cluster builders
    in the form of JetConsituentModSequence
    '''

    logger.info( 'Configuring origin-corrected cluster collections:' )
    ctools = []
    # Most likely we need both or none, but set this up generally
    from JetRec.JetRecStandard import jtm
    if doLC:
        logger.info( '    * LCOriginTopoClusters' )
        ctools += [jtm.JetConstitSeq_LCOrigin]
    if doEM:
        logger.info( '    * EMOriginTopoClusters' )
        ctools += [jtm.JetConstitSeq_EMOrigin]

    return ctools
