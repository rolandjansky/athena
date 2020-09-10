# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# commont content for Hbb DAODs
# will impact content of HIGG5D1, HIGG5D2, HIGG5D3, HIGG2D4

def getJetEMPFlowName() :
    from BTagging.BTaggingFlags import BTaggingFlags
    if BTaggingFlags.Do2019Retraining:
        return 'AntiKt4EMPFlowJets_BTagging201810'
    else:
        return 'AntiKt4EMPFlowJets'

def getBTagEMTopoName() :
    from BTagging.BTaggingFlags import BTaggingFlags
    if BTaggingFlags.Do2019Retraining:
        return 'BTagging_AntiKt4EMTopo_201810'
    else:
        return 'BTagging_AntiKt4EMTopo'

def getJetEMTopoName() :
    from BTagging.BTaggingFlags import BTaggingFlags
    if BTaggingFlags.Do2019Retraining:
        return 'AntiKt4EMTopoJets_BTagging201810'
    else:
        return 'AntiKt4EMTopoJets'

def addVRSmallJets(sequence, largeRColls = None, do_ghost=False, logger=None, doFlipTagger=False, training='201810', *pos_opts, **opts):
    from AthenaCommon import Logging
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
    addCHSPFlowObjects()

    if logger is None:
        logger = Logging.logging.getLogger('VRLogger')
    #
    if opts or pos_opts:
        logger.warning('Options specified for VR jets, they will be ignored')

    if largeRColls is None:
      largeRColls = ["AntiKt4EMPFlowJets"]
    import DerivationFrameworkFlavourTag.HbbCommon as HbbCommon
    VRName, ghostLab = HbbCommon.buildVRJets(sequence, do_ghost, logger, doFlipTagger, training)
    toAssociate = {
        ghostLab : ghostLab.lower()
        }
    #
    ungroomedNames = []
    for collection in largeRColls:
        ungroomedName, getters = linkVRJetsToSmallRJets(sequence, collection, toAssociate)
        ungroomedNames.append(ungroomedName)

    return ungroomedNames
#
def getJetRecTool(collection, getParent=True):
    from JetRec.JetRecStandardToolManager import jtm
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
    addCHSPFlowObjects()
    if "AntiKt4EMPFlowJets" not in jtm.tools:
        jtm.addJetFinder("AntiKt4EMPFlowJets",  "AntiKt", 0.4,   "empflow", "pflow_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter= 10000, calibOpt="arj:pflow")
    try:
      jetRecTool = jtm[collection]
    except KeyError as e:
        raise KeyError("JetRecTool {0} not present in jtm".format(collection) )
    if getParent and hasattr(jetRecTool, "InputContainer") and jetRecTool.InputContainer:
        jetRecTool = getJetRecTool(jetRecTool.InputContainer, True)
    return jetRecTool
#
def linkVRJetsToSmallRJets(
    sequence, collection, getters):
    from JetRec.JetRecStandardToolManager import jtm
    import DerivationFrameworkJetEtMiss.JetCommon as JetCommon
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import nameJetsFromAlg
    from AthenaCommon import Logging
    logger = Logging.logging.getLogger('HbbTaggerLog')
    #
    originalJetRecTool = getJetRecTool(collection, getParent = True)
    originalUngroomedName = originalJetRecTool.name()
    ungroomedJetAlg = originalUngroomedName
    if ungroomedJetAlg.endswith("Jets"):
      ungroomedJetAlg = ungroomedJetAlg[:-4]
    originalFinder = jtm[originalJetRecTool.JetFinder.getName()]
    originalGetters = [jtm[g.getName()] for g in originalJetRecTool.PseudoJetGetters]
    newGetters = [jtm[g] for g in getters.values()]
    #
    comb_name = "_".join(getters.keys() )
    LargeRJetFindingAlg = "jfind_{0}_{1}".format(collection, comb_name).lower()
    LargeRJetPrefix     = "{0}_{1}".format(collection, comb_name)
    LargeRJets = nameJetsFromAlg(LargeRJetPrefix)
    LinkTransferAlg     = "LinkTransfer_{0}_{1}".format(collection, comb_name)
    # Check to see if this large R jet collection is already known to JetCommon                                                                                                                               
    if LargeRJetFindingAlg in JetCommon.DFJetAlgs:
      logger.info("Found {0} in DFJetAlgs".format(LargeRJetFindingAlg) )
      # Is it in our sequence?                                                                                                                                                             
      if hasattr(sequence, LargeRJetFindingAlg):
        logger.info("Algorithm already exists in the input sequence. Will not "
                    "add again")
      else:
          logger.info("Adding algorithm into the sequence {0}".format(sequence) )
          sequence += JetCommon.DFJetAlgs[LargeRJetFindingAlg]
    else:
    # Check to see if the corresponding JetRecTool already exists                                                                                                                           
        if hasattr(jtm, LargeRJets):
            logger.info("JetRecTool {0} already exists in jtm".format(LargeRJets) )
        else:
            logger.info("Create a new JetRecTool {0}".format(LargeRJets) )
            JetCommon.OutputJets.setdefault("CustomJets", []).append(LargeRJets)
            originalModifiers = [jtm[m.getName()] for m in originalJetRecTool.JetModifiers]
            jtm.addJetFinder(
                output = LargeRJets,
                alg = originalFinder.JetAlgorithm,
                radius = originalFinder.JetRadius,
                gettersin = originalGetters + newGetters,
                modifiersin = originalModifiers,
                ghostArea = 0.01,
                ptmin = originalFinder.PtMin,
                variableRMinRadius = originalFinder.VariableRMinRadius,
                variableRMassScale = originalFinder.VariableRMassScale,
                calibOpt = "none")
        from JetRec.JetRecConf import JetAlgorithm
        logger.info(
            "Creating new jet algorithm {0} and adding it to sequence {1}".format(
                LargeRJetFindingAlg, sequence) )
        theJetAlg = JetAlgorithm(LargeRJetFindingAlg, Tools = [jtm[LargeRJets] ])
        sequence += theJetAlg
        JetCommon.DFJetAlgs[LargeRJetFindingAlg] = theJetAlg
    
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getJetExternalAssocTool, applyJetAugmentation
    assocTool = getJetExternalAssocTool(
        ungroomedJetAlg,
        LargeRJetPrefix,
        MomentPrefix = '',
        ListOfOldLinkNames=[g.Label for g in newGetters]
        )
    applyJetAugmentation(
      ungroomedJetAlg, LinkTransferAlg, sequence, assocTool)

    return originalUngroomedName, [g.Label for g in newGetters]

def getHIGG5Common() :
    from BTagging.BTaggingFlags import BTaggingFlags
    Common = [
        "egammaClusters.rawE.phi_sampl.calM",
        "Muons.clusterLink.EnergyLoss.energyLossType",
        "TauJets.IsTruthMatched.truthJetLink.truthParticleLink.ptDetectorAxis.etaDetectorAxis.phiDetectorAxis.mDetectorAxis",
        (getJetEMTopoName()+".TrackWidthPt500.GhostTrackCount.Jvt.JvtJvfcorr.JvtRpt"
         ".JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.DetectorEta"
         ".DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.DFCommonJets_Calib_m"),
        # ("AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810.-JetConstitScaleMomentum_pt.-JetConstitScaleMomentum_eta.-JetConstitScaleMomentum_phi.-JetConstitScaleMomentum_m"
        #    ".-constituentLinks.-constituentWeight.-ConstituentScale"),
        ("AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet"
         ".NumTrkPt1000.NumTrkPt500.TrackWidthPt1000.TrackWidthPt500.SumPtTrkPt1000.SumPtTrkPt500"),
        ("AntiKt10UFOCSSKJets.GhostVR30Rmax4Rmin02TrackJet"
         ".NumTrkPt1000.NumTrkPt500.TrackWidthPt1000.TrackWidthPt500.SumPtTrkPt1000.SumPtTrkPt500"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201810"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201903"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag"),
        ("AntiKt4EMPFlowJets"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201810"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201903"),
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
         ".PlanarFlow.Angularity.Aplanarity.FoxWolfram0.FoxWolfram2.KtDR.ZCut12"
         ".GhostTrack.GhostTrackCount.GhostVR30Rmax4Rmin02TrackJet"
         ".Width"),
        ("AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.constituentLinks"
         ".JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m"
         ".Angularity.Aplanarity.DetectorEta.ECF1.ECF2.ECF3.FoxWolfram0.FoxWolfram2.GhostMuonSegmentCount.GhostTrackCount.KtDR.Parent"
         ".PlanarFlow.Qw.Split12.Split23.Tau1_wta.Tau2_wta.Tau3_wta.ZCut12"
         ".NumTrkPt1000.NumTrkPt500.TrackWidthPt1000.TrackWidthPt500.SumPtTrkPt1000.SumPtTrkPt500"
         ".GhostAntiKt2TrackJet.GhostTrack.GhostVR30Rmax4Rmin02TrackJet"
         ".Width"),
        ("AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.pt.eta.phi.m.constituentLinks"
         ".JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m"
         ".Angularity.Aplanarity.DetectorEta.ECF1.ECF2.ECF3.FoxWolfram0.FoxWolfram2.GhostMuonSegmentCount.GhostTrackCount.KtDR.Parent"
         ".PlanarFlow.Qw.Split12.Split23.Tau1_wta.Tau2_wta.Tau3_wta.ZCut12"
         ".NumTrkPt1000.NumTrkPt500.TrackWidthPt1000.TrackWidthPt500.SumPtTrkPt1000.SumPtTrkPt500"
         ".GhostTrack.GhostVR30Rmax4Rmin02TrackJet"
         ".Width"),

        ("AntiKt10TrackCaloClusterJets"
         ".NumTrkPt1000.NumTrkPt500.SumPtTrkPt1000.SumPtTrkPt500.TrackWidthPt1000.TrackWidthPt500"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201810"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201903"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag"
         ".GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag"),
         "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag",
         "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag",
         "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag",
         "BTagging_AntiKtVR30Rmax4Rmin02Track_201903GhostTag",

        # "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.MV2c10_discriminant.MV2cl100_discriminant",
        # "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.DL1_pu.DL1_pc.DL1_pb.DL1mu_pu.DL1mu_pc.DL1mu_pb.DL1rnn_pu.DL1rnn_pc.DL1rnn_pb",
        getBTagEMTopoName()+".MV2cl100_discriminant"]
    if BTaggingFlags.Do2019Retraining:
        Common += ["BTagging_AntiKt4EMPFlow_201810.MV2cl100_discriminant", "BTagging_AntiKt4EMPFlow_201903.MV2cl100_discriminant"]
    else:
        Common += ["BTagging_AntiKt4EMPFlow.MV2cl100_discriminant"]
    Common += [
        "BTagging_AntiKt4PV0Track.MV2cl100_discriminant",
     "CaloCalTopoClusters.CENTER_MAG.calE.calEta.calM.calPhi.calPt.e_sampl.etaCalo.eta_sampl.phiCalo.phi_sampl.rawE.rawEta.rawM.rawPhi",
        "TauChargedParticleFlowObjects.bdtPi0Score.e.eta.m.phi.pt.rapidity",
        "TrackCaloClustersCombinedAndNeutral.pt.eta.phi.m.taste.trackParticleLink.caloClusterLinks"
        ]
    return Common

def getHIGG5CommonTruthContainers() :
    return ["TruthPrimaryVertices","HardScatterParticles","HardScatterVertices","TruthBosonsWithDecayParticles","TruthBosonsWithDecayVertices","TruthTopQuarkWithDecayParticles","TruthTopQuarkWithDecayVertices","TruthElectrons","TruthMuons","TruthTaus","TruthTausWithDecayParticles","TruthTausWithDecayVertices","TruthTaus","TruthNeutrinos","TruthBSM","TruthHFWithDecayParticles","TruthHFWithDecayVertices"]

def getHIGG5CommonTruth() :
    return [
        getJetEMTopoName()+".ConeTruthLabelID",
        getJetEMPFlowName()+".ConeTruthLabelID",
        "TruthEvents.PDFID1.PDFID2.PDGID1.PDGID2.Q.X1.X2.XF1.XF2.weights.crossSection.crossSectionError.truthParticleLinks",
        #"TruthVertices.barcode.x.y.z.t.id.incomingParticleLinks.outgoingParticleLinks",
        "TruthParticles.px.py.pz.e.m.decayVtxLink.prodVtxLink.barcode.pdgId.status.TopHadronOriginFlag.classifierParticleOrigin.classifierParticleType.classifierParticleOutCome.dressedPhoton.polarizationTheta.polarizationPhi",
        "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthParticleLink.truthType",
        'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID',
        'AntiKt10LCTopoJetsJets.PartonTruthLabelID',
        'AntiKt10TrackCaloClusterJets.PartonTruthLabelID'
    ]

def getHIGG5CommonTruthDictionExtionson() :
    return {
        "TruthBoson"                         : "xAOD::TruthParticleContainer" ,
        "TruthBosonAux"                      : "xAOD::TruthParticleAuxContainer" ,
        "TruthTop"                           : "xAOD::TruthParticleContainer" ,
        "TruthTopAux"                        : "xAOD::TruthParticleAuxContainer" ,
        "TruthBSM"                           : "xAOD::TruthParticleContainer" ,
        "TruthBSMAux"                        : "xAOD::TruthParticleAuxContainer" ,
        "HardScatterParticles"               : "xAOD::TruthParticleContainer" ,
        "HardScatterParticlesAux"            : "xAOD::TruthParticleAuxContainer" ,
        "TruthBosonsWithDecayParticles"      : "xAOD::TruthParticleContainer" ,
        "TruthBosonsWithDecayParticlesAux"   : "xAOD::TruthParticleAuxContainer" ,
        "TruthTopQuarkWithDecayParticles"    : "xAOD::TruthParticleContainer" ,
        "TruthTopQuarkWithDecayParticlesAux" : "xAOD::TruthParticleAuxContainer" ,
        "TruthTausWithDecayParticles"        : "xAOD::TruthParticleContainer" ,
        "TruthTausWithDecayParticlesAux"     : "xAOD::TruthParticleAuxContainer" ,
        "TruthHFWithDecayParticles"          : "xAOD::TruthParticleContainer" ,
        "TruthHFWithDecayParticlesAux"       : "xAOD::TruthParticleAuxContainer" ,
        "HardScatterVertices"                : "xAOD::TruthVertexContainer" ,
        "HardScatterVerticesAux"             : "xAOD::TruthVertexAuxContainer" ,
        "TruthBosonsWithDecayVertices"       : "xAOD::TruthVertexContainer" ,
        "TruthBosonsWithDecayVerticesAux"    : "xAOD::TruthVertexAuxContainer" ,
        "TruthTopQuarkWithDecayVertices"     : "xAOD::TruthVertexContainer" ,
        "TruthTopQuarkWithDecayVerticesAux"  : "xAOD::TruthVertexAuxContainer" ,
        "TruthTausWithDecayVertices"         : "xAOD::TruthVertexContainer" ,
        "TruthTausWithDecayVerticesAux"      : "xAOD::TruthVertexAuxContainer" ,
        "TruthPrimaryVertices"               : "xAOD::TruthVertexContainer",
        "TruthPrimaryVerticesAux"            : "xAOD::TruthVertexAuxContainer",
        "TruthHFWithDecayVertices"           : "xAOD::TruthVertexContainer",
        "TruthHFWithDecayVerticesAux"        : "xAOD::TruthVertexAuxContainer"
        }

def getHIGG5CommonDictionExtionson(add_truth_if_mc=True) :
  common_dict = {
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"                     : "xAOD::JetContainer"        ,
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810Aux"                  : "xAOD::JetAuxContainer"     ,
      "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"                        : "xAOD::BTaggingContainer"   ,
      "BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux"                     : "xAOD::BTaggingAuxContainer",
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"                     : "xAOD::JetContainer"        ,
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903Aux"                  : "xAOD::JetAuxContainer"     ,
      "BTagging_AntiKtVR30Rmax4Rmin02Track_201903"                        : "xAOD::BTaggingContainer"   ,
      "BTagging_AntiKtVR30Rmax4Rmin02Track_201903Aux"                     : "xAOD::BTaggingAuxContainer",
      }
  from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
  if add_truth_if_mc and DerivationFrameworkIsMonteCarlo:
      common_dict.update( getHIGG5CommonTruthDictionExtionson() )
  return common_dict

def getHIGG5CommonSmartCollections(add_truth_if_mc=True) :
    from BTagging.BTaggingFlags import BTaggingFlags
    common_smart_collections= ["Electrons",
                               "Photons",
                               "Muons",
                               "TauJets",
                               "MET_Reference_AntiKt4EMTopo",
                               "MET_Reference_AntiKt4EMPFlow",
                               "AntiKt4EMTopoJets",
                               "AntiKt4EMPFlowJets",
                               getJetEMTopoName()]
    if BTaggingFlags.Do2019Retraining:
        common_smart_collections+=["AntiKt4EMPFlowJets_BTagging201810", "AntiKt4EMPFlowJets_BTagging201903"]
    else:
        common_smart_collections+=["AntiKt4EMPFlowJets"]
    common_smart_collections+= [
                               "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                               "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                               # "AntiKtVR30Rmax4Rmin02Track",
                               getBTagEMTopoName()]
    if BTaggingFlags.Do2019Retraining:
        common_smart_collections+=["BTagging_AntiKt4EMPFlow_201810", "BTagging_AntiKt4EMPFlow_201903"]
    else:
        common_smart_collections+=["BTagging_AntiKt4EMPFlow"]
    common_smart_collections+= [
                               #"BTagging_AntiKt2Track",
                               #  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                               "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                               "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                               "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag",
                               "BTagging_AntiKtVR30Rmax4Rmin02Track_201903GhostTag",
                               "InDetTrackParticles",
                               "PrimaryVertices"]
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
    if add_truth_if_mc and DerivationFrameworkIsMonteCarlo :
        common_smart_collections += [
            "AntiKt4TruthJets",
            "AntiKt4TruthDressedWZJets"
            #          ,"AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets"
            ]
    return common_smart_collections


def filterContentList(pattern, content_list) :
    result=[]
    import re
    pat=re.compile(pattern)
    for elm in content_list :
        head=elm.split('.',2)[0]
        if pat.match(head) :
            result.append(elm)
    return result

def getTruth3Collections(kernel) :

    #STEP 1do the prejet augmentations by hand 
    decorationDressing='dressedPhoton'
    import DerivationFrameworkMCTruth.TruthDerivationTools
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc.DFCommonTruthElectronDressingTool.decorationName = decorationDressing
    ToolSvc.DFCommonTruthMuonDressingTool.decorationName = decorationDressing

    if not hasattr(kernel,'MCTruthCommonPreJetKernel'):
        augmentationToolsList = [ ToolSvc.DFCommonTruthClassificationTool,
                                  ToolSvc.DFCommonTruthMuonTool,ToolSvc.DFCommonTruthElectronTool,
                                  ToolSvc.DFCommonTruthPhotonToolSim,
                                  ToolSvc.DFCommonTruthNeutrinoTool,
                                  ToolSvc.DFCommonTruthTopTool,
                                  ToolSvc.DFCommonTruthBosonTool,
                                  ToolSvc.DFCommonTruthBSMTool,
                                  ToolSvc.DFCommonTruthElectronDressingTool, ToolSvc.DFCommonTruthMuonDressingTool,
                                  ToolSvc.DFCommonTruthElectronIsolationTool1, ToolSvc.DFCommonTruthElectronIsolationTool2,
                                  ToolSvc.DFCommonTruthMuonIsolationTool1, ToolSvc.DFCommonTruthMuonIsolationTool2,
                                  ToolSvc.DFCommonTruthPhotonIsolationTool1, ToolSvc.DFCommonTruthPhotonIsolationTool2]
        from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
        from AthenaCommon import CfgMgr
        kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonPreJetKernel",
                                                                 AugmentationTools = augmentationToolsList
                                                                 )
    #STEP2 rest of addStandardTruth
    from DerivationFrameworkMCTruth.MCTruthCommon import addTruthJets,addTruthMET
    from DerivationFrameworkMCTruth.MCTruthCommon import schedulePostJetMCTruthAugmentations
    from DerivationFrameworkMCTruth.MCTruthCommon import addHFAndDownstreamParticles,addBosonsAndDownstreamParticles,addTopQuarkAndDownstreamParticles
    from DerivationFrameworkMCTruth.MCTruthCommon import addHardScatterCollection,addPVCollection,addTruthCollectionNavigationDecorations,addParentAndDownstreamParticles
    # Jets and MET
    addTruthJets(kernel, decorationDressing)
    addTruthMET(kernel)
    # Tools that must come after jets
    schedulePostJetMCTruthAugmentations(kernel, decorationDressing)
    #STEP3 SPECIAL add B and C hadrons (keep all generations below)
    addHFAndDownstreamParticles(kernel,True,True,-1)
    #STEP4 SPECIAL W/Z/H boson collection with children (this should then also include W decay products of top)
    addBosonsAndDownstreamParticles(kernel,1)
    #STEP5 INCLUDE special top collection with 1 generation below (custom)
    addTopQuarkAndDownstreamParticles(kernel,1)
     #STEP6 hard scatter information 
     ### generation=1 (only one generation, so really ME...)
     ### generation=2 add both two VBF/VBS quarks in final state
    addHardScatterCollection(kernel, 2)
     #STEP7 add PV information (up to ~60 vertices per event)
    addPVCollection(kernel)
     #STEP8 add custom tau collection with 1 generation below (custom)
    addParentAndDownstreamParticles(kernel,
                                    generations=1,
                                    parents=[15],
                                    prefix='Taus')
     #STEP9
     # Add back the navigation contect for the collections we want
    addTruthCollectionNavigationDecorations(kernel,["HardScatterParticles","TruthBosonsWithDecayParticles","TruthTopQuarkWithDecayParticles","TruthElectrons","TruthMuons","TruthTausWithDecayParticles","TruthNeutrinos","TruthBSM"])



# --- common thinning tools
def getTruthThinningTool(tool_prefix, thinning_helper) :
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
    if not DerivationFrameworkIsMonteCarlo :
        return None
    # MC truth thinning (not for data)
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    # truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_Top_Quark  = "((abs(TruthParticles.pdgId) == 6))"
    truth_cond_BC_Quark  = "((abs(TruthParticles.pdgId) ==  5))||((abs(TruthParticles.pdgId) ==  4))" # C quark and Bottom quark
    truth_cond_Hadrons = "((abs(TruthParticles.pdgId) >=  400)&&(abs(TruthParticles.pdgId)<600))||((abs(TruthParticles.pdgId) >=  4000)&&(abs(TruthParticles.pdgId)<6000))||((abs(TruthParticles.pdgId) >=  10400)&&(abs(TruthParticles.pdgId)<10600))||((abs(TruthParticles.pdgId) >=  20400)&&(abs(TruthParticles.pdgId)<20600))"
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Top_Quark+') || (('+ truth_cond_Hadrons+')&&(sqrt(TruthParticles.px*TruthParticles.px+TruthParticles.py*TruthParticles.py)>5000))'


    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    MCThinningTool = DerivationFramework__GenericTruthThinning(
        name                         = tool_prefix + "MCThinningTool",
        ThinningService              = thinning_helper.ThinningSvc(),
        ParticleSelectionString      = truth_expression,
        PreserveDescendants          = False,
        PreserveGeneratorDescendants = False,
        PreserveAncestors            = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += MCThinningTool
    return MCThinningTool

def getInDetTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault('name',                   tool_prefix + 'TPThinningTool')
    kwargs.setdefault('ThinningService',        thinning_helper.ThinningSvc())
    kwargs.setdefault('SelectionString',        '( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )')
    kwargs.setdefault('InDetTrackParticlesKey', 'InDetTrackParticles')

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
    thinning_tool = DerivationFramework__TrackParticleThinning( name = kwargs.pop('name'), **kwargs)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += thinning_tool
    return thinning_tool

def getMuonTrackParticleThinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
    thinning_tool = DerivationFramework__MuonTrackParticleThinning(name                   = tool_prefix + "MuonTPThinningTool",
                                                                   ThinningService        = thinning_helper.ThinningSvc(),
                                                                   MuonKey                = "Muons",
                                                                   InDetTrackParticlesKey = "InDetTrackParticles")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += thinning_tool
    return thinning_tool

def getMuonCaloClusterThinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
    thinning_tool = DerivationFramework__CaloClusterThinning(name                   = tool_prefix + "MuonCCThinningTool",
                                                             ThinningService        = thinning_helper.ThinningSvc(),
                                                             SGKey                  = "Muons",
                                                             TopoClCollectionSGKey  = "CaloCalTopoClusters",
                                                             SelectionString        = "Muons.pt > 6*GeV",
                                                             ConeSize               = 0.4)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += thinning_tool
    return thinning_tool

def getElectronCaloClusterThinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
    thinning_tool = DerivationFramework__CaloClusterThinning(name                   = tool_prefix + "ElectronCCThinningTool",
                                                             ThinningService        = thinning_helper.ThinningSvc(),
                                                             SGKey                  = "Electrons",
                                                             TopoClCollectionSGKey  = "CaloCalTopoClusters",
                                                             # SelectionString        = "Electrons.pt > 6*GeV",
                                                             ConeSize               = 0.4)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += thinning_tool
    return thinning_tool

def getEgammaTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
    kwargs.setdefault( 'name',                  tool_prefix + "ElectronTPThinningTool")
    kwargs.setdefault( 'ThinningService',       thinning_helper.ThinningSvc())
    kwargs.setdefault( 'SGKey',                 'Electrons')
    kwargs.setdefault( 'InDetTrackParticlesKey','InDetTrackParticles')
    kwargs.setdefault( 'BestMatchOnly',         True)

    thinning_tool =  DerivationFramework__EgammaTrackParticleThinning(name  = kwargs.pop('name'),
                                                                  **kwargs)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getElectronTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name',                  tool_prefix + "ElectronTPThinningTool")
    kwargs.setdefault( 'SGKey','Electrons')
    return getEgammaTrackParticleThinning(tool_prefix, thinning_helper,**kwargs)


def getPhotonTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name', tool_prefix + "PhotonTPThinningTool")
    kwargs.setdefault( 'SGKey','Photons')
    return getEgammaTrackParticleThinning(tool_prefix, thinning_helper,**kwargs)

def getJetTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
    kwargs.setdefault( 'name',                  tool_prefix + "JetTPThinningTool")
    kwargs.setdefault( 'ThinningService',       thinning_helper.ThinningSvc())
    kwargs.setdefault( 'JetKey',                getJetEMTopoName())
    kwargs.setdefault( 'InDetTrackParticlesKey','InDetTrackParticles')

    thinning_tool = DerivationFramework__JetTrackParticleThinning(name  = kwargs.pop('name'),
                                                                  **kwargs)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt4EMTopoTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name',tool_prefix + 'AntiKt4EMTopoJetTPThinningTool')
    kwargs.setdefault( 'JetKey',getJetEMTopoName())
    kwargs.setdefault( 'SelectionString','('+getJetEMTopoName()+'.DFCommonJets_Calib_pt > 15*GeV)')
    return getJetTrackParticleThinning(tool_prefix, thinning_helper, **kwargs)

def getAntiKt4EMPFlowTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name',tool_prefix + 'AntiKt4EMPFlowJetTPThinningTool')
    kwargs.setdefault( 'JetKey',getJetEMPFlowName())
    kwargs.setdefault( 'SelectionString','('+getJetEMPFlowName()+'.pt > 15*GeV)')
    return getJetTrackParticleThinning(tool_prefix, thinning_helper, **kwargs)

def getAntiKt10LCTopoTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name', tool_prefix + 'AntiKt10LCTopoJetTPThinningTool')
    kwargs.setdefault( 'JetKey',         'AntiKt10LCTopoJets')
    kwargs.setdefault( 'SelectionString','(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta)<2.6)')
    kwargs.setdefault( 'ApplyAnd',       False)
    return getJetTrackParticleThinning(tool_prefix, thinning_helper, **kwargs)

def getTCCTrackParticleThinning(tool_prefix, thinning_helper) :
    # Tracks and CaloClusters associated with TCCs
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
    thinning_tool = DerivationFramework__TCCTrackParticleThinning(name                         = tool_prefix + "TCCTPThinningTool",
                                                                  ThinningService              = thinning_helper.ThinningSvc(),
                                                                  JetKey                       = "AntiKt10TrackCaloClusterJets",
                                                                  TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
                                                                  InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                  CaloCalTopoClustersKey       = "CaloCalTopoClusters",
                                                                  ThinOriginCorrectedClusters  = True,
                                                                  SelectionString              = "AntiKt10TrackCaloClusterJets.pt>150*GeV && abs(AntiKt10TrackCaloClusterJets.eta)<2.6",
                                                                  OriginCaloCalTopoClustersKey = "LCOriginTopoClusters")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getUFOTrackParticleThinning(tool_prefix, thinning_helper) :
    # Tracks and CaloClusters associated with UFOs
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UFOTrackParticleThinning
    thinning_tool = DerivationFramework__UFOTrackParticleThinning(name                   = tool_prefix + "UFOCSSKTPThinningTool",
                                                                  ThinningService        = thinning_helper.ThinningSvc(),
                                                                  JetKey                 = "AntiKt10UFOCSSKJets",
                                                                  UFOKey                 = "UFOCSSK",
                                                                  InDetTrackParticlesKey = "InDetTrackParticles",
                                                                  PFOCollectionSGKey     = "JetETMiss",
                                                                  AdditionalPFOKey       = ["CSSK"])

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool


def getTauTrackParticleThinning(tool_prefix, thinning_helper) :
    # Tracks associated with taus
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
    thinning_tool = DerivationFramework__TauTrackParticleThinning(name                   = tool_prefix + "TauTPThinningTool",
                                                                  ThinningService        = thinning_helper.ThinningSvc(),
                                                                  SelectionString        = "TauJets.pt > 18*GeV",
                                                                  TauKey                 = "TauJets",
                                                                  ConeSize               = 0.6,
                                                                  InDetTrackParticlesKey = "InDetTrackParticles")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10UFOCSSKSoftDropBeta100Zcut10Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool =DerivationFramework__GenericObjectThinning( name             = tool_prefix + "UFOJetThinningTool",
                                                               ThinningService  = thinning_helper.ThinningSvc(),
                                                               ContainerName    = "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                                               SelectionString  = "(AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.pt > 150*GeV && abs(AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.eta)<2.6)",
                                                               ApplyAnd         = False)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool


def getTauCaloClusterThinning(tool_prefix, thinning_helper) :
    # calo cluster thinning
    from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
    thinning_tool = DerivationFramework__CaloClusterThinning(name                  = tool_prefix + "TauCCThinningTool",
                                                             ThinningService       = thinning_helper.ThinningSvc(),
                                                             SGKey                 = "TauJets",
                                                             TopoClCollectionSGKey = "CaloCalTopoClusters")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getJetCaloClusterThinning(tool_prefix, thinning_helper, **kwargs) :
    if 'SGKey' not in kwargs :
        raise Exception('getJetCaloClusterThinning called without providing the keyword argument SGKey')

    from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
    kwargs.setdefault('name',                 tool_prefix + 'FatjetCCThinningTool')
    kwargs.setdefault('ThinningService',      thinning_helper.ThinningSvc())
    kwargs.setdefault('TopoClCollectionSGKey','CaloCalTopoClusters')
    kwargs.setdefault('ApplyAnd',             False)
    kwargs.setdefault('AdditionalClustersKey', ["EMOriginTopoClusters","LCOriginTopoClusters","CaloCalTopoClusters"])
    thinning_tool = DerivationFramework__JetCaloClusterThinning(name = kwargs.pop('name'),
                                                                AdditionalClustersKey = kwargs.pop('AdditionalClustersKey'),
                                                                **kwargs)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10LCTopoCaloClusterThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault('name',                 tool_prefix + 'AntiKt10LCTopoCaloClusterThinningTool')
    kwargs.setdefault('SGKey',                'AntiKt10LCTopoJets')
    kwargs.setdefault('SelectionString',      '(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta)<2.6)')
    kwargs.setdefault('AdditionalClustersKey', ["EMOriginTopoClusters","LCOriginTopoClusters","CaloCalTopoClusters"])
    return getJetCaloClusterThinning(tool_prefix, thinning_helper, **kwargs)


def getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool = DerivationFramework__GenericObjectThinning( name             = tool_prefix + "LargeRJetThinningTool",
                                                                ThinningService  = thinning_helper.ThinningSvc(),
                                                                ContainerName    = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                SelectionString  = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.6)",
                                                                ApplyAnd         = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool =DerivationFramework__GenericObjectThinning( name             = tool_prefix + "TCCJetThinningTool",
                                                               ThinningService  = thinning_helper.ThinningSvc(),
                                                               ContainerName    = "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
                                                               SelectionString  = "(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.eta)<2.6)",
                                                               ApplyAnd         = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10TruthTrimmedPtFrac5SmallR20Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool =DerivationFramework__GenericObjectThinning( name             = tool_prefix + "AntiKt10TruthTrimmedPtFrac5SmallR20JetThinningTool",
                                                               ThinningService  = thinning_helper.ThinningSvc(),
                                                               ContainerName    = "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                                               SelectionString  = "(AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt > 150*GeV && abs(AntiKt10TruthTrimmedPtFrac5SmallR20Jets.eta)<2.6)",
                                                               ApplyAnd         = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10TruthWZTrimmedPtFrac5SmallR20Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool =DerivationFramework__GenericObjectThinning( name             = tool_prefix + "AntiKt10TruthWZTrimmedPtFrac5SmallR20JetThinningTool",
                                                               ThinningService  = thinning_helper.ThinningSvc(),
                                                               ContainerName    = "AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets",
                                                               SelectionString  = "(AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets.pt > 150*GeV && abs(AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets.eta)<2.6)",
                                                               ApplyAnd         = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool


def addTrimmedTruthWZJets(sequence, output_group) :
    from DerivationFrameworkJetEtMiss.JetCommon import addTrimmedJets
    addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, mods="groomed", includePreTools=False, algseq=sequence,outputGroup=output_group)

def addJetOutputs(slimhelper,contentlist,smartlist=[],vetolist=[]):
    '''
    copy from  DerivationFrameworkJetEtMiss.JetCommon, which only adds the jet to
    all variables if the jet content is not already listed in the ExtraVariables
    '''
    outputlist = []
    from AthenaCommon import Logging
    dfjetlog = Logging.logging.getLogger('JetCommon')

    from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
    for content in contentlist:
        if content in OutputJets.keys():
            for item in OutputJets[content]:
                if item in vetolist: continue
                outputlist.append(item)
        else:
            outputlist.append(content)

    for item in outputlist:
        if not slimhelper.AppendToDictionary.has_key(item):
            slimhelper.AppendToDictionary[item]='xAOD::JetContainer'
            slimhelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
        if item in smartlist:
            dfjetlog.info( "Add smart jet collection "+item )
            slimhelper.SmartCollections.append(item)
        else :
            head=item + '.'
            add_item=True
            for var in slimhelper.ExtraVariables :
                if len(var) > len(head) :
                    print 'DEBUG HIGG5Commong.addJetOutputs %s == %s ' %(var[0:len(head)], head)
                    if var[0:len(head)] == head :
                        dfjetlog.info( "Add specialised content for jet collection "+item )
                        add_item=False
                        break
            if add_item :
                dfjetlog.info( "Add full jet collection "+item )
                slimhelper.AllVariables.append(item)
"""
def addAntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub(sequence) :
    ExKtJetCollection__FatJet = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
    #  doTrackJet = False
    from AthenaCommon.AppMgr import ToolSvc
    from DerivationFrameworkFlavourTag.HbbCommon import addExKtCoM
    ExCoMJetCollection__SubJet = addExKtCoM(sequence,
                                            ToolSvc,
                                            ExKtJetCollection__FatJet,
                                            2,
                                            doTrackSubJet=False,
                                            doGhostAssoc=False,
                                            ExGhostLabels=["GhostBHadronsFinal","GhostCHadronsFinal"],
                                            min_subjet_pt_mev=0,
                                            subjetAlgName="CoM")

    from BTagging.BTaggingFlags import BTaggingFlags
    BTaggingFlags.CalibrationChannelAliases += [
                   "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo"]
"""
