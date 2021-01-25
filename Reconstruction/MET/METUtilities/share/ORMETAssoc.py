from METReconstruction.METRecoFlags import jobproperties, metFlags
from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig, getMETAssocAlg
from METReconstruction.METRecoConfig import getMETRecoAlg

JetType = 'PFlowJet' if metFlags.DoORMet==False else 'ORPFlowJet'

#if metFlags.DoORMet==True : modConstKey = "OverlapRemovedCHSParticleFlowObjects" 
associators = [AssocConfig(JetType),
                   AssocConfig('Muon'),
                   AssocConfig('Ele'),
                   AssocConfig('Gamma'),
                   #AssocConfig('Tau'), # TODO, need to fix links in METTauAssociator
                   AssocConfig('Soft')]
cfg_akt4pf = METAssocConfig('AntiKt4EMPFlow' if metFlags.DoORMet==False else 'AntiKt4OverlapRemovedEMPFlow',
                                associators,
                                doPFlow=True,
                                usePFOLinks=False, 
                                useFELinks=False 
                            )

metFlags.METAssocConfigs()[cfg_akt4pf.suffix] = cfg_akt4pf
metFlags.METAssocOutputList().append(cfg_akt4pf.suffix)
metAlg = getMETAssocAlg('METAssociation' if metFlags.DoORMet==False else 'METAssociationOR',configs={cfg_akt4pf.suffix:cfg_akt4pf})
topSequence += metAlg



