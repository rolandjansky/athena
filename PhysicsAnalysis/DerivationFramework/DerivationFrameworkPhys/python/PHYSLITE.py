# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
#====================================================================
# DAOD_PHYSLITE.py
# This defines DAOD_PHYSLITE, an unskimmed DAOD format for Run 3.
# It contains the variables and objects needed for the large majority 
# of physics analyses in ATLAS.
# It requires the flag PHYSLITE in Derivation_tf.py   
#====================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Main algorithm config
def PHYSLITEKernelCfg(ConfigFlags, name='PHYSLITEKernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel) for PHYSLITE"""
    acc = ComponentAccumulator()

    # Common augmentations
    from DerivationFrameworkPhys.PhysCommonConfig import PhysCommonAugmentationsCfg
    acc.merge(PhysCommonAugmentationsCfg(ConfigFlags, TriggerListsHelper = kwargs['TriggerListsHelper']))

    # Thinning tools...
    from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleThinningCfg, MuonTrackParticleThinningCfg, TauTrackParticleThinningCfg, DiTauTrackParticleThinningCfg, TauJetLepRMParticleThinningCfg
    from DerivationFrameworkTools.DerivationFrameworkToolsConfig import GenericObjectThinningCfg
    from DerivationFrameworkCalo.DerivationFrameworkCaloConfig import CaloClusterThinningCfg

    # Inner detector group recommendations for indet tracks in analysis
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations
    PHYSLITE_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
    PHYSLITETrackParticleThinningTool = acc.getPrimaryAndMerge(TrackParticleThinningCfg(
        ConfigFlags,
        name                    = "PHYSLITETrackParticleThinningTool",
        StreamName              = kwargs['StreamName'], 
        SelectionString         = PHYSLITE_thinning_expression,
        InDetTrackParticlesKey  = "InDetTrackParticles"))
    
    # Include inner detector tracks associated with muons
    PHYSLITEMuonTPThinningTool = acc.getPrimaryAndMerge(MuonTrackParticleThinningCfg(
        ConfigFlags,
        name                    = "PHYSLITEMuonTPThinningTool",
        StreamName              = kwargs['StreamName'],
        MuonKey                 = "Muons",
        InDetTrackParticlesKey  = "InDetTrackParticles"))
    
    # disable tau thinning for now
    tau_thinning_expression = "(TauJets.ptFinalCalib >= 0)"
    PHYSLITETauJetsThinningTool = acc.getPrimaryAndMerge(GenericObjectThinningCfg(ConfigFlags,
        name            = "PHYSLITETauJetThinningTool",
        StreamName      = kwargs['StreamName'],
        ContainerName   = "TauJets",
        SelectionString = tau_thinning_expression))
    
    # Only keep tau tracks (and associated ID tracks) classified as charged tracks
    PHYSLITETauTPThinningTool = acc.getPrimaryAndMerge(TauTrackParticleThinningCfg(
        ConfigFlags,
        name                   = "PHYSLITETauTPThinningTool",
        StreamName             = kwargs['StreamName'],
        TauKey                 = "TauJets",
        InDetTrackParticlesKey = "InDetTrackParticles",
        DoTauTracksThinning    = True,
        TauTracksKey           = "TauTracks"))
    
    tau_murm_thinning_expression = tau_thinning_expression.replace('TauJets', 'TauJets_MuonRM')
    PHYSLITETauJetMuonRMParticleThinningTool = acc.getPrimaryAndMerge(TauJetLepRMParticleThinningCfg(
        ConfigFlags,
        name                   = "PHYSLITETauJets_MuonRMThinningTool",
        StreamName             = kwargs['StreamName'],
        originalTauKey         = "TauJets",
        LepRMTauKey            = "TauJets_MuonRM",
        InDetTrackParticlesKey = "InDetTrackParticles",
        TauTracksKey           = "TauTracks_MuonRM",
        SelectionString        = tau_murm_thinning_expression))

    # ID tracks associated with high-pt di-tau
    PHYSLITEDiTauTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(
        ConfigFlags,
        name                    = "PHYSLITEDiTauTPThinningTool",
        StreamName              = kwargs['StreamName'],
        DiTauKey                = "DiTauJets",
        InDetTrackParticlesKey  = "InDetTrackParticles"))

    ## Low-pt di-tau thinning
    PHYSLITEDiTauLowPtThinningTool = acc.getPrimaryAndMerge(GenericObjectThinningCfg(ConfigFlags,
                                                                                 name            = "PHYSLITEDiTauLowPtThinningTool",
                                                                                 StreamName      = kwargs['StreamName'],
                                                                                 ContainerName   = "DiTauJetsLowPt",
                                                                                 SelectionString = "DiTauJetsLowPt.nSubjets > 1"))
    
    # ID tracks associated with low-pt ditau
    PHYSLITEDiTauLowPtTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(ConfigFlags,
                                                                                        name                    = "PHYSLITEDiTauLowPtTPThinningTool",
                                                                                        StreamName              = kwargs['StreamName'],
                                                                                        DiTauKey                = "DiTauJetsLowPt",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        SelectionString         = "DiTauJetsLowPt.nSubjets > 1"))

    # keep calo clusters around electrons
    PHYSLITEElectronCaloClusterThinningTool = acc.getPrimaryAndMerge(CaloClusterThinningCfg(
        ConfigFlags,
        name="PHYSLITEElectronCaloClusterThinningTool",
        StreamName=kwargs['StreamName'],
        SGKey="AnalysisElectrons",
        CaloClCollectionSGKey="egammaClusters",
        ConeSize=-1.0))

    # keep calo clusters around photons
    PHYSLITEPhotonCaloClusterThinningTool = acc.getPrimaryAndMerge(CaloClusterThinningCfg(
        ConfigFlags,
        name="PHYSLITEPhotonCaloClusterThinningTool",
        StreamName=kwargs['StreamName'],
        SGKey="AnalysisPhotons",
        CaloClCollectionSGKey="egammaClusters",
        ConeSize=-1.0))

    # Collect the thinning tools
    thinningTools = [PHYSLITETrackParticleThinningTool,
                     PHYSLITEMuonTPThinningTool,
                     PHYSLITETauJetsThinningTool,
                     PHYSLITETauTPThinningTool,
                     PHYSLITETauJetMuonRMParticleThinningTool,
                     PHYSLITEDiTauTPThinningTool,
                     PHYSLITEDiTauLowPtThinningTool,
                     PHYSLITEDiTauLowPtTPThinningTool,
                     PHYSLITEElectronCaloClusterThinningTool,
                     PHYSLITEPhotonCaloClusterThinningTool ]

    #==============================================================================
    # Analysis-level variables 
    #==============================================================================

    # Set up the systematics loader/handler algorithm:
    sysLoader = CompFactory.CP.SystematicsSvc( 'SystematicsSvc' )
    sysLoader.systematicsList= ['']
    acc.addService(sysLoader)

    dataType = "data"
    if ConfigFlags.Input.isMC: dataType = "mc"

    # Create a pile-up analysis sequence
    from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence
    pileupSequence = makePileupAnalysisSequence( dataType )
    pileupSequence.configure( inputName = {}, outputName = {} )
    for element in pileupSequence.getGaudiConfig2Components():
        acc.addEventAlgo(element)

    # Include, and then set up the electron analysis sequence:
    from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import  makeElectronAnalysisSequence
    electronSequence = makeElectronAnalysisSequence( dataType, 'LooseLHElectron.NonIso', shallowViewOutput = False, deepCopyOutput = True )
    electronSequence.configure( inputName = 'Electrons',
                                outputName = 'AnalysisElectrons' )
    for element in electronSequence.getGaudiConfig2Components():
        acc.addEventAlgo(element)

    # Include, and then set up the photon analysis sequence:                                       
    from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import makePhotonAnalysisSequence
    photonSequence = makePhotonAnalysisSequence( dataType, 'Loose.Undefined', deepCopyOutput = True, shallowViewOutput = False, recomputeIsEM=False )
    photonSequence.configure( inputName = 'Photons',
                              outputName = 'AnalysisPhotons' )
    for element in photonSequence.getGaudiConfig2Components():
        acc.addEventAlgo(element)

    # Include, and then set up the muon analysis algorithm sequence:
    from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
    muonSequence = makeMuonAnalysisSequence( dataType, shallowViewOutput = False, deepCopyOutput = True, workingPoint = 'Loose.NonIso' )
    muonSequence.configure( inputName = 'Muons',
                            outputName = 'AnalysisMuons' )
    for element in muonSequence.getGaudiConfig2Components():
        acc.addEventAlgo(element)

    # Include, and then set up the tau analysis algorithm sequence:                                                    
    # Commented for now due to use of public tools
    #from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
    #tauSequence = makeTauAnalysisSequence( dataType, 'Baseline', shallowViewOutput = False, deepCopyOutput = True )
    #tauSequence.configure( inputName = 'TauJets', outputName = 'AnalysisTauJets' )
    #for element in tauSequence.getGaudiConfig2Components():
    #    acc.addEventAlgo(element,sequenceName=tauSequence.name())

    # Include, and then set up the jet analysis algorithm sequence:
    jetContainer = 'AntiKt4EMPFlowJets'
    from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
    jetSequence = makeJetAnalysisSequence( dataType, jetContainer, deepCopyOutput = True, shallowViewOutput = False, runFJvtUpdate = False, runFJvtSelection = False, runJvtSelection = False)
    jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJets' )
    for element in jetSequence.getGaudiConfig2Components():
        acc.addEventAlgo(element)

    # Build MET from our analysis objects
    from METReconstruction.METAssocCfg import AssocConfig, METAssocConfig
    from METReconstruction.METAssociatorCfg import getAssocCA
    associators = [AssocConfig('PFlowJet', 'AnalysisJets'),
                   AssocConfig('Muon', 'AnalysisMuons'),
                   AssocConfig('Ele', 'AnalysisElectrons'),
                   AssocConfig('Gamma', 'AnalysisPhotons'),
                   #AssocConfig('Tau', 'AnalysisTauJets'),
                   AssocConfig('Soft', '')]
    PHYSLITE_cfg = METAssocConfig('AnalysisMET',
                                  ConfigFlags,
                                  associators,
                                  doPFlow=True,
                                  usePFOLinks=True)
    components_PHYSLITE_cfg = getAssocCA(PHYSLITE_cfg,sequencename='SeqPHYSLITE',METName='AnalysisMET')
    acc.merge(components_PHYSLITE_cfg)

    # The derivation kernel itself
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, ThinningTools = thinningTools)) 

    return acc


def PHYSLITECfg(ConfigFlags):

    acc = ComponentAccumulator()

    # Get the lists of triggers needed for trigger matching.
    # This is needed at this scope (for the slimming) and further down in the config chain
    # for actually configuring the matching, so we create it here and pass it down
    # TODO: this should ideally be called higher up to avoid it being run multiple times in a train
    from DerivationFrameworkPhys.TriggerListsHelper import TriggerListsHelper
    PHYSLITETriggerListsHelper = TriggerListsHelper()

    # Common augmentations
    acc.merge(PHYSLITEKernelCfg(ConfigFlags, name="PHYSLITEKernel", StreamName = 'StreamDAOD_PHYSLITE', TriggerListsHelper = PHYSLITETriggerListsHelper))

    # ============================
    # Define contents of the format
    # =============================
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    
    PHYSLITESlimmingHelper = SlimmingHelper("PHYSLITESlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)

    # Trigger content
    PHYSLITESlimmingHelper.IncludeTriggerNavigation = False
    PHYSLITESlimmingHelper.IncludeJetTriggerContent = False
    PHYSLITESlimmingHelper.IncludeMuonTriggerContent = False
    PHYSLITESlimmingHelper.IncludeEGammaTriggerContent = False
    PHYSLITESlimmingHelper.IncludeJetTauEtMissTriggerContent = False
    PHYSLITESlimmingHelper.IncludeTauTriggerContent = False
    PHYSLITESlimmingHelper.IncludeEtMissTriggerContent = False
    PHYSLITESlimmingHelper.IncludeBJetTriggerContent = False
    PHYSLITESlimmingHelper.IncludeBPhysTriggerContent = False
    PHYSLITESlimmingHelper.IncludeMinBiasTriggerContent = False

    # Trigger matching
    # Run 2
    if ConfigFlags.Trigger.EDMVersion == 2:
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import AddRun2TriggerMatchingToSlimmingHelper
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSLITESlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_", 
                                         TriggerList = PHYSLITETriggerListsHelper.Run2TriggerNamesTau)
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSLITESlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_",
                                         TriggerList = PHYSLITETriggerListsHelper.Run2TriggerNamesNoTau)
    # Run 3
    if ConfigFlags.Trigger.EDMVersion == 3:
        from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import AddRun3TrigNavSlimmingCollectionsToSlimmingHelper
        AddRun3TrigNavSlimmingCollectionsToSlimmingHelper(PHYSLITESlimmingHelper)        
        # Run 2 is added here temporarily to allow testing/comparison/debugging
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import AddRun2TriggerMatchingToSlimmingHelper
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSLITESlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_", 
                                         TriggerList = PHYSLITETriggerListsHelper.Run3TriggerNamesTau)
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSLITESlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_",
                                         TriggerList = PHYSLITETriggerListsHelper.Run3TriggerNamesNoTau)

    # Event content
    PHYSLITESlimmingHelper.AppendToDictionary = {
        'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
        'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
        'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
        'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
        'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
        'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
        'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
        'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
        'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
        'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
        'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
        'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
        'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
        'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
        'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
        'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
        'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
        'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
        'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer',
        'AnalysisElectrons':'xAOD::ElectronContainer', 'AnalysisElectronsAux':'xAOD::ElectronAuxContainer',
        'AnalysisMuons':'xAOD::MuonContainer', 'AnalysisMuonsAux':'xAOD::MuonAuxContainer',
        'AnalysisJets':'xAOD::JetContainer','AnalysisJetsAux':'xAOD::AuxContainerBase',
        'AnalysisPhotons':'xAOD::PhotonContainer', 'AnalysisPhotonsAux':'xAOD::PhotonAuxContainer',
        'AnalysisTauJets':'xAOD::TauJetContainer', 'AnalysisTauJetsAux':'xAOD::TauJetAuxContainer',
        'MET_Core_AnalysisMET':'xAOD::MissingETContainer', 'MET_Core_AnalysisMETAux':'xAOD::MissingETAuxContainer',
        'METAssoc_AnalysisMET':'xAOD::MissingETAssociationMap', 'METAssoc_AnalysisMETAux':'xAOD::MissingETAuxAssociationMap',
        'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
    }

    PHYSLITESlimmingHelper.SmartCollections = [
        'EventInfo',
        'InDetTrackParticles',
        'PrimaryVertices',
    ]
 
    PHYSLITESlimmingHelper.ExtraVariables = [ 
        'AnalysisElectrons.trackParticleLinks.pt.eta.phi.m.charge.author.DFCommonElectronsLHVeryLoose.DFCommonElectronsLHLoose.DFCommonElectronsLHLooseBL.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsLHVeryLooseIsEMValue.DFCommonElectronsLHLooseIsEMValue.DFCommonElectronsLHLooseBLIsEMValue.DFCommonElectronsLHMediumIsEMValue.DFCommonElectronsLHTightIsEMValue.DFCommonElectronsECIDS.DFCommonElectronsECIDSResult.ptvarcone20.ptvarcone40.topoetcone20.topoetcone20ptCorrection.ptcone20_Nonprompt_All_MaxWeightTTVA_pt500.ptcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000.caloClusterLinks.ambiguityLink.TruthLink.truthParticleLink.truthOrigin.truthType.truthPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink.firstEgMotherPdgId.ambiguityType.OQ',
        'AnalysisPhotons.pt.eta.phi.m.author.OQ.DFCommonPhotonsIsEMLoose.DFCommonPhotonsIsEMTight.DFCommonPhotonsIsEMTightIsEMValue.DFCommonPhotonsCleaning.DFCommonPhotonsCleaningNoTime.ptcone20.topoetcone20.topoetcone40.topoetcone20ptCorrection.topoetcone40ptCorrection.caloClusterLinks.vertexLinks.ambiguityLink.TruthLink.truthParticleLink.truthOrigin.truthType',
        'GSFTrackParticles.chiSquared.phi.d0.theta.qOverP.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.z0.vz.charge.vertexLink.numberOfPixelHits.numberOfSCTHits',
        'GSFConversionVertices.trackParticleLinks.x.y.z.px.py.pz.pt1.pt2.neutralParticleLinks.minRfirstHit',
        'egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.ETACALOFRAME.PHICALOFRAME.ETA2CALOFRAME.PHI2CALOFRAME.constituentClusterLinks',
        'AnalysisMuons.pt.eta.phi.truthType.truthOrigin.author.muonType.quality.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.InnerDetectorPt.MuonSpectrometerPt.DFCommonGoodMuon.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.TruthLink.truthParticleLink.charge.extrapolatedMuonSpectrometerTrackParticleLink.allAuthors.ptcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptcone20_Nonprompt_All_MaxWeightTTVA_pt500.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500.numberOfPrecisionLayers.combinedTrackOutBoundsPrecisionHits.numberOfPrecisionLayers.numberOfPrecisionHoleLayers.numberOfGoodPrecisionLayers.innerSmallHits.innerLargeHits.middleSmallHits.middleLargeHits.outerSmallHits.outerLargeHits.extendedSmallHits.extendedLargeHits.extendedSmallHoles.isSmallGoodSectors.cscUnspoiledEtaHits.EnergyLoss.energyLossType.momentumBalanceSignificance.scatteringCurvatureSignificance.scatteringNeighbourSignificance.CaloMuonIDTag',
        'CombinedMuonTrackParticles.qOverP.d0.z0.vz.phi.theta.truthOrigin.truthType.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.numberOfPixelDeadSensors.numberOfPixelHits.numberOfPixelHoles.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers.chiSquared.numberDoF',
        'ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.truthOrigin.truthType.qOverP.theta.phi',
        'MuonSpectrometerTrackParticles.phi.d0.z0.vz.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.vertexLink.theta.qOverP.truthParticleLink',
        'AnalysisTauJets.pt.eta.phi.m.ptFinalCalib.etaFinalCalib.ptTauEnergyScale.etaTauEnergyScale.charge.isTauFlags.PanTau_DecayMode.NNDecayMode.RNNJetScore.RNNJetScoreSigTrans.RNNEleScore.RNNEleScoreSigTrans.tauTrackLinks.vertexLink.truthParticleLink.truthJetLink.IsTruthMatched.truthOrigin.truthType',
        'AnalysisJets.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.NumTrkPt500.SumPtTrkPt500.DetectorEta.Jvt.JVFCorr.JvtRpt.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.PartonTruthLabelID.ConeTruthLabelID.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.TrueFlavor.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.btagging.btaggingLink.GhostTrack.DFCommonJets_fJvt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PSFrac.JetAccessorMap.EMFrac.Width.ActiveArea4vec_pt.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.EnergyPerSampling.SumPtChargedPFOPt500',
        'BTagging_AntiKt4EMPFlow.DL1r_pu.DL1rmu_pu.DL1r_pb.DL1rmu_pb.DL1r_pc.DL1rmu_pc',
        'TruthPrimaryVertices.t.x.y.z',
        'MET_Core_AnalysisMET.name.mpx.mpy.sumet.source',
        'METAssoc_AnalysisMET.',
        'InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights.numberOfTRTHits.numberOfTRTOutliers',
        'EventInfo.hardScatterVertexLink.RandomRunNumber',
        'Kt4EMPFlowEventShape.Density',
        'TauTracks.pt.eta.phi.flagSet.trackLinks'
    ]

    if ConfigFlags.Input.isMC:
        from DerivationFrameworkMCTruth.MCTruthCommonConfig import addTruth3ContentToSlimmerTool
        addTruth3ContentToSlimmerTool(PHYSLITESlimmingHelper)

    # Output stream    
    PHYSLITEItemList = PHYSLITESlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_PHYSLITE", ItemList=PHYSLITEItemList, AcceptAlgs=["PHYSLITEKernel"]))

    return acc

