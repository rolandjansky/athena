#This is a postInclude file to be used to run the BLS stream in production.                                                                                                          
#Author: James Walder   
removed_items = []

items_to_remove = ['SlowMuonContainer','SlowMuonAuxContainer',
                'PFOContainer', 'PFOAuxContainer','ParticleFlow',
                'JetContainer', 'JetAuxContainer','PFlow',
                'BTaggingContainer','BTaggingAuxContainer',
                'BTagVertexContainer','BTagVertexAuxContainer','BTagging_',
                'JetMomentMap',
                'EventShape','EventShapeAuxInfo',
                'ForwardElectrons','ForwardElectronsAux',
                'DiTauJets',
                'InDetForwardTrackParticles','CombinedStauTrackParticles',
                'xAOD::MuonContainer#Staus','xAOD::MuonAuxContainer#StausAux',
                ]



for item in CILMergeAOD.list():
    for toremove in items_to_remove:
        if toremove in item:
            removed_items.append(item)

CILMergeAOD.removeItem(removed_items)

StreamAOD.ItemList=CILMergeAOD()
print(CILMergeAOD.list())



