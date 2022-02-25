# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocCfg import AssocConfig, METAssocConfig,getMETAssocAlg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from METUtilities.METMakerConfig import getMETMakerAlg


def METAssociatorCfg(configFlags, jetType):
    sequencename = "METAssociation"

    components = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg   
    extrapCfg = AtlasExtrapolatorCfg(configFlags)
    extrapCfg.popPrivateTools()
    components.merge(extrapCfg)
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    AthSequencer=CompFactory.AthSequencer
    components.addSequence( AthSequencer(sequencename) )

    modConstKey = ""
    modClusColls = {}
    if configFlags.MET.UseTracks:
        modConstKey="OriginCorr"
        modClusColls={
            'LCOriginCorrClusters':'LCOriginTopoClusters',
            'EMOriginCorrClusters':'EMOriginTopoClusters'
            }

    if jetType == "AntiKt4LCTopo":
        associators = [AssocConfig('LCJet'),
                    AssocConfig('Muon'),
                    AssocConfig('Ele'),
                    AssocConfig('Gamma'),
                    AssocConfig('Tau'),
                    AssocConfig('Soft')]
        cfg_akt4lc = METAssocConfig('AntiKt4LCTopo',
                                    configFlags,
                                    associators,
                                    doPFlow=False,
                                    modConstKey=modConstKey,
                                    modClusColls=modClusColls
                                    )
        components_akt4lc= getAssocCA(cfg_akt4lc,sequencename='METAssoc_AntiKt4LCTopo',METName='AntiKt4LCTopo')
        components.merge(components_akt4lc)

    elif jetType == "AntiKt4EMTopo":
        associators = [AssocConfig('EMJet'),
                    AssocConfig('Muon'),
                    AssocConfig('Ele'),
                    AssocConfig('Gamma'),
                    AssocConfig('Tau'),
                    AssocConfig('Soft')]
        cfg_akt4em = METAssocConfig('AntiKt4EMTopo',
                                    configFlags,
                                    associators,
                                    doPFlow=False,
                                    modConstKey=modConstKey,
                                    modClusColls=modClusColls
                                    )
        components_akt4em= getAssocCA(cfg_akt4em,sequencename='METAssoc_AntiKt4EMTopo',METName='AntiKt4EMTopo')
        components.merge(components_akt4em)

    elif jetType == "AntiKt4EMPFlow":
        associators = [AssocConfig('PFlowJet'),
                       AssocConfig('Muon'),
                       AssocConfig('Ele'),
                       AssocConfig('Gamma'),
                       AssocConfig('Tau'),
                       AssocConfig('Soft')]
        cfg_akt4pf = METAssocConfig('AntiKt4EMPFlow',
                                    configFlags,
                                    associators,
                                    doPFlow=True,
                                    usePFOLinks=metFlags.UseFELinks()
                                    )
        components_akt4pf= getAssocCA(cfg_akt4pf,sequencename='METAssoc_AntiKt4EMPFlow',METName='AntiKt4EMPFlow')
        components.merge(components_akt4pf)
    else:
        raise RuntimeError("Jet type not recognized by METAssociatorCfg: {}".format(jetType) )
    return components
    
    
def getAssocCA(config,sequencename='METAssociation',METName=''):
    components = ComponentAccumulator()
    from AthenaConfiguration.ComponentFactory import CompFactory
    AthSequencer=CompFactory.AthSequencer
    components.addSequence( AthSequencer(sequencename) )
    assocAlg = getMETAssocAlg(algName='METAssociation_'+METName,configs={config.suffix:config})
    components.addEventAlgo(assocAlg,sequencename)
    if not METName=='':
        makerAlg=getMETMakerAlg(METName)
        components.addEventAlgo(makerAlg,sequencename)
    return components
