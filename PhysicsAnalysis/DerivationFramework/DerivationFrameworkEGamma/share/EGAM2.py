#********************************************************************
# EGAM2.py
# J/psi->ee derivation for calibration
# reductionConf flag EGAM2 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM2ExtraContent import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False


#====================================================================
# SET UP STREAM (to be done early in the game to set up thinning Svc
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM2Stream )
EGAM2Stream = MSMgr.NewPoolRootStream( streamName, fileName )


#====================================================================
# SET UP SKIMMING
#====================================================================

# SELECTION FOR CALIBRATION

#====================================================================
# J/psi->ee selection for e/gamma calibration
# 2 tight or medium e (depends on Run2 triggers..), pT>4.5 GeV, OS, 1<Mee<5 GeV
#====================================================================
electronPtRequirement = '(Electrons.pt > 4.5*GeV)'
if RecomputeElectronSelectors :
    electronQualityRequirement = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium)'
else :
    electronQualityRequirement = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium)'
requirement_el = '(' + electronQualityRequirement + '&&' + electronPtRequirement + ')'


from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM2_JPSIEEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM2_JPSIEEMassTool",
                                                                 Object1Requirements = requirement_el,
                                                                 Object2Requirements = requirement_el,
                                                                 StoreGateEntryName = "EGAM2_DiElectronMass",
                                                                 Mass1Hypothesis = 0.511*MeV,
                                                                 Mass2Hypothesis = 0.511*MeV,
                                                                 Container1Name = "Electrons",
                                                                 Container2Name = "Electrons",
                                                                 CheckCharge = True,
                                                                 DoTransverseMass = False,
                                                                 MinDeltaR = 0.0)
ToolSvc += EGAM2_JPSIEEMassTool
print EGAM2_JPSIEEMassTool

# SELECTION FOR T&P

#====================================================================
# Jpsi->ee selection based on Jpsi->e+cluster trigger, for low pT (7-20 GeV) central photons
# Tag: 1 tight e, central, pT>4.5 GeV
# Probe: 1 e, central, pT>4.5 GeV
# OS+SS
# 1<mee<6 GeV
# dR>0.15
#====================================================================
if RecomputeElectronSelectors :
    requirement_el_tag = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && Electrons.pt > 4.5*GeV'
else :
    requirement_el_tag = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && Electrons.pt > 4.5*GeV'
requirement_el_probe = 'Electrons.pt > 4.5*GeV'

EGAM2_JPSIEEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM2_JPSIEEMassTool2",
                                                                  Object1Requirements = requirement_el_tag,
                                                                  Object2Requirements = requirement_el_probe,
                                                                  StoreGateEntryName = "EGAM2_DiElectronMass2",
                                                                  Mass1Hypothesis = 0.511*MeV,
                                                                  Mass2Hypothesis = 0.511*MeV,
                                                                  Container1Name = "Electrons",
                                                                  Container2Name = "Electrons",
                                                                  CheckCharge = False,
                                                                  DoTransverseMass = False,
                                                                  MinDeltaR = 0.15)
ToolSvc += EGAM2_JPSIEEMassTool2
print EGAM2_JPSIEEMassTool2

# Skimming criteria (offline selection)
expression_calib = '(count(EGAM2_DiElectronMass > 1.0*GeV && EGAM2_DiElectronMass < 5.0*GeV)>=1)'
expression_TP = '(count(EGAM2_DiElectronMass2 > 1.0*GeV && EGAM2_DiElectronMass2 < 6.0*GeV)>=1)'
expression = expression_calib + ' || ' + expression_TP
#expression = expression_calib
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM2_OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM2_OfflineSkimmingTool",
                                                                        expression = expression)
ToolSvc += EGAM2_OfflineSkimmingTool
print "EGAM2 offline skimming tool:", EGAM2_OfflineSkimmingTool

#====================================================================
# trigger-based selection
#====================================================================
triggers=['HLT_e5_lhtight_e4_etcut_Jpsiee']
triggers+=['HLT_e5_lhtight_nod0_e4_etcut_Jpsiee']
triggers+=['HLT_e5_lhtight_e4_etcut']
triggers+=['HLT_e5_lhtight_nod0_e4_etcut']

triggers+=['HLT_e9_lhtight_e4_etcut_Jpsiee']
triggers+=['HLT_e9_lhtight_nod0_e4_etcut_Jpsiee']
triggers+=['HLT_e9_etcut_e5_lhtight_nod0_Jpsiee']
triggers+=['HLT_e9_etcut_e5_lhtight_Jpsiee']

triggers+=['HLT_e14_etcut_e5_lhtight_Jpsiee']
triggers+=['HLT_e14_etcut_e5_lhtight_nod0_Jpsiee']
triggers+=['HLT_e14_lhtight_e4_etcut_Jpsiee']
triggers+=['HLT_e14_lhtight_nod0_e4_etcut_Jpsiee']

triggers+=['HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1RD0_FILLED']
triggers+=['HLT_e5_lhtight_nod0_e9_etcut_Jpsiee']
triggers+=['HLT_e5_lhtight_nod0_e14_etcut_Jpsiee']
triggers+=['HLT_e5_lhtight_nod0_e9_etcut_Jpsiee_L1JPSI-1M5-EM7']
triggers+=['HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7']
triggers+=['HLT_e5_lhtight_nod0_e14_etcut_Jpsiee_L1JPSI-1M5-EM12']
triggers+=['HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12']



from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EGAM2_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "EGAM2_TriggerSkimmingTool", TriggerListOR = triggers)

ToolSvc += EGAM2_TriggerSkimmingTool
print "EGAM2 trigger skimming tool:", EGAM2_TriggerSkimmingTool


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EGAM2_SkimmingTool = DerivationFramework__FilterCombinationOR(name="EGAM2SkimmingTool", FilterList=[EGAM2_OfflineSkimmingTool,EGAM2_TriggerSkimmingTool] )
ToolSvc+=EGAM2_SkimmingTool
print "EGAM2 skimming tool:", EGAM2_SkimmingTool



#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM2_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM2_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM2_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]


#====================================================================
# SET UP THINNING
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EGAM2ThinningHelper = ThinningHelper( "EGAM2ThinningHelper" )
EGAM2ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))'
EGAM2ThinningHelper.AppendToStream( EGAM2Stream, ExtraContainersTrigger )


thinningTools=[]


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam2Seq = CfgMgr.AthSequencer("EGAM2Sequence")
DerivationFrameworkJob += egam2Seq


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
egam2Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM2Kernel",
                                                         AugmentationTools = [EGAM2_JPSIEEMassTool,EGAM2_JPSIEEMassTool2,EGAM2_GainDecoratorTool] + EGAM2_ClusterEnergyPerLayerDecorators,
                                                         SkimmingTools = [EGAM2_SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )


#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam2Seq,"EGAM2")


#====================================================================
# SET UP STREAM SELECTION
#====================================================================
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM2Stream.AcceptAlgs(["EGAM2Kernel"])



#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM2SlimmingHelper = SlimmingHelper("EGAM2SlimmingHelper")

EGAM2SlimmingHelper.SmartCollections = [
				        "Electrons",
                                        "Photons",
					"Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]
# muons, tau, MET, b-tagging could be switched off if not needed and use too much space

# Add egamma trigger objects
EGAM2SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM2SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM2SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM2SlimmingHelper.AllVariables += ExtraContainersTrigger

if globalflags.DataSource()=='geant4':
    EGAM2SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM2SlimmingHelper.AllVariables += ExtraContainersTruth
else:
    EGAM2SlimmingHelper.ExtraVariables += ExtraContainersTriggerDataOnly

for tool in EGAM2_ClusterEnergyPerLayerDecorators:
    EGAM2SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM2SlimmingHelper
EGAM2SlimmingHelper.AppendContentToStream(EGAM2Stream)
