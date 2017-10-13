#********************************************************************
# EGAM9.py - keep events passing or of photon triggers used for
#            boostrap efficiency measurement of photon triggers
#            fake electron candidates 
# reductionConf flag EGAM9 in Reco_tf.py
# author: fernando.monticelli@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *


#====================================================================
# SKIMMING TOOLS
#====================================================================

# SELECTION FOR BACKGROUND ESTIMATES

#====================================================================
# prescaled _etcut triggers
# prescaled _loose triggers
# prescaled _lhloose triggers
#====================================================================

# photon_selection ='count(Photons.pt > 10.0*GeV && ( (Photons.isEMTight & 0x1fc01)==0) )>0' 
# it seems hex numbers are not interpreted
photon_selection ='count(Photons.pt > 10.0*GeV &&  Photons.Loose>0 )>0'
electron_selection = ' count(Electrons.pt > 10.0*GeV && Electrons.LHMedium>0)>0 '
objectSelection = '( (' + photon_selection + ') || ('+ electron_selection +' ) )'

triggers =[]

# HLT_noalg_ triggers
triggers += ['HLT_noalg_L1EM15VH']
triggers += ['HLT_noalg_L1EM12']
triggers += ['HLT_noalg_L1EM15']
triggers += ['HLT_noalg_L1EM18VH']
triggers += ['HLT_noalg_L1EM20VH']
triggers += ['HLT_noalg_L1EM10']
triggers += ['HLT_noalg_L1EM10VH']
triggers += ['HLT_noalg_L1EM13VH']
triggers += ['HLT_noalg_L1EM20VHI']
triggers += ['HLT_noalg_L1EM22VHI']
triggers += ['HLT_noalg_L1EM8VH']

# pt_cut triggers
triggers += ['HLT_g20_etcut_L1EM12']            

# Passed through triggers for bootstrapping
triggers += ['HLT_g10_loose']
triggers += ['HLT_g15_loose_L1EM7']
triggers += ['HLT_g20_loose_L1EM12']
triggers += ['HLT_g20_loose']
triggers += ['HLT_g25_loose_L1EM15']
triggers += ['HLT_g60_loose']
triggers += ['HLT_g100_loose']
triggers += ['HLT_g120_loose']
triggers += ['HLT_g160_loose']
triggers += ['HLT_g160_loose_L1EM24VHIM']
triggers += ['HLT_g180_loose']
triggers += ['HLT_g180_loose_L1EM24VHIM']
triggers += ['HLT_g35_loose_L1EM15']
triggers += ['HLT_g40_loose_L1EM15']
triggers += ['HLT_g45_loose_L1EM15']
triggers += ['HLT_g50_loose_L1EM15']
triggers += ['HLT_g70_loose']
triggers += ['HLT_g80_loose']
triggers += ['HLT_g140_loose']
triggers += ['HLT_g200_loose']


expression = '(' + ' || '.join(triggers) + ') && '+objectSelection
print expression


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM9_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM9_SkimmingTool",
                                                                   expression = expression)
ToolSvc += EGAM9_SkimmingTool
print "EGAM9 skimming tool:", EGAM9_SkimmingTool


#====================================================================
# DECORATION TOOLS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM9_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM9_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM9_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]



#====================================================================                                                                              
# Max Cell sum decoration tool
#====================================================================                                                        

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM9_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM9_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM9_MaxCellDecoratorTool


#================
# THINNING
#================
thinningTools=[]




# Truth thinning
truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EGAM9TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM9TruthThinningTool",
                                                                   ThinningService         = "EGAM9ThinningSvc",
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = False,
                                                                   PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "EGAM9 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()=='geant4':
    ToolSvc += EGAM9TruthThinningTool
    thinningTools.append(EGAM9TruthThinningTool)
print "EGAM9 thinningTools: ", thinningTools


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam9Seq = CfgMgr.AthSequencer("EGAM9Sequence")
DerivationFrameworkJob += egam9Seq



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
egam9Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM9Kernel",
                                                         AugmentationTools = [EGAM9_GainDecoratorTool, EGAM9_MaxCellDecoratorTool] + EGAM9_ClusterEnergyPerLayerDecorators,
                                                         SkimmingTools = [EGAM9_SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )



#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam9Seq,"EGAM9")



#============ Create Derivation EGAM9 cell collection ==================

# Keep only calo cells associated with the egammaClusters collection
from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
                                       outputCellKey="DFEGAMCellContainer")

#========================================================================


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM9Stream )
EGAM9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM9Stream.AcceptAlgs(["EGAM9Kernel"])


#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EGAM9ThinningSvc", outStreams=[evtStream] )

#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkEGamma.EGAM9ExtraContent import *

# Keep only electrons and photons
EGAM9SlimmingHelper = SlimmingHelper("EGAM9SlimmingHelper")
EGAM9SlimmingHelper.SmartCollections = [
				        "Electrons",
					"Photons",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"
                                        ]

# Add egamma trigger objects
EGAM9SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM9SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM9SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM9SlimmingHelper.AllVariables += ExtraContainersPhotons
EGAM9SlimmingHelper.AllVariables += ExtraContainersTrigger
if globalflags.DataSource()!='geant4':
    EGAM9SlimmingHelper.AllVariables += ExtraContainersTriggerDataOnly

if globalflags.DataSource()=='geant4':
    EGAM9SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM9SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM9_ClusterEnergyPerLayerDecorators:
    EGAM9SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM9SlimmingHelper
EGAM9SlimmingHelper.AppendContentToStream(EGAM9Stream)
