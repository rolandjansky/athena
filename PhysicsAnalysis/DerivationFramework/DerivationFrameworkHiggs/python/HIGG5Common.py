# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# commont content for Hbb DAODs
# will impact content of HIGG5D1, HIGG5D2, HIGG5D3, HIGG2D4

def getHIGG5Common() :
    return [
        "egammaClusters.rawE.phi_sampl.calM",
        "Muons.clusterLink.EnergyLoss.energyLossType",
        "TauJets.IsTruthMatched.truthJetLink.truthParticleLink.ptDetectorAxis.etaDetectorAxis.phiDetectorAxis.mDetectorAxis",
        ("AntiKt4EMTopoJets.TrackWidthPt500.GhostTrackCount.Jvt.JvtJvfcorr.JvtRpt"
           ".JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.DetectorEta"
           ".DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.DFCommonJets_Calib_m"),
        ("AntiKtVR30Rmax4Rmin02TrackJets.-JetConstitScaleMomentum_pt.-JetConstitScaleMomentum_eta.-JetConstitScaleMomentum_phi.-JetConstitScaleMomentum_m"
            ".-constituentLinks.-constituentWeight.-ConstituentScale"),
        "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet",
        "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c10_discriminant",
        "BTagging_AntiKt4EMTopo.MV2cl100_discriminant",
        "CaloCalTopoClusters.CENTER_MAG.calE.calEta.calM.calPhi.calPt.e_sampl.etaCalo.eta_sampl.phiCalo.phi_sampl.rawE.rawEta.rawM.rawPhi",
        "TauChargedParticleFlowObjects.bdtPi0Score.e.eta.m.phi.pt.rapidity"
        ]

def getHIGG5CommonTruth() :
    return [
        "AntiKt4EMTopoJets.ConeTruthLabelID",
        "TruthEvents.PDFID1.PDFID2.PDGID1.PDGID2.Q.X1.X2.XF1.XF2.weights.crossSection.crossSectionError.truthParticleLinks",
        "TruthVertices.barcode.x.y.z.t.id.incomingParticleLinks.outgoingParticleLinks",
         # "TruthParticles.px.py.pz.e.m.decayVtxLink.prodVtxLink.barcode.pdgId.status.TopHadronOriginFlag.classifierParticleOrigin.classifierParticleType.classifierParticleOutCome.dressedPhoton.polarizationTheta.polarizationPhi",
        ("TruthParticles.px.py.pz.e.m.decayVtxLink.prodVtxLink.barcode.pdgId.status.TopHadronOriginFlag"
            ".classifierParticleOrigin.classifierParticleType.classifierParticleOutCome"
            ".dressedPhoton.polarizationPhi.polarizationTheta"
            ".truthOrigin.truthParticleLink.truthType"),
        "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthParticleLink.truthType"
        ]

def filterContentList(pattern, content_list) :
    result=[]
    import re
    pat=re.compile(pattern)
    for elm in content_list :
        head=elm.split('.',2)[0]
        if pat.match(head) :
            result.append(elm)
    return result

# --- common thinning tools
def getTruthThinningTool(tool_prefix, thinning_helper) :
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
    if not DerivationFrameworkIsMonteCarlo :
        return None
    # MC truth thinning (not for data)
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top quark and Bottom quark
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    MCThinningTool = DerivationFramework__GenericTruthThinning(
        name                         = tool_prefix + "MCThinningTool",
        ThinningService              = thinning_helper.ThinningSvc(),
        ParticleSelectionString      = truth_expression,
        PreserveDescendants          = False,
        PreserveGeneratorDescendants = True,
        PreserveAncestors            = True)
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
    kwargs.setdefault( 'JetKey',                'AntiKt4EMTopoJets')
    kwargs.setdefault( 'InDetTrackParticlesKey','InDetTrackParticles')

    thinning_tool = DerivationFramework__JetTrackParticleThinning(name  = kwargs.pop('name'),
                                                                  **kwargs)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt4EMTopoTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name',tool_prefix + 'AntiKt4EMTopoJetTPThinningTool')
    kwargs.setdefault( 'JetKey','AntiKt4EMTopoJets')
    return getJetTrackParticleThinning(tool_prefix, thinning_helper, **kwargs)

def getAntiKt10LCTopoTrackParticleThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault( 'name', tool_prefix + 'AntiKt10LCTopoJetTPThinningTool')
    kwargs.setdefault( 'JetKey',         'AntiKt10LCTopoJets')
    kwargs.setdefault( 'SelectionString','(AntiKt10LCTopoJets.pt > 100*GeV && abs(AntiKt10LCTopoJets.eta)<2.6)')
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
                                                                  SelectionString              = "AntiKt10TrackCaloClusterJets.pt>100*GeV && abs(AntiKt10TrackCaloClusterJets.eta)<2.6",
                                                                  OriginCaloCalTopoClustersKey = "LCOriginTopoClusters")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool


def getTauTrackParticleThinning(tool_prefix, thinning_helper) :
    # Tracks associated with taus
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
    thinning_tool = DerivationFramework__TauTrackParticleThinning(name                   = tool_prefix + "TauTPThinningTool",
                                                                  ThinningService        = thinning_helper.ThinningSvc(),
                                                                  TauKey                 = "TauJets",
                                                                  ConeSize               = 0.6,
                                                                  InDetTrackParticlesKey = "InDetTrackParticles")
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
    thinning_tool = DerivationFramework__JetCaloClusterThinning(name                  = kwargs.pop('name'), **kwargs)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10LCTopoCaloClusterThinning(tool_prefix, thinning_helper, **kwargs) :
    kwargs.setdefault('name',                 tool_prefix + 'AntiKt10LCTopoCaloClusterThinningTool')
    kwargs.setdefault('SGKey',                'AntiKt10LCTopoJets')
    kwargs.setdefault('SelectionString',      '(AntiKt10LCTopoJets.pt > 100*GeV && abs(AntiKt10LCTopoJets.eta)<2.6)')
    return getJetCaloClusterThinning(tool_prefix, thinning_helper, **kwargs)

def getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool = DerivationFramework__GenericObjectThinning( name             = tool_prefix + "LargeRJetThinningTool",
                                                                ThinningService  = thinning_helper.ThinningSvc(),
                                                                ContainerName    = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                SelectionString  = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 100*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.6)",
                                                                ApplyAnd         = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning(tool_prefix, thinning_helper) :
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    thinning_tool =DerivationFramework__GenericObjectThinning( name             = tool_prefix + "TCCJetThinningTool",
                                                               ThinningService  = thinning_helper.ThinningSvc(),
                                                               ContainerName    = "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
                                                               SelectionString  = "(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt > 100*GeV && abs(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.eta)<2.6)",
                                                               ApplyAnd         = False)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+= thinning_tool
    return thinning_tool

def addTrimmedTruthWZJets(sequence, output_group) :
    from DerivationFrameworkJetEtMiss.JetCommon import addTrimmedJets
    addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, mods="groomed", includePreTools=False, algseq=sequence,outputGroup=output_group)
