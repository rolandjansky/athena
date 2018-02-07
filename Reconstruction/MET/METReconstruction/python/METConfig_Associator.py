# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig

modClusKey = ""
modClusColl = {}
if metFlags.UseTracks():
                 modClusKey="OriginCorr",
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
                            modClusKey=modClusKey,
                            modClusColl=modClusColl
                            )

metFlags.METAssocConfigs()[cfg_akt4lc.suffix] = cfg_akt4lc
metFlags.METAssocOutputList().append(cfg_akt4lc.suffix)

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
                            modClusKey=modClusKey,
                            modClusColl=modClusColl,
                            )

metFlags.METAssocConfigs()[cfg_akt4em.suffix] = cfg_akt4em
metFlags.METAssocOutputList().append(cfg_akt4em.suffix)

############################################################################
# PFlow
if metFlags.DoPFlow() and metFlags.UseTracks():
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

    metFlags.METAssocConfigs()[cfg_akt4pf.suffix] = cfg_akt4pf
    metFlags.METAssocOutputList().append(cfg_akt4pf.suffix)
