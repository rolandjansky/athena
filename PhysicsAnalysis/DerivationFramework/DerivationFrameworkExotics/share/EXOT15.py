#********************************************************************
# EXOT15.py 
# reductionConf flag EXOT15 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

exot15Seq = CfgMgr.AthSequencer("EXOT15Sequence")


#====================================================================
# THINNING TOOLS
#====================================================================

thinning_expression = "(InDetTrackParticles.pt > 1.0*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 3)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT15TPThinningTool = DerivationFramework__TrackParticleThinning( name              = "EXOT15TPThinningTool",
                                                                ThinningService         = "EXOT15ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
#ToolSvc += EXOT15TPThinningTool

#=======================================
# SKIMMING   
#=======================================

skimmingTools = []

SkipTriggerRequirement=(globalflags.DataSource()=='geant4') #apply triggers only to data; include events with 1 jet only in MC
if SkipTriggerRequirement:
    topology_selection = "( (count (abs(AntiKt4EMTopoJets.eta) < 2.8 && AntiKt4EMTopoJets.pt > 20) > 0) || (count (abs(MSDisplacedVertex.z) >= 0) > 0) )"
    print "vtx+jet topo selection: ", topology_selection
else: 
    topology_selection = "((count (abs(MSDisplacedVertex.z) >= 0) > 0) )"
    print "vtx topo selection:",  topology_selection

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = "(EventInfo.eventTypeBitmask==1)" + "||" + topology_selection
if (beamEnergy > 6.0e+06):
    expression = topology_selection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT15SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT15SkimmingTool",
                                                                      expression = expression)
 
ToolSvc += EXOT15SkimmingTool
skimmingTools.append(EXOT15SkimmingTool)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
if (beamEnergy > 6.0e+06):
    #SkipTriggerRequirement=(globalflags.DataSource()=='geant4') #apply triggers only to data, not to MC
    if  SkipTriggerRequirement:
        print "trigger disabled"
    else:
        print "trigger enabled"
        EXOT15TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT15TriggerSkimmingTool",
                                                                             TriggerListAND = [],
                                                                             TriggerListOR  = ["HLT_j30_jes_PS_llp_L1TAU30",
                                                                                               "HLT_j30_jes_PS_llp_L1TAU60",
                                                                                               "HLT_j30_jes_PS_llp_noiso_L1TAU60",
                                                                                               "HLT_j30_muvtx","HLT_j30_muvtx_noiso",
                                                                                               "HLT_j100",
                                                                                               "HLT_j400",
                                                                                               "HLT_j30_jes_PS_llp_L1LLP-NOMATCH",
                                                                                               "HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH",
                                                                                               "HLT_j30_jes_PS_llp_L1TAU40",
                                                                                               "HLT_j30_jes_PS_llp_L1TAU8_EMPTY",
                                                                                               "HLT_j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO",
                                                                                               "HLT_j30_muvtx_L1MU4_EMPTY",
                                                                                               "HLT_j30_muvtx_L1MU4_UNPAIRED_ISO"])
        

        ToolSvc += EXOT15TriggerSkimmingTool
        skimmingTools.append(EXOT15TriggerSkimmingTool)

    if not SkipTriggerRequirement:
        from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
        EXOT15ORSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT15ORSkimmingTool", FilterList = skimmingTools )
        ToolSvc += EXOT15ORSkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot15Seq
if SkipTriggerRequirement:
    exot15Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT15Kernel", SkimmingTools = [EXOT15SkimmingTool])
else:
    exot15Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT15Kernel", SkimmingTools = [EXOT15ORSkimmingTool])
#exot15Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT15Kernel", ThinningTools = [EXOT15TPThinningTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT15Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT15Stream )
EXOT15Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT15Stream.AcceptAlgs(["EXOT15Kernel"])

#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT15ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT15ContentList import *
EXOT15SlimmingHelper = SlimmingHelper("EXOT15SlimmingHelper")
EXOT15SlimmingHelper.SmartCollections = EXOT15SmartContent
EXOT15SlimmingHelper.AllVariables = EXOT15AllVariablesContent

#EXOT15SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT15SlimmingHelper.IncludeJetTriggerContent = True
EXOT15SlimmingHelper.IncludeBJetTriggerContent = True
EXOT15SlimmingHelper.IncludeTauTriggerContent = True
#EXOT15SlimmingHelper.IncludeMuonTriggerContent = True
EXOT15SlimmingHelper.ExtraVariables += ['HLT_xAOD__JetContainer_a4tcemsubjesFS.m.EMFrac','Electrons.LHMedium','PrimaryVertices.x.y']

EXOT15SlimmingHelper.AppendContentToStream(EXOT15Stream)
