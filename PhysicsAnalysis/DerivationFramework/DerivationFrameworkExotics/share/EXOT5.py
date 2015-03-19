#********************************************************************
# EXOT5.py
# reductionConf flag EXOT5 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags

IS_SIMULATION = globalflags.DataSource() == 'geant4'

#====================================================================
# THINNING TOOLS
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import (
    DerivationFramework__MuonTrackParticleThinning,
    DerivationFramework__EgammaTrackParticleThinning,
    DerivationFramework__JetTrackParticleThinning)

thinningTools = []
# Keep tracks associated with muons
EXOT5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                    = 'EXOT5MuonTPThinningTool',
    ThinningService         = 'EXOT5ThinningSvc',
    MuonKey                 = 'Muons',
    InDetTrackParticlesKey  = 'InDetTrackParticles',
    ConeSize                = 0.3)
ToolSvc += EXOT5MuonTPThinningTool
thinningTools.append(EXOT5MuonTPThinningTool)

# Keep tracks associated with electrons
EXOT5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
    name                    = 'EXOT5ElectronTPThinningTool',
    ThinningService         = 'EXOT5ThinningSvc',
    SGKey             	    = 'Electrons',
    GSFTrackParticlesKey    = 'GSFTrackParticles',
    InDetTrackParticlesKey  = 'InDetTrackParticles',
    #SelectionString         = 'Electrons.pt > 10*GeV',
    ConeSize                = 0.3)
ToolSvc += EXOT5ElectronTPThinningTool
thinningTools.append(EXOT5ElectronTPThinningTool)

# Keep tracks associated with jets
"""
EXOT5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
    name                    = 'EXOT5JetTPThinningTool',
    ThinningService         = 'EXOT5ThinningSvc',
    JetKey                  = 'AntiKt4LCTopoJets',
    InDetTrackParticlesKey  = 'InDetTrackParticles',
    SelectionString         = 'AntiKt4LCTopoJets.pt > 20*GeV')
ToolSvc += EXOT5JetTPThinningTool
thinningTools.append(EXOT5JetTPThinningTool)
"""

# Cluster thinning
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import \
    DerivationFramework__CaloClusterThinning

# Keep clusters associated to electrons
EXOT5ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'EXOT5ElectronCCThinningTool',
    ThinningService         = 'EXOT5ThinningSvc',
    SGKey             	    = 'Electrons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    #SelectionString         = 'Electrons.pt > 10*GeV',
    #FrwdClCollectionSGKey   = 'LArClusterEMFrwd',
    ConeSize                = 0.4)
ToolSvc += EXOT5ElectronCCThinningTool
thinningTools.append(EXOT5ElectronCCThinningTool)

if IS_SIMULATION:
    # MC Thinning
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import \
        DerivationFramework__MenuTruthThinning

    EXOT5MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                       = 'EXOT5MCThinningTool',
        ThinningService            = 'EXOT5ThinningSvc',
        WriteEverything            = False,
        WritePartons               = False,
        PartonPtThresh             = -1.0,
        WriteHadrons               = False,
        WriteBHadrons              = False,
        WriteGeant                 = False,
        GeantPhotonPtThresh        = -1.0,
        WriteTauHad                = True,
        WriteBSM                   = True,
        WriteBosons                = True,
        WriteBSMProducts           = True,
        WriteTopAndDecays          = True,
        WriteAllLeptons            = True,
        #WriteLeptonsNotFromHadrons = True,
        WriteStatus3               = False,
        WriteFirstN                = -1,
        #PreserveDescendants        = False,
        )
    ToolSvc += EXOT5MCThinningTool
    thinningTools.append(EXOT5MCThinningTool)

#====================================================================
# SKIMMING TOOL
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import \
    DerivationFramework__xAODStringSkimmingTool

skimmingTools = []
beamEnergy = jobproperties.Beam.energy()
if (beamEnergy < 4.1e+06):
    expression = (
        '((EventInfo.eventTypeBitmask==1) || '
        '(EF_xe80_tclcw || EF_xe80_tclcw_loose || '
        'EF_e24vhi_medium1 || EF_e60_medium1 || '
        'EF_mu24i_tight || EF_mu36_tight)) && '
        '(count(AntiKt4LCTopoJets.pt > 100*GeV) >= 1)')
else:
    expression = (
        '(HLT_xe100 || HLT_j30_xe10_razor170 || '
        'HLT_e28_tight_iloose || HLT_e60_medium || '
        'HLT_mu26_imedium || HLT_mu50) && '
        '(count(AntiKt4LCTopoJets.pt > 100*GeV) >= 1)')

EXOT5StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name='EXOT5StringSkimmingTool',
    expression=expression)
ToolSvc += EXOT5StringSkimmingTool
skimmingTools.append(EXOT5StringSkimmingTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import \
    DerivationFramework__DerivationKernel

DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    'EXOT5Kernel', SkimmingTools=skimmingTools, ThinningTools=thinningTools)

#====================================================================
# SET UP STREAM
#====================================================================
stream_name = derivationFlags.WriteDAOD_EXOT5Stream.StreamName
file_name = buildFileName(derivationFlags.WriteDAOD_EXOT5Stream)
EXOT5Stream = MSMgr.NewPoolRootStream(stream_name, file_name)
EXOT5Stream.AcceptAlgs(['EXOT5Kernel'])

# SPECIAL SETUP FOR THINNING
# Thinning service name must match the one passed to the thinning tools above
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
aug_stream = MSMgr.GetStream(stream_name)
evt_stream = aug_stream.GetEventStream()
svcMgr += createThinningSvc(svcName='EXOT5ThinningSvc',
                            outStreams=[evt_stream])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

EXOT5SlimmingHelper = SlimmingHelper('EXOT5SlimmingHelper')
EXOT5SlimmingHelper.SmartCollections = [
    'Electrons',
    'Muons',
    #'Photons',
    'InDetTrackParticles',
    'PrimaryVertices',
    ]
EXOT5SlimmingHelper.ExtraVariables = [
    'Electrons.author.Medium.Tight.Loose.charge',
    'Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40',
    'CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix', # SUSYTools
    'ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix', # SUSYTools
    ]
EXOT5SlimmingHelper.AllVariables = [
    'AntiKt4LCTopoJets',
    #'AntiKt4TruthJets',
    'BTagging_AntiKt4LCTopo',
    #'BTagging_AntiKt4Truth',
    'MET_Reference_AntiKt4LCTopo',
    'METAssoc_AntiKt4LCTopo',
    'MET_Core_AntiKt4LCTopo',
    'MET_Truth',
    'TruthEvents',
    #'TruthVertex', # MenuTruthThinning does not keep vertices
    'TruthParticles',
    'CaloCalTopoClusters',
    ]
EXOT5SlimmingHelper.UserContent = []
EXOT5SlimmingHelper.AppendContentToStream(EXOT5Stream)
