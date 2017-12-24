#********************************************************************
# EGAM5.py
# W->enu reduction
# reductionConf flag EGAM5 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM5ExtraContent import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False


#====================================================================
# SET UP STREAM (to be done early in the game to set up thinning Svc
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM5Stream )
EGAM5Stream = MSMgr.NewPoolRootStream( streamName, fileName )


#====================================================================
# SET UP SKIMMING
#====================================================================

#====================================================================
# WTP triggers
#====================================================================

# L1Topo W T&P 
triggers =  ['HLT_e13_etcut_trkcut' ]
triggers +=  ['HLT_e18_etcut_trkcut' ]
## # Non-L1Topo W TP commissioning triggers ==> in MC, in 50 ns data
triggers +=  ['HLT_e13_etcut_trkcut_xs15' ]
triggers +=  ['HLT_e18_etcut_trkcut_xs20' ]
## W T&P triggers ==> not in MC, in 50 ns data
triggers +=  ['HLT_e13_etcut_trkcut_xs15_mt25' ]
triggers +=  ['HLT_e18_etcut_trkcut_xs20_mt35' ]
###W T&P triggers ==> not in MC, not in 50 ns data, will be in 25 ns data
triggers +=  ['HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05' ]
triggers +=  ['HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25' ]
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25' ]
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05' ]
triggers +=  ['HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05' ]
triggers +=  ['HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25' ]
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25' ]
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05' ]
triggers +=  ['HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15' ]
triggers +=  ['HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35' ]
triggers +=  ['HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35' ]
triggers +=  ['HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15' ]

# others
triggers +=  ['HLT_e5_etcut_L1W-05DPHI-JXE-0']
triggers +=  ['HLT_e5_etcut_L1W-10DPHI-JXE-0']
triggers +=  ['HLT_e5_etcut_L1W-15DPHI-JXE-0']
triggers +=  ['HLT_e5_etcut_L1W-10DPHI-EMXE-0']
triggers +=  ['HLT_e5_etcut_L1W-15DPHI-EMXE-0']
triggers +=  ['HLT_e5_etcut_L1W-05DPHI-EMXE-1']
triggers +=  ['HLT_e5_etcut_L1W-05RO-XEHT-0']
triggers +=  ['HLT_e5_etcut_L1W-90RO2-XEHT-0']
triggers +=  ['HLT_e5_etcut_L1W-250RO2-XEHT-0']
triggers +=  ['HLT_e5_etcut_L1W-HT20-JJ15.ETA49']

triggers +=  ['HLT_e13_etcut_L1W-NOMATCH']
triggers +=  ['HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT']
triggers +=  ['HLT_e13_etcut_L1EM10_W-MT25']
triggers +=  ['HLT_e13_etcut_L1EM10_W-MT30']
triggers +=  ['HLT_e13_etcut_trkcut_L1EM12']
triggers +=  ['HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE']
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi15_mt25']
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi15_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20']
triggers +=  ['HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi15_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_xe30_mt35']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20']
triggers +=  ['HLT_e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0']

triggers +=  ['HLT_e18_etcut_L1EM15_W-MT35']
triggers +=  ['HLT_e18_etcut_trkcut_L1EM15']
triggers +=  ['HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_xe30_mt35']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30'] 
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30'] 
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE'] 
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE'] 
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE']
triggers +=  ['HLT_e18_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30'] 
triggers +=  ['HLT_e18_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE'] 
triggers +=  ['HLT_e18_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE']
triggers +=  ['HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30'] 
triggers +=  ['HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE']

# added for 2017
triggers += ['HLT_e60_etcut']
triggers += ['HLT_e60_etcut_L1EM24VHIM']
triggers += ['HLT_e60_etcut_trkcut_L1EM24VHIM_j15_perf_xe60_6dphi15_mt35']
triggers += ['HLT_e60_etcut_trkcut_L1EM24VHIM_xe60_mt35']
triggers += ['HLT_e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35']
triggers += ['HLT_e60_etcut_trkcut_L1EM24VHIM_xs30_xe30_mt35']
triggers += ['HLT_e60_lhmedium_nod0']
triggers += ['HLT_e60_lhmedium_nod0_L1EM24VHI']
triggers += ['HLT_e60_lhmedium_nod0_L1EM24VHIM']
triggers += ['HLT_e60_lhvloose_nod0']
triggers += ['HLT_e60_etcut_trkcut_j15_perf_xe60_6dphi05_mt35']
triggers += ['HLT_e60_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35']
triggers += ['HLT_e70_etcut']
triggers += ['HLT_e70_etcut_L1EM24VHIM']
triggers += ['HLT_e70_lhloose_nod0_L1EM24VHIM_xe70noL1']
triggers += ['HLT_e70_lhloose_nod0_xe70noL1']
triggers += ['HLT_noalg_l1topo_L1EM15']
triggers += ['HLT_noalg_l1topo_L1EM7']
triggers += ['HLT_j80_xe80']
triggers += ['HLT_xe80_tc_lcw_L1XE50']
triggers += ['HLT_xe90_mht_L1XE50']
triggers += ['HLT_xe90_tc_lcw_wEFMu_L1XE50']
triggers += ['HLT_xe90_mht_wEFMu_L1XE50']
triggers += ['HLT_xe110_mht_L1XE50']
triggers += ['HLT_xe110_pufit_L1XE50']

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EGAM5_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                   = "EGAM5_TriggerSkimmingTool",
                                                                        TriggerListOR          = triggers)
ToolSvc += EGAM5_TriggerSkimmingTool
print "EGAM5 trigger skimming tool:", EGAM5_TriggerSkimmingTool

#====================================================================
# W->enu selection for electron calibration
#====================================================================

# could add track isolation (if included in single electron trigger..)
if RecomputeElectronSelectors :
    requirement_el = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && Electrons.pt > 24.5*GeV'
else :
    requirement_el = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && Electrons.pt > 24.5*GeV'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGTransverseMassTool
EGAM5_MTTool = DerivationFramework__EGTransverseMassTool( name = "EGAM5_MTTool",
                                                          ObjectRequirements = requirement_el,
                                                          METmin = 25*GeV,
                                                          StoreGateEntryName = "WENU_TransverseMass",
                                                          ObjectMassHypothesis = 0.511*MeV,
                                                          ObjectContainerName = "Electrons",
                                                          METContainerName = "MET_LocHadTopo",
                                                          )
ToolSvc += EGAM5_MTTool
print EGAM5_MTTool

# Skimming criteria
expression = 'count(WENU_TransverseMass>40*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM5_OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM5_OfflineSkimmingTool",
                                                                         expression = expression)
ToolSvc += EGAM5_OfflineSkimmingTool
print "EGAM5 offline skimming tool:", EGAM5_OfflineSkimmingTool

#====================================================
# Third selection: mix of offline and online criteria
#====================================================
expression = '(HLT_e60_lhloose_xe60noL1 || HLT_e120_lhloose || HLT_j80_xe80 || HLT_xe70 || HLT_xe80_tc_lcw_L1XE50  || HLT_xe90_mht_L1XE50 ||  HLT_xe90_tc_lcw_wEFMu_L1XE50 || HLT_xe90_mht_wEFMu_L1XE50) && count(Electrons.pt>14.5*GeV)>=1'
EGAM5_ThirdSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM5_ThirdSkimmingTool",
                                                                       expression = expression)
ToolSvc += EGAM5_ThirdSkimmingTool
print "EGAM5 offline skimming tool:", EGAM5_ThirdSkimmingTool


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EGAM5_SkimmingTool = DerivationFramework__FilterCombinationOR(name="EGAM5_SkimmingTool", FilterList=[EGAM5_TriggerSkimmingTool,EGAM5_OfflineSkimmingTool,EGAM5_ThirdSkimmingTool] )
ToolSvc+=EGAM5_SkimmingTool



#====================================================================
# DECORATION TOOLS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM5_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM5_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM5_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]


#====================================================================                 
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM5_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM5_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM5_MaxCellDecoratorTool



#====================================================================
# SET UP THINNING
#====================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EGAM5ThinningHelper = ThinningHelper( "EGAM5ThinningHelper" )
EGAM5ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))'
if globalflags.DataSource()!='geant4':
    ExtraContainersTrigger += ExtraContainersTriggerDataOnly
EGAM5ThinningHelper.AppendToStream( EGAM5Stream, ExtraContainersTrigger )

thinningTools=[]
# TO BE ADDED


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam5Seq = CfgMgr.AthSequencer("EGAM5Sequence")
DerivationFrameworkJob += egam5Seq



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
egam5Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM5Kernel",
                                                         AugmentationTools = [EGAM5_MTTool,EGAM5_GainDecoratorTool,EGAM5_MaxCellDecoratorTool] + EGAM5_ClusterEnergyPerLayerDecorators,
                                                         SkimmingTools = [EGAM5_SkimmingTool]
                                                         )


#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam5Seq,"EGAM5")


#====================================================================
# SET UP STREAM SELECTION   
#====================================================================
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM5Stream.AcceptAlgs(["EGAM5Kernel"])


#====================================================================
# SET UP SKIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM5SlimmingHelper = SlimmingHelper("EGAM5SlimmingHelper")

EGAM5SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM5SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM5SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM5SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM5SlimmingHelper.AllVariables += ExtraContainersTrigger

if globalflags.DataSource()=='geant4':
    EGAM5SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM5SlimmingHelper.AllVariables += ExtraContainersTruth
else:
    EGAM5SlimmingHelper.ExtraVariables += ExtraContainersTriggerDataOnly

for tool in EGAM5_ClusterEnergyPerLayerDecorators:
    EGAM5SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM5SlimmingHelper
EGAM5SlimmingHelper.AppendContentToStream(EGAM5Stream)

