#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo

def useSmartSlimmingIfSupported(itemPairs, smAllVarlist, addItemList, ContainerNamesAndTypes):
    for i in itemPairs:
        cname = i.split('#')[-1]
        if cname in ContainerNamesAndTypes and itemPairs[i].split('#')[-1].rstrip('.') in ContainerNamesAndTypes:
            smAllVarlist.append(cname)
            print cname, 'added to smartlimming'
        else:
            addItemList.append(i)
            addItemList.append(itemPairs[i])

def getMUON0TriggerContainers():
    return {'xAOD::MuonRoIContainer#LVL1MuonRoIs':'xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.',
'xAOD::L2StandAloneMuonContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo':'xAOD::L2StandAloneMuonAuxContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfoAux.',
'xAOD::L2CombinedMuonContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo':'xAOD::L2CombinedMuonAuxContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfoAux.',
'xAOD::L2IsoMuonContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfo':'xAOD::L2IsoMuonAuxContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfoAux.',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo':'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux.',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo_FullScan':'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfo_FullScanAux.',
'xAOD::MuonRoIContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuon':'xAOD::MuonRoIAuxContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuonAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFID':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTFAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFIDAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTF':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTFAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrigAux.'}


class MuonsDxAODStreamConfigurer:
    commonItems = []
    MUON0OnlyItems = ["CosmicMuonCollection#HLT_CosmicMuons", "MuonCaloEnergyContainer#MuonCaloEnergyCollection"]
#     MUON1OnlyItems = ['CaloClusterCellLinkContainer#MuonClusterCollection_links', 'CaloCellContainer#AODCellContainer']
    MUON1OnlyItems = [] ## will be done in MUON1
    MUON2OnlyItems = []
    MUON3OnlyItems = []

    MUON0OnlyItems += ['xAOD::TrigNavigation#*','xAOD::TrigNavigationAuxInfo#*'] ## be careful, they could go to smart slimming...

    ### samrt slimming containers
    comSmSlList = ["Muons", "PrimaryVertices", "InDetTrackParticles","AntiKt4EMPFlowJets_BTagging201810", "AntiKt4EMPFlowJets_BTagging201903", "AntiKt4EMTopoJets_BTagging201810", "BTagging_AntiKt4EMTopo_201810", "BTagging_AntiKt4EMPFlow_201810",  "BTagging_AntiKt4EMPFlow_201903"]
    smSlContainer = {'MUON0':[], 'MUON1':['AntiKt4EMPFlowJets','AntiKt4EMTopoJets','Electrons','MET_Reference_AntiKt4EMTopo','MET_Reference_AntiKt4EMPFlow','Photons'], 'MUON2':['AntiKt4EMPFlowJets','AntiKt4EMTopoJets'], 'MUON3':['AntiKt4EMTopoJets','Electrons']}

    ### all varaible containers
#     commonAllVarList = ["Muons", "PrimaryVertices", "InDetTrackParticles", "MuonSegments", "MuonTruthParticles", "CombinedMuonTrackParticles", "ExtrapolatedMuonTrackParticles", "MuonSpectrometerTrackParticles", "InDetForwardTrackParticles"]
    commonAllVarList = ["Muons", "MuonSegments", "MuonTruthParticles", "CombinedMuonTrackParticles", "ExtrapolatedMuonTrackParticles", "MuonSpectrometerTrackParticles", "InDetForwardTrackParticles","MSOnlyExtrapolatedMuonTrackParticles"]
    MUON0OnlyAllVar = ['Staus','ExtrapolatedStauTrackParticles','CombinedStauTrackParticles','SlowMuons',"InDetTrackParticles"] # slow muons
    MUON1OnlyAllVar = ['CaloCalTopoClusters', 'MuonClusterCollection', "InDetTrackParticles"]
    MUON2OnlyAllVar = ['PrimaryVertices']
    MUON3OnlyAllVar = ['CaloCalTopoClusters', 'MuonClusterCollection']
    MUON4OnlyAllVar = ['PrimaryVertices']

#     if globalflags.DataSource()=='geant4':
    if DerivationFrameworkIsMonteCarlo:
        MUON1OnlyAllVar += ['AntiKt4TruthJets',"TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth"]

#     useSmartSlimmingIfSupported(getMUON0TriggerContainers(), MUON0OnlyAllVar, MUON0OnlyItems)
    allVarContainer = {'MUON0':MUON0OnlyAllVar, 'MUON1':MUON1OnlyAllVar, 'MUON2':[], 'MUON3':MUON3OnlyAllVar}
    Items = {'MUON0':MUON0OnlyItems, 'MUON1':MUON1OnlyItems, 'MUON2':MUON2OnlyItems, 'MUON3':MUON3OnlyItems}

    ### keep trigger content
    UseTriggerContent = {'MUON0': False} # default is True, MUON0 added by hand

    ### MUON4
    Items['MUON4'] = []
    smSlContainer['MUON4'] = []
    allVarContainer['MUON4'] = []
    UseTriggerContent['MUON4'] = False

    ### MUON6
    Items['MUON6'] = []
    smSlContainer['MUON6'] = []
    allVarContainer['MUON6'] = []
    UseTriggerContent['MUON6'] = True 

    checkContainers = {'MUON0':getMUON0TriggerContainers(), 'MUON1':getMUON0TriggerContainers(), 'MUON2':getMUON0TriggerContainers()}

    ### Extra variables
    ### Eventshape for pileup subtraction in isolation
    eventShapeVars = ['TopoClusterIsoCentralEventShape.DensitySigma.Density.DensityArea',
                      'TopoClusterIsoForwardEventShape.DensitySigma.Density.DensityArea',
                      'NeutralParticleFlowIsoCentralEventShape.DensitySigma.Density.DensityArea',
                      'NeutralParticleFlowIsoForwardEventShape.DensitySigma.Density.DensityArea']

    extraVariables = {'MUON1':eventShapeVars, 'MUON2':eventShapeVars, 'MUON3':eventShapeVars}

    ### For FSR check
    extraVariables['MUON1'].append('Photons.truthType.truthOrigin.topoetcone40')
    extraVariables['MUON1'].append('Electrons.truthType.truthOrigin.topoetcone40')
    extraVariables['MUON1'].append('InDetTrackParticles.deltaphi_0.deltatheta_0.sigmadeltaphi_0.sigmadeltatheta_0.deltaphi_1.deltatheta_1.sigmadeltaphi_1.sigmadeltatheta_1')
    extraVariables['MUON3'].append('Photons.truthType.truthOrigin.topoetcone40')
    extraVariables['MUON3'].append('Electrons.truthType.truthOrigin.topoetcone40')
    extraVariables['MUON3'].append('InDetTrackParticles.deltaphi_0.deltatheta_0.sigmadeltaphi_0.sigmadeltatheta_0.deltaphi_1.deltatheta_1.sigmadeltaphi_1.sigmadeltatheta_1')

    ### PV slimming for size reduction
    pvExtra = 'PrimaryVertices.numberDoF.chiSquared.sumPt2.x.y.z'
    extraVariables['MUON1'].append(pvExtra)
    extraVariables['MUON3'].append(pvExtra)
    extraVariables['MUON0'] = [pvExtra]


    ## for TrackAssociatedCaloSample
    extraVariables['MUON1'].append('InDetTrackParticles.trackCaloClusEta.trackCaloClusPhi.trackCaloSampleE.trackCaloSampleNumber')


    ### get final lists
    for s in Items: Items[s]+=commonItems
    for s in allVarContainer: allVarContainer[s]+=commonAllVarList
    for s in smSlContainer: smSlContainer[s]+=comSmSlList

    @classmethod
    def Config(self, stream, configKey):
        ### smart sliming
        from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
        SlHelper = SlimmingHelper(configKey+"SlimmingHelper")
        SlHelper.SmartCollections = self.smSlContainer[configKey]
        useSmartSlimmingIfSupported(self.checkContainers.get(configKey, []),self.allVarContainer[configKey], self.Items[configKey], SlHelper.NamesAndTypes)
        SlHelper.AllVariables = self.allVarContainer[configKey]
#         SlHelper.StaticContent = self.Items[configKey]
        SlHelper.ExtraVariables = self.extraVariables.get(configKey, [])
        SlHelper.IncludeMuonTriggerContent = self.UseTriggerContent.get(configKey, True)
        SlHelper.AppendContentToStream(stream)

        ### other items
        for item in self.Items[configKey]:
            stream.AddItem(item)

    def show(self):
        print 'Add Items:'
        for i in self.Items: print i,'=',self.Items[i]
        print 'Smart slimming:'
        for i in self.smSlContainer and (not i in self.allVarContainer): print i,'=',self.smSlContainer[i]
        print 'Keep all varaibles:'
        for i in self.allVarContainer: print i,'=',self.allVarContainer[i]
        print 'Keep trigger content:'
        for i in self.UseTriggerContent: print i,'=',self.UseTriggerContent[i]


if __name__ == '__main__':
    a = MuonsDxAODStreamConfigurer()
    a.show()
