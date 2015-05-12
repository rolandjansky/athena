#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
class MuonsDxAODStreamConfigurer:
    commonItems = []
    MUON0OnlyItems = ["CosmicMuonCollection#HLT_CosmicMuons", "MuonCaloEnergyContainer#MuonCaloEnergyCollection"]
    MUON1OnlyItems = ['CaloClusterCellLinkContainer#MuonClusterCollection_links', 'CaloCellContainer#AODCellContainer']
    MUON2OnlyItems = []
    MUON3OnlyItems = []
    MUON0OnlyItems += ["xAOD::MuonRoIContainer#LVL1MuonRoIs"]
    MUON0OnlyItems += ["xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux."]
    MUON0OnlyItems += ['xAOD::L2StandAloneMuonContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo',
'xAOD::L2CombinedMuonContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo',
'xAOD::L2IsoMuonContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfo',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuTagIMO_EF',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_eMuonEFInfo',
'xAOD::MuonRoIContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuon',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticles',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticles',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticles',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticles',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFID',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTF',
]
    MUON0OnlyItems += ['xAOD::L2StandAloneMuonAuxContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfoAux.',
'xAOD::L2CombinedMuonAuxContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfoAux.',
'xAOD::L2IsoMuonAuxContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfoAux.',
'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux.',
'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuTagIMO_EFAux.',
'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_eMuonEFInfoAux.',
'xAOD::MuonRoIAuxContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuonAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticlesAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticlesAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticlesAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTFAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFIDAux.',
'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTFAux.',
]
#     triggerMatchingItems = [
#             "xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo",
#             "xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux.pt.eta.phi.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.extrapolatedMuonSpectrometerTrackParticleLink.combinedTrackParticleLink.clusterLink",
#             "xAOD::MuonRoIContainer#LVL1MuonRoIs",
#             "xAOD::MuonRoIAuxContainer#VL1MuonRoIsAux.eta.phi.thrName"]

    triggerMatchingItems_basic = ['xAOD::TrigNavigation#*','xAOD::TrigNavigationAuxInfo#*']
    MUON0OnlyItems += triggerMatchingItems_basic

    triggerMatchingItems = triggerMatchingItems_basic + [
            "xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo",
            "xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux.",
            "xAOD::MuonRoIContainer#LVL1MuonRoIs",
            "xAOD::MuonRoIAuxContainer#VL1MuonRoIsAux."]
    MUON1OnlyItems += triggerMatchingItems
    MUON2OnlyItems += triggerMatchingItems
#     MUON3OnlyItems += triggerMatchingItems

    Items = {'MUON0':MUON0OnlyItems, 'MUON1':MUON1OnlyItems, 'MUON2':MUON2OnlyItems, 'MUON3':MUON3OnlyItems}

    ### samrt slimming containers
    smSlContainer = {'MUON0':[], 'MUON1':[], 'MUON2':[], 'MUON3':[]}
#     extraVariables = {'MUON1': ['InDetTrackParticles.-caloExtension','ExtrapolatedMuonTrackParticles.-caloExtension']} 

    ### all varaible containers
    commonAllVarList = ["Muons", "PrimaryVertices", "InDetTrackParticles", "MuonSegments", "MuonTruthParticles", "CombinedMuonTrackParticles", "ExtrapolatedMuonTrackParticles", "MuonSpectrometerTrackParticles"]
    MUON1OnlyAllVar = ['CaloCalTopoClusters', 'MuonClusterCollection']
    allVarContainer = {'MUON0':[], 'MUON1':MUON1OnlyAllVar, 'MUON2':[], 'MUON3':[]}

    ### keep trigger content? ## Not actually used, wait until ready
    UseTriggerContent = {'MUON0': False} # default is True

    ### MUON4
    Items['MUON4'] = []
    smSlContainer['MUON4'] = []
    allVarContainer['MUON4'] = []
    UseTriggerContent['MUON4'] = False

    ### get final lists
    for s in Items: Items[s]+=commonItems
    for s in allVarContainer: allVarContainer[s]+=commonAllVarList

    @classmethod
    def Config(self, stream, configKey):
        ### smart sliming
        from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
        SlHelper = SlimmingHelper(configKey+"SlimmingHelper")
        SlHelper.SmartCollections = self.smSlContainer[configKey]
        SlHelper.AllVariables = self.allVarContainer[configKey]
#         SlHelper.ExtraVariables = self.extraVariables.get(configKey, [])
#         SlHelper.IncludeMuonTriggerContent = self.UseTriggerContent.get(configKey, True)
        SlHelper.AppendContentToStream(stream)

        ### other items
        for item in self.Items[configKey]:
            stream.AddItem(item)

    def show(self):
        print 'Add Items:'
        for i in self.Items: print i,'=',self.Items[i]
        print 'Smart slimming:'
        for i in self.smSlContainer: print i,'=',self.smSlContainer[i]
        print 'Keep all varaibles:'
        for i in self.allVarContainer: print i,'=',self.allVarContainer[i]
        print 'Keep trigger content:'
        for i in self.UseTriggerContent: print i,'=',self.UseTriggerContent[i]

if __name__ == '__main__':
    a = MuonsDxAODStreamConfigurer()
    a.show()
