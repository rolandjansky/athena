# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#********************************************************************
# SUSY6.py
# Derivation merger of old SUSY6 and SUSY8, for dE/dx, DT and SMP
#********************************************************************

from AthenaCommon import Logging
susy6log = Logging.logging.getLogger('SUSY6')

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkTau.TauCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from RecExConfig.InputFilePeeker import inputFileSummary

### Set up stream
streamName  = derivationFlags.WriteDAOD_SUSY6Stream.StreamName
fileName    = buildFileName(derivationFlags.WriteDAOD_SUSY6Stream)
SUSY6Stream = MSMgr.NewPoolRootStream(streamName, fileName)
SUSY6Stream.AcceptAlgs(["SUSY6KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY6ThinningHelper = ThinningHelper("SUSY6ThinningHelper")
thinningTools       = []
AugmentationTools   = []
DecorationTools     = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY6 = CfgMgr.AthSequencer("SeqSUSY6")
DerivationFrameworkJob += SeqSUSY6

### Toggle soft-pion outputs and thinning
doSoftPion = False
if 'tag_info' in inputFileSummary:
  if 'AMITag' in inputFileSummary['tag_info']:
    AMITag = inputFileSummary['tag_info']['AMITag']
    if 'r11571' in AMITag or 'r11620' in AMITag:
      doSoftPion = True

#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY6TriggerList import *

# SUSY6 allTriggers = SUSY6DiMuonTriggers + SUSY6SingleEleTriggers + SUSY6SingleMuTriggers
# QUESTION why only list these triggers here and not all relevant ones
allTriggers = SUSY6METTriggers + SUSY6JetMETTriggers + SUSY6SingleEleTriggers + SUSY6SingleMuTriggers + SUSY6DiMuonTriggers + SUSY6LateMuonTriggers
# susy6log.info("SUSY6TriggerList: {!s}".format(allTriggers))

SUSY6ThinningHelper.TriggerChains = '|'.join(allTriggers)
SUSY6ThinningHelper.AppendToStream( SUSY6Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================

if doSoftPion:

  from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
  InDetTrackletSelectionTool = InDet__InDetTrackSelectionTool(name           = "InDetTrackletSelectionTool",
                                                             CutLevel        = "NoCut", 
                                                             minPt           = 5000.0,
                                                             maxD0           = 10.0,
                                                             maxZ0SinTheta   = 10.0 )

  ToolSvc += InDetTrackletSelectionTool 

  # Vertex thinning; keep associated tracks
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__VertexParticleThinning
  vertexParticleThinningTool = DerivationFramework__VertexParticleThinning(name                   = "VertexParticleThinning",
                                                                           ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                           TrackSelectionTool     = InDetTrackletSelectionTool,
                                                                           MinGoodTracks          = 1,
                                                                           VertexKey              = "VrtSecDecay",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles" )

  ToolSvc += vertexParticleThinningTool
  thinningTools.append(vertexParticleThinningTool)

  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
  SUSY6TrackletThinningTool = DerivationFramework__TrackParticleThinning(name                   = "SUSY6TrackletThinningTool",
                                                                         ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                         TrackSelectionTool     = InDetTrackletSelectionTool,
                                                                         InDetTrackParticlesKey = "InDetPixelPrdAssociationTrackParticles")

  ToolSvc += SUSY6TrackletThinningTool
  thinningTools.append(SUSY6TrackletThinningTool)


# Dedicated track-thinning for pixel dE/dx calibration using pT < 10 GeV tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__PixeldEdxTrackParticleThinning
pixeldEdxThinningTool = DerivationFramework__PixeldEdxTrackParticleThinning(name                   = "PixeldEdxTrackParticleThinning",
                                                                              ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                              InDetTrackParticlesKey = "InDetTrackParticles" )
ToolSvc += pixeldEdxThinningTool
thinningTools.append(pixeldEdxThinningTool)

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY6TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "SUSY6TPThinningTool",
                                                                 ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                 SelectionString        = "InDetTrackParticles.pt > 10*GeV",
                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY6TPThinningTool
thinningTools.append(SUSY6TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "SUSY6MuonTPThinningTool",
                                                                         ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY6MuonTPThinningTool
thinningTools.append(SUSY6MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "SUSY6ElectronTPThinningTool",
                                                                               ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                               SGKey             	  = "Electrons",
                                                                               InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY6ElectronTPThinningTool
thinningTools.append(SUSY6ElectronTPThinningTool)

# Photon thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY6PhotonThinningTool = DerivationFramework__GenericObjectThinning(name            = "SUSY6PhotonThinningTool",
                                                                     ThinningService = SUSY6ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "Photons",
                                                                     ApplyAnd        = False,
                                                                     SelectionString = "Photons.pt > 10*GeV")
ToolSvc += SUSY6PhotonThinningTool
thinningTools.append(SUSY6PhotonThinningTool)

# TauJets thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY6TauThinningTool = DerivationFramework__GenericObjectThinning(name            = "SUSY6TauThinningTool",
                                                                  ThinningService = SUSY6ThinningHelper.ThinningSvc(),
                                                                  ContainerName   = "TauJets",
                                                                  SelectionString = "TauJets.ptFinalCalib > 20*GeV")
ToolSvc += SUSY6TauThinningTool
thinningTools.append(SUSY6TauThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "SUSY6TauTPThinningTool",
                                                                       ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       SelectionString        = "TauJets.ptFinalCalib > 20*GeV",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY6TauTPThinningTool
thinningTools.append(SUSY6TauTPThinningTool)

# TauTracks thinning
from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TauTracksThinning
SUSY6TauTracksThinningTool = DerivationFramework__TauTracksThinning(name            = "SUSY6TauTracksThinningTool",
                                                                    ThinningService = SUSY6ThinningHelper.ThinningSvc(),
                                                                    TauKey          = "TauJets",
                                                                    SelectionString = "TauJets.ptFinalCalib > 20*GeV",
                                                                    TauTracksKey    = "TauTracks")
ToolSvc += SUSY6TauTracksThinningTool
thinningTools.append(SUSY6TauTracksThinningTool)

# Forward track thinning (remove everything)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
forwardTrackThinner = DerivationFramework__TrackParticleThinning(name                   = "forwardTrackThinner",
                                                                 ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                 SelectionString        = "InDetForwardTrackParticles.pt < 0.*GeV",
                                                                 InDetTrackParticlesKey = "InDetForwardTrackParticles",
                                                                 ApplyAnd               = True)
ToolSvc += forwardTrackThinner
thinningTools.append(forwardTrackThinner)

# QUESTION in SUSY8 we had a JetTrackParticleThinning tools, not sure we need that
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
# SUSY6JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "SUSY6JetTPThinningTool",
#                                                                        ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
#                                                                        JetKey                 = "AntiKt4EMTopoJets",
#                                                                        InDetTrackParticlesKey = "InDetTrackParticles",
#                                                                        ApplyAnd               = True)
# ToolSvc += SUSY6JetTPThinningTool
# thinningTools.append(SUSY6JetTPThinningTool)

#==========================================================================================
# ISOLATION DECORATING ( copied from DerivationFrameworkMuons/TrackIsolationDecorator.py )
#==========================================================================================

from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta = 3.
TrackIsoTool.TrackSelectionTool.minPt = 1000.
TrackIsoTool.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += TrackIsoTool

TrackIsoToolPdEdx = xAOD__TrackIsolationTool("TrackIsoToolPdEdx")
TrackIsoToolPdEdx.TrackSelectionTool.maxZ0SinTheta = 3.
TrackIsoToolPdEdx.TrackSelectionTool.maxD0 = 2.
TrackIsoToolPdEdx.TrackSelectionTool.minPt = 1000.
TrackIsoToolPdEdx.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += TrackIsoToolPdEdx

TrackIsoToolPdEdxTight = xAOD__TrackIsolationTool("TrackIsoToolPdEdxTight")
TrackIsoToolPdEdxTight.TrackSelectionTool.maxZ0SinTheta = 0.5
TrackIsoToolPdEdxTight.TrackSelectionTool.maxD0 = 0.5
TrackIsoToolPdEdxTight.TrackSelectionTool.minPt = 1000.
TrackIsoToolPdEdxTight.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += TrackIsoToolPdEdxTight

if DerivationFrameworkIsDataOverlay:
    raise RuntimeError('Not sure how to run over overlay data for SUSY6')
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
IsoCorrectionTool = CP__IsolationCorrectionTool("NewLeakageCorrTool", IsMC = DerivationFrameworkHasTruth)
ToolSvc += IsoCorrectionTool

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
MyCaloClustersInConeTool = xAOD__CaloClustersInConeTool("MyCaloClustersInConeTool", CaloClusterLocation = "CaloCalTopoClusters")
ToolSvc += MyCaloClustersInConeTool

from CaloIdentifier import SUBCALO

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
CaloIsoTool = xAOD__CaloIsolationTool("CaloIsoTool")
CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.NewLeakageCorrTool
CaloIsoTool.ClustersInConeTool = ToolSvc.MyCaloClustersInConeTool
CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
CaloIsoTool.UseEMScale  = True
CaloIsoTool.UseCaloExtensionCaching = False
CaloIsoTool.saveOnlyRequestedCorrections = True
CaloIsoTool.addCaloExtensionDecoration = False
ToolSvc += CaloIsoTool

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# Calculate ptcone&ptvarcone, etcone&topoetcone
deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone30, isoPar.topoetcone20]
deco_prefix = ''  #'SUSY6_'

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParametersKVU
SUSY6DFCommonKVU = DerivationFramework__TrackParametersKVU(name                       = "SUSY6DFCommonKVU",
                                                           TrackParticleContainerName = "InDetPixelPrdAssociationTrackParticles",
                                                           VertexContainerName        = "PrimaryVertices")
ToolSvc += SUSY6DFCommonKVU
DecorationTools.append(SUSY6DFCommonKVU)

SUSY6DFThreeKVU = DerivationFramework__TrackParametersKVU(name                       = "SUSY6DFThreeKVU",
                                                          TrackParticleContainerName = "InDetPixelThreeLayerTrackParticles",
                                                          VertexContainerName        = "PrimaryVertices")
ToolSvc += SUSY6DFThreeKVU
DecorationTools.append(SUSY6DFThreeKVU)

SUSY6DFFourKVU = DerivationFramework__TrackParametersKVU(name                       = "SUSY6DFFourKVU",
                                                         TrackParticleContainerName = "InDetPixelFourLayerTrackParticles",
                                                         VertexContainerName        = "PrimaryVertices")
ToolSvc += SUSY6DFFourKVU
DecorationTools.append(SUSY6DFFourKVU)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__CaloIsolationDecorator
SUSY6IDTrackDecorator = DerivationFramework__CaloIsolationDecorator(name               = "SUSY6IDTrackDecorator",
                                                                    TrackIsolationTool = TrackIsoTool,
                                                                    CaloIsolationTool  = CaloIsoTool,
                                                                    TargetContainer    = "InDetTrackParticles",
                                                                    SelectionString    = "InDetTrackParticles.pt>10*GeV",
                                                                    ptcones            = deco_ptcones,
                                                                    topoetcones        = deco_topoetcones,
                                                                    Prefix             = deco_prefix)
ToolSvc += SUSY6IDTrackDecorator
DecorationTools.append(SUSY6IDTrackDecorator)

SUSY6PixelTrackDecorator = DerivationFramework__CaloIsolationDecorator(name               = "SUSY6PixelTrackDecorator",
                                                                       TrackIsolationTool = TrackIsoTool,
                                                                       CaloIsolationTool  = CaloIsoTool,
                                                                       TargetContainer    = "InDetPixelPrdAssociationTrackParticles",
                                                                       SelectionString    = "InDetPixelPrdAssociationTrackParticles.pt>10*GeV",
                                                                       ptcones            = deco_ptcones,
                                                                       topoetcones        = deco_topoetcones,
                                                                       Prefix             = deco_prefix)
ToolSvc += SUSY6PixelTrackDecorator
DecorationTools.append(SUSY6PixelTrackDecorator)

SUSY6ThreeLayerTrackDecorator = DerivationFramework__CaloIsolationDecorator(name               = "SUSY6ThreeLayerTrackDecorator",
                                                                            TrackIsolationTool = TrackIsoTool,
                                                                            CaloIsolationTool  = CaloIsoTool,
                                                                            TargetContainer    = "InDetPixelThreeLayerTrackParticles",
                                                                            ptcones            = deco_ptcones,
                                                                            topoetcones        = deco_topoetcones,
                                                                            Prefix             = deco_prefix)
ToolSvc += SUSY6ThreeLayerTrackDecorator
DecorationTools.append(SUSY6ThreeLayerTrackDecorator)

SUSY6FourLayerTrackDecorator = DerivationFramework__CaloIsolationDecorator(name               = "SUSY6FourLayerTrackDecorator",
                                                                           TrackIsolationTool = TrackIsoTool,
                                                                           CaloIsolationTool  = CaloIsoTool,
                                                                           TargetContainer    = "InDetPixelFourLayerTrackParticles",
                                                                           ptcones            = deco_ptcones,
                                                                           topoetcones        = deco_topoetcones,
                                                                           Prefix             = deco_prefix)
ToolSvc += SUSY6FourLayerTrackDecorator
DecorationTools.append(SUSY6FourLayerTrackDecorator)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
SUSY6IsoTrackDecoratorPdEdx = DerivationFramework__trackIsolationDecorator(name               = "SUSY6TrackIsoPdEdxIDDecorator",
                                                                           TrackIsolationTool = TrackIsoToolPdEdx,
                                                                           TargetContainer    = "InDetTrackParticles",
                                                                           ptcones            = [isoPar.ptcone30,isoPar.ptcone20],
                                                                           Prefix             = 'TrkIsoPtPdEdx_')
ToolSvc += SUSY6IsoTrackDecoratorPdEdx
DecorationTools.append(SUSY6IsoTrackDecoratorPdEdx)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
SUSY6IsoTrackletDecoratorPdEdx = DerivationFramework__trackIsolationDecorator(name               = "SUSY6TrackIsoPdEdxPixelDecorator",
                                                                              TrackIsolationTool = TrackIsoToolPdEdx,
                                                                              TargetContainer    = "InDetPixelPrdAssociationTrackParticles",
                                                                              ptcones            = [isoPar.ptcone30,isoPar.ptcone20],
                                                                              Prefix             = 'TrkIsoPtPdEdx_')
ToolSvc += SUSY6IsoTrackletDecoratorPdEdx
DecorationTools.append(SUSY6IsoTrackletDecoratorPdEdx)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
SUSY6IsoTightTrackDecoratorPdEdx = DerivationFramework__trackIsolationDecorator(name               = "SUSY6TrackIsoTightPdEdxIDDecorator",
                                                                                TrackIsolationTool = TrackIsoToolPdEdxTight,
                                                                                TargetContainer    = "InDetTrackParticles",
                                                                                ptcones            = [isoPar.ptcone30,isoPar.ptcone20],
                                                                                Prefix             = 'TrkIsoPtTightPdEdx_')
ToolSvc += SUSY6IsoTightTrackDecoratorPdEdx
DecorationTools.append(SUSY6IsoTightTrackDecoratorPdEdx)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
SUSY6IsoTrackletTightDecoratorPdEdx = DerivationFramework__trackIsolationDecorator(name               = "SUSY6TrackIsoTightPdEdxPixelDecorator",
                                                                                   TrackIsolationTool = TrackIsoToolPdEdxTight,
                                                                                   TargetContainer    = "InDetPixelPrdAssociationTrackParticles",
                                                                                   ptcones            = [isoPar.ptcone30,isoPar.ptcone20],
                                                                                   Prefix             = 'TrkIsoPtTightPdEdx_')
ToolSvc += SUSY6IsoTrackletTightDecoratorPdEdx
DecorationTools.append(SUSY6IsoTrackletTightDecoratorPdEdx)

#====================================================================
# TrackParticleCaloCellDecoration
#====================================================================

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParticleCaloCellDecorator
SUSY6_TrackParticleCaloCellDecorator = DerivationFramework__TrackParticleCaloCellDecorator(name             = "SUSY6_TrackParticleCaloCellDecorator",
                                                                                           DecorationPrefix = "SUSY6",
                                                                                           ContainerName    = "InDetTrackParticles")

ToolSvc += SUSY6_TrackParticleCaloCellDecorator
AugmentationTools.append(SUSY6_TrackParticleCaloCellDecorator)

#====================================================================
# TRUTH THINNING
#====================================================================

if DerivationFrameworkHasTruth:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY6TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY6TruthThinningTool",
                                                       ThinningService              = SUSY6ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = False, 
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = True, 
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = True,
                                                       WriteBosons                  = True,
                                                       WriteBosonProducts           = True, 
                                                       WriteBSMProducts             = True,
                                                       WriteTopAndDecays            = True, 
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = False,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True, 
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)

  ToolSvc += SUSY6TruthThinningTool
  thinningTools.append(SUSY6TruthThinningTool)

  truth_expression = '(((abs(TruthParticles.pdgId) >= 1000000) && (abs(TruthParticles.pdgId) <= 1000040)) || ((abs(TruthParticles.pdgId) >= 2000000) && (abs(TruthParticles.pdgId) <= 2000040)))'
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
  SUSY6TruthSUSYThinningTool = DerivationFramework__GenericTruthThinning( name                         = "SUSY6TruthSUSYThinningTool",
                                                                          ThinningService              = SUSY6ThinningHelper.ThinningSvc(),
                                                                          ParticleSelectionString      = truth_expression,
                                                                          PreserveDescendants          = True,
                                                                          PreserveGeneratorDescendants = False,
                                                                          SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset
                                                                         )
  ToolSvc += SUSY6TruthSUSYThinningTool
  thinningTools.append(SUSY6TruthSUSYThinningTool)

        
#====================================================================
# SKIMMING TOOL 
#====================================================================

# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
allTriggers = SUSY6METTriggers + SUSY6JetMETTriggers + SUSY6SingleMuTriggers + SUSY6DiMuonTriggers + SUSY6LateMuonTriggers
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY6TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name          = "SUSY6TriggerSkimmingTool",
                                                                    TriggerListOR = allTriggers)
ToolSvc += SUSY6TriggerSkimmingTool

dimuonTriggers       = SUSY6DiMuonTriggers
jetxeTriggers        = SUSY6JetMETTriggers
singleEleTriggers    = SUSY6SingleEleTriggers
singleMuTriggers     = SUSY6SingleMuTriggers
xeTriggers           = SUSY6METTriggers

stdTrackRequirements = ' ( InDetTrackParticles.pt >= 10*GeV ) && ( ( InDetTrackParticles.ptvarcone20 / InDetTrackParticles.pt ) < 0.2 )'
pixTrackRequirements = ' ( InDetPixelPrdAssociationTrackParticles.pt >= 10*GeV ) && ( ( InDetPixelPrdAssociationTrackParticles.ptvarcone20 / InDetPixelPrdAssociationTrackParticles.pt ) < 0.2 ) '
trackExpression='( count('+stdTrackRequirements+') + count('+pixTrackRequirements+')>= 2 )'

electronRequirements = '(Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium) && (Electrons.ptvarcone20 / Electrons.pt < 0.15) && (Electrons.topoetcone20 / Electrons.pt < 0.2)'
# muonRequirements = '(Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection) && (Muons.ptvarcone30 / Muons.pt < 0.15) && (Muons.topoetcone20 / Muons.pt < 0.3)'
electronExpression = '( count('+electronRequirements+') >= 1 ) && ( ' + ' || '.join(singleEleTriggers) + ') && ' + trackExpression
# muonExpression     = '( count('+muonRequirements+') >= 1 ) && (' + ' || '.join(singleMuTriggers) + ') && ' + trackExpression

# ZllRequirements =' ( ' + electronExpression + ' ) || ( ' + muonExpression + ' ) '
ZllRequirements =' ( ' + electronExpression + ' ) '

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY6ZllSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name       = "SUSY6ZllSkimmingTool",
                                                                   expression = ZllRequirements)
ToolSvc += SUSY6ZllSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
SUSY6SkimmingORTool = DerivationFramework__FilterCombinationOR(name       = "SUSY6SkimmingORTool",
                                                               FilterList = [SUSY6ZllSkimmingTool, SUSY6TriggerSkimmingTool])
ToolSvc += SUSY6SkimmingORTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================

SeqSUSY6 += CfgMgr.DerivationFramework__DerivationKernel("SUSY6KernelSkim",
                                                         AugmentationTools = DecorationTools,
                                                         SkimmingTools = [SUSY6SkimmingORTool])

#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY6)


## Adding decorations for fJVT PFlow jets                                                                                                                                                                   
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY6, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY6, algname='JetForwardJvtToolBDTAlg')
#==============================================================================
# Augment after skim
#==============================================================================

SeqSUSY6 += CfgMgr.DerivationFramework__DerivationKernel("SUSY6KernelAug",
                                                         AugmentationTools = AugmentationTools,
                                                         ThinningTools     = thinningTools)

#====================================================================
# CONTENT LIST  
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSUSY.SUSY6ContentList import *

SUSY6SlimmingHelper = SlimmingHelper("SUSY6SlimmingHelper")
SUSY6SlimmingHelper.SmartCollections = [
  "Electrons",
  "Photons",
  "Muons",
  "AntiKt4EMTopoJets",
  "AntiKt4EMPFlowJets",
  "PrimaryVertices",
  "MET_Reference_AntiKt4EMTopo",
  "MET_Reference_AntiKt4EMPFlow",
  "InDetTrackParticles",
  "TauJets",
  "AntiKt4EMPFlowJets_BTagging201810",
  "AntiKt4EMPFlowJets_BTagging201903",
  "BTagging_AntiKt4EMPFlow_201810",
  "BTagging_AntiKt4EMPFlow_201903",
  "AntiKt4EMTopoJets_BTagging201810",
  "BTagging_AntiKt4EMTopo_201810"
]

SUSY6SlimmingHelper.AllVariables = [
  # "CaloCalTopoClusters",
  "CombinedMuonTrackParticles", 
  "CombinedStauTrackParticles",
  # "ExtrapolatedMuonTrackParticles",
  "InDetPixelPrdAssociationTrackParticles",
  "InDetPixelThreeLayerTrackParticles",
  "InDetPixelFourLayerTrackParticles",
  # "InDetTrackParticlesAssociatedClusters",
  # "InDetTrackParticlesClusterAssociations",
  "METAssoc_AntiKt4EMTopo",
  "MET_Calo",
  "MET_Core_AntiKt4EMTopo",
  "MET_EMTopo",
  "MET_LocHadTopo",
  "MET_Track",
  "MET_Truth",
  "MSonlyTracklets",
  "MuonSegments",
  "MuonSpectrometerTrackParticles",
  # "Muons",
  "SlowMuons",
  "Staus",
  "TruthEvents",
  "TruthParticles",
  "TruthVertices"
]

if doSoftPion:
  SUSY6SlimmingHelper.AllVariables += ['VrtSecDecay']

SUSY6SlimmingHelper.ExtraVariables = SUSY6ExtraVariables

SUSY6SlimmingHelper.IncludeMuonTriggerContent = True
SUSY6SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY6SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY6SlimmingHelper.IncludeJetTriggerContent = False
# QUESTION IncludeJetTriggerContent was on True for SUSY8, but might not have to be
SUSY6SlimmingHelper.IncludeTauTriggerContent = True
SUSY6SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY6SlimmingHelper.IncludeBJetTriggerContent = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkHasTruth:
  SUSY6SlimmingHelper.AppendToDictionary = {
    'TruthTop':'xAOD::TruthParticleContainer',
    'TruthTopAux':'xAOD::TruthParticleAuxContainer',
    'TruthBSM':'xAOD::TruthParticleContainer',
    'TruthBSMAux':'xAOD::TruthParticleAuxContainer',
    'TruthBoson':'xAOD::TruthParticleContainer',
    'TruthBosonAux':'xAOD::TruthParticleAuxContainer'
  }
  
  SUSY6SlimmingHelper.AllVariables += [
    "TruthElectrons",
    "TruthMuons",
    "TruthTaus",
    "TruthPhotons",
    "TruthNeutrinos",
    "TruthTop",
    "TruthBSM",
    "TruthBoson"
  ]

SUSY6SlimmingHelper.AppendContentToStream(SUSY6Stream)
