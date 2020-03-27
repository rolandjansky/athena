# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocCfg import AssocConfig, METAssocConfig,getMETAssocTool,getMETAssocAlg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon import CfgMgr
from METUtilities.METMakerConfig import getMETMakerAlg

#attempt at migrating to the new configuration system
def METAssociator_Cfg(configFlags):
    sequencename = "METAssociation"

    components = ComponentAccumulator()
    from AthenaCommon.AlgSequence import AthSequencer
    components.addSequence( AthSequencer(sequencename) )

    modConstKey = ""
    modClusColls = {}
    if configFlags.MET.UseTracks:
        modConstKey="OriginCorr"
        modClusColls={
            'LCOriginCorrClusters':'LCOriginTopoClusters',
            'EMOriginCorrClusters':'EMOriginTopoClusters'
            }

    ############################################################################
    # AntiKt4LCTopo
    JetType = 'LCJet'

    associators = [AssocConfig(JetType),
                   AssocConfig('Muon'),
                   AssocConfig('Ele'),
                   AssocConfig('Gamma'),
                   AssocConfig('Tau'),
                   AssocConfig('Soft')]
    cfg_akt4lc = METAssocConfig('AntiKt4LCTopo',
                                associators,
                                doPFlow=False,
                                modConstKey=modConstKey,
                                modClusColls=modClusColls
                                )

    assoctool_akt4lc = getMETAssocTool(cfg_akt4lc)
    assocAlg_akt4lc = getMETAssocAlg(algName='METAssociation_LCJets',tools=[assoctool_akt4lc])
    components.addEventAlgo(assocAlg_akt4lc,sequencename)
    makerAlg_akt4lc= getMETMakerAlg('AntiKt4LCTopo')
    components.addEventAlgo(makerAlg_akt4lc,'METAssociation')
    
    #metFlags.METAssocConfigs()[cfg_akt4lc.suffix] = cfg_akt4lc
    #metFlags.METAssocOutputList().append(cfg_akt4lc.suffix)

    ############################################################################
    # AntiKt4EMTopo
    JetType = 'EMJet'
    
    associators = [AssocConfig(JetType),
                   AssocConfig('Muon'),
                   AssocConfig('Ele'),
                   AssocConfig('Gamma'),
                   AssocConfig('Tau'),
                   AssocConfig('Soft')]
    cfg_akt4em = METAssocConfig('AntiKt4EMTopo',
                                associators,
                                doPFlow=False,
                                modConstKey=modConstKey,
                                modClusColls=modClusColls
                                )
    assoctool_akt4em = getMETAssocTool(cfg_akt4em)
    assocAlg_akt4em = getMETAssocAlg(algName='METAssociation_EMJets',tools=[assoctool_akt4em])
    components.addEventAlgo(assocAlg_akt4em,sequencename)
    makerAlg_akt4em= getMETMakerAlg('AntiKt4EMTopo')
    components.addEventAlgo(makerAlg_akt4em,'METAssociation')   
    #metFlags.METAssocConfigs()[cfg_akt4em.suffix] = cfg_akt4em
    #metFlags.METAssocOutputList().append(cfg_akt4em.suffix)

    ############################################################################
    # PFlow
    if configFlags.MET.DoPFlow and configFlags.MET.UseTracks:
        JetType = 'PFlowJet'
    
        associators = [AssocConfig(JetType),
                       AssocConfig('Muon'),
                       AssocConfig('Ele'),
                       AssocConfig('Gamma'),
                       AssocConfig('Tau'),
                       AssocConfig('Soft')]
        cfg_akt4pf = METAssocConfig('AntiKt4EMPFlow',
                                    associators,
                                    doPFlow=True
                                    )
	assoctool_akt4pf = getMETAssocTool(cfg_akt4pf)
        #metFlags.METAssocConfigs()[cfg_akt4pf.suffix] = cfg_akt4pf
        #metFlags.METAssocOutputList().append(cfg_akt4pf.suffix)
    	assocAlg_akt4pf = getMETAssocAlg(algName='METAssociation_PFlowJets',tools=[assoctool_akt4pf])
    	components.addEventAlgo(assocAlg_akt4em,sequencename)
        makerAlg_akt4pf= getMETMakerAlg('AntiKt4EMPFlow')
        components.addEventAlgo(makerAlg_akt4pf,'METAssociation')   
    return components
