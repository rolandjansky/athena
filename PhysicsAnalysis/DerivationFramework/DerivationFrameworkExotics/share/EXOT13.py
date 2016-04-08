#********************************************************************
# EXOT13.py 
# reductionConf flag EXOT13 in Reco_tf.py   
#********************************************************************
#from JetRec.JetRecStandard import jtm
#jtm.modifiersMap["groomed"] += [jtm.ktsplitter] # to add substructure to trimmed jets

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT13Stream )
EXOT13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT13Stream.AcceptAlgs(["EXOT13Kernel"])

exot13Seq = CfgMgr.AthSequencer("EXOT13Sequence")

triggerNavChains  = ""
triggerNavChains += "3mu6_msonly|3mu6_msonly_L1MU4_EMPTY|3mu6_msonly_L1MU4_UNPAIRED_ISO|"
triggerNavChains += "mu20_msonly_mu6noL1_msonly_nscan05|mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY|mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO|"
triggerNavChains += "j30_muvtx|j30_muvtx_noiso|j30_muvtx_L1MU4_EMPTY|j30_muvtx_L1MU4_UNPAIRED_ISO|"
triggerNavChains += "j30_jes_PS_llp_L1TAU30|j30_jes_PS_llp_L1TAU40|j30_jes_PS_llp_L1TAU60|j30_jes_PS_llp_L1TAU8_EMPTY|j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO|j30_jes_PS_llp_noiso_L1TAU60|"
triggerNavChains += "g15_loose_2mu10_msonly|g15_loose_2mu10_msonly_L1MU4_EMPTY|g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO|"
triggerNavChains += "g35_medium_g25_medium|g35_medium_g25_medium_L1EM7_EMPTY|g35_medium_g25_medium_L1EM7_UNPAIRED_ISO|"
triggerNavChains += "2mu14|j25|j35|j45|j60|j80|mu26_imedium|mu26|mu40|mu50|e17_lhloose_L1EM15|e60_lhmedium|e24_lhmedium_iloose_L1EM18VH|e24_lhmedium_iloose_L1EM20VH|e24_lhtight_iloose_L1EM20VH|e26_lhtight_iloose|mu20_2mu0noL1_JpsimumuFS|mu18_2mu0noL1_JpsimumuFS|mu20_2mu4_JpsimumuL2|mu18_2mu4_JpsimumuL2"

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT13ThinningHelper = ThinningHelper("EXOT13ThinningHelper")
#EXOT13ThinningHelper.TriggerChains = triggerNavChains
EXOT13ThinningHelper.AppendToStream(EXOT13Stream)

#=======================================
# SKIMMING   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '(EventInfo.eventTypeBitmask==1) || EF_3mu6_MSonly || EF_2mu4_mu11_MSonly || EF_g35_medium_g25 || EF_2mu10_MSonly_g12_loose || EF_2mu14'
if (beamEnergy > 6.0e+06):
    expression = 'L1_3MU6 || HLT_3mu6 || HLT_mu20_imedium_2mu4noL1 || HLT_g35_medium1_g25_medium1 || HLT_g12_loose1_2mu10_msonly || HLT_2mu14 || HLT_j80_xe80'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT13StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT13StringSkimmingTool",
                                                                         expression = expression)

ToolSvc += EXOT13StringSkimmingTool
print EXOT13StringSkimmingTool

#=======================================
# THINNING
#=======================================

thinningTools=[]

# Thin all inner detector tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT13TPThinningTool = DerivationFramework__TrackParticleThinning( name                    =  "EXOT13TPThinningTool",
                                                                    ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                    SelectionString         =  "InDetTrackParticles.pt > 0.5*GeV && InDetTrackParticles.eta > -2.5 && InDetTrackParticles.eta < 2.5",
                                                                    InDetTrackParticlesKey  =  "InDetTrackParticles")
ToolSvc += EXOT13TPThinningTool
thinningTools.append(EXOT13TPThinningTool)

# Thin truth
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT13MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT13MCThinningTool",
                                                             ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                              WritePartons               = False,
                                                              WriteHadrons               = True,
                                                              WriteBHadrons              = True,
                                                              WriteGeant                 = False,
                                                              GeantPhotonPtThresh        = -1.0,
                                                              WriteTauHad                = True,
                                                              PartonPtThresh             = -1.0,
                                                              WriteBSM                   = True,
                                                              WriteBosons                = True,
                                                              WriteBSMProducts           = True,
                                                              WriteTopAndDecays          = True,
                                                              WriteEverything            = False,
                                                              WriteAllLeptons            = True,
                                                              WriteLeptonsNotFromHadrons         = True,
                                                              WriteStatus3               = True,
                                                              WriteFirstN                = -1,
                                                              PreserveDescendants        = False,
                                                              PreserveGeneratorDescendants = True,
                                                              PreserveAncestors = True)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
    ToolSvc += EXOT13MCThinningTool
    thinningTools.append(EXOT13MCThinningTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot13Seq
exot13Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT13Kernel_skim", SkimmingTools = [EXOT13StringSkimmingTool])
exot13Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT13Kernel", ThinningTools = thinningTools)


# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT13ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT13
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT13ContentList import *
EXOT13SlimmingHelper = SlimmingHelper("EXOT13SlimmingHelper")
EXOT13SlimmingHelper.SmartCollections = EXOT13SmartContent 
EXOT13SlimmingHelper.AllVariables = EXOT13AllVariablesContent 
EXOT13SlimmingHelper.StaticContent = EXOT13UnslimmedContent 
EXOT13SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT13SlimmingHelper.IncludeMuonTriggerContent = True
EXOT13SlimmingHelper.IncludeJetTriggerContent = True
EXOT13SlimmingHelper.AppendContentToStream(EXOT13Stream)


