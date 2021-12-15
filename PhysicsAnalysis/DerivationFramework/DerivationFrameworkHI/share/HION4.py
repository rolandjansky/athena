
#********************************************************************
# HION4.py - derivations intended for analyses of exclusive
#            production of muons, electrons, photons
#            and vector mesons in UPC
#             
# reductionConf flag HION4 in Reco_tf.py
# author: mateusz.dyndal@cern.ch, marcin.guzik@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit


#====================================================================
# SKIMMING TOOLS
#====================================================================

# SELECTION FOR BACKGROUND ESTIMATES

#====================================================================
# prescaled _etcut triggers
# prescaled _loose triggers
# prescaled _lhloose triggers
#====================================================================

#requirement_object = 'Electrons.pt > 2.0*GeV'
#objectSelection = 'count('+requirement_object+') >= 1'


muonsRequirements = '(Muons.pt >= 2.0*GeV) && (abs(Muons.eta) < 2.6)'
electronsRequirements = '(Electrons.pt > 2.0*GeV) && (abs(Electrons.eta) < 2.6)'
photonsRequirements = '(Photons.pt >= 2.0*GeV)'

muonOnlySelection = 'count('+muonsRequirements+') >= 1'
electronOnlySelection = 'count('+electronsRequirements+') >= 1'
photonOnlySelection = 'count('+photonsRequirements+') >=2'
electronPhotonSelection = '(count('+electronsRequirements+') + count('+photonsRequirements+')) >= 2'

trackRequirements = '(InDetTrackParticles.pt >= 0.2*GeV) && (abs(InDetTrackParticles.eta) < 2.5)'
trackOnlySelection = '( count('+trackRequirements+') >= 2 && 5 >= count('+trackRequirements+') )' 

tightTrackRequirements = '(InDetTrackParticles.pt >= 1*GeV) && (abs(InDetTrackParticles.eta) < 2.5)'
tightTrackOnlySelection = '( count('+tightTrackRequirements+') == 2 )'

objectSelection = '('+muonOnlySelection+' || '+electronOnlySelection+' || '+photonOnlySelection+' || '+electronPhotonSelection+' ||'+trackOnlySelection+')'

#2015 triggers
triggers  = ['HLT_mb_sptrk_vetombts2in_L1MU0_VTE50']
triggers += ['HLT_noalg_L1MU0_VTE50']
triggers += ['HLT_hi_loose_upc_L1ZDC_A_C_VTE50']
triggers += ['HLT_mb_sptrk_vetombts2in_L1ZDC_A_C_VTE50']
triggers += ['HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50']
triggers += ['HLT_noalg_L1ZDC_A_C_VTE50']
triggers += ['HLT_hi_gg_upc_L1TE5_VTE200']


#2016 triggers
triggers += ['HLT_e5_etcut_L1EM3_VZDC_A']
triggers += ['HLT_e5_etcut_L1EM3_VZDC_C']
triggers += ['HLT_e5_lhloose_nod0']
triggers += ['HLT_e5_lhloose_L1EM3_VZDC_A']
triggers += ['HLT_e5_lhloose_L1EM3_VZDC_C']
triggers += ['HLT_e5_lhloose_nod0_L1EM3_VZDC_A']
triggers += ['HLT_e5_lhloose_nod0_L1EM3_VZDC_C']
triggers += ['HLT_2e5_etcut_L12EM3_VZDC_A']
triggers += ['HLT_2e5_etcut_L12EM3_VZDC_C']
triggers += ['HLT_2e5_lhloose_L12EM3_VZDC_A']
triggers += ['HLT_2e5_lhloose_L12EM3_VZDC_C']
triggers += ['HLT_2e5_lhloose_nod0_L12EM3_VZDC_A']
triggers += ['HLT_2e5_lhloose_nod0_L12EM3_VZDC_C']
triggers += ['HLT_g5_loose_L1VZDC_A']
triggers += ['HLT_g5_loose_L1VZDC_C']
triggers += ['HLT_g10_loose_L1VZDC_A']
triggers += ['HLT_g10_loose_L1VZDC_C']
triggers += ['HLT_g5_etcut_L1VZDC_A']
triggers += ['HLT_g5_etcut_L1VZDC_C']
triggers += ['HLT_g10_etcut_L1VZDC_A']
triggers += ['HLT_g10_etcut_L1VZDC_C']
triggers += ['HLT_mu4_L1MU4_VTE10']
triggers += ['HLT_mu4_L1MU4_VZDC_A']
triggers += ['HLT_mu4_L1MU4_VZDC_C']
triggers += ['HLT_2mu4_L12MU4_VTE10']
triggers += ['HLT_2mu4_L12MU4_VZDC_A']
triggers += ['HLT_2mu4_L12MU4_VZDC_C']
triggers += ['HLT_mu4_L1MU4_VTE10_VZDC_A']
triggers += ['HLT_mu4_L1MU4_VTE10_VZDC_C']
triggers += ['HLT_2mu4_L12MU4_VTE10_VZDC_A']
triggers += ['HLT_2mu4_L12MU4_VTE10_VZDC_C']
triggers += ['HLT_mu6_L1MU6_VTE10_VZDC_A']
triggers += ['HLT_mu6_L1MU6_VTE10_VZDC_C']
triggers += ['HLT_mb_sptrk_exclusiveloose_L1VTE10_VZDC_A']
triggers += ['HLT_mb_sptrk_exclusiveloose_L1VTE10_VZDC_C']
triggers += ['HLT_mb_sptrk_exclusiveloose_L1RD0_FILLED']
triggers += ['HLT_mb_sptrk_exclusiveloose_L1ZDC']
triggers += ['HLT_mb_sptrk_exclusiveloose_L1MU4_VTE10']
triggers += ['HLT_mb_sptrk_exclusivetight_L1VTE10_VZDC_A']
triggers += ['HLT_mb_sptrk_exclusivetight_L1VTE10_VZDC_C']
triggers += ['HLT_mb_sptrk_exclusivetight_L1RD0_FILLED']
triggers += ['HLT_mb_sptrk_exclusivetight_L1ZDC']
triggers += ['HLT_hi_upc_FgapA_L1RD0']
triggers += ['HLT_hi_upc_FgapC_L1RD0']
triggers += ['HLT_hi_upc_FgapAC_L1RD0']
triggers += ['HLT_hi_upc_FgapA_L1MBTS_1']
triggers += ['HLT_hi_upc_FgapC_L1MBTS_1']
triggers += ['HLT_hi_upc_FgapAC_L1MBTS_1']
triggers += ['HLT_hi_upc_L2FgapA_L1MBTS_1']
triggers += ['HLT_hi_upc_L2FgapC_L1MBTS_1']
triggers += ['HLT_hi_upc_L2FgapAC_L1MBTS_1']
triggers += ['HLT_hi_upc_EFFgapA_L1MBTS_1']
triggers += ['HLT_hi_upc_EFFgapC_L1MBTS_1']
triggers += ['HLT_hi_upc_EFFgapAC_L1MBTS_1']
triggers += ['HLT_hi_upc_FgapA_L1ZDC']
triggers += ['HLT_hi_upc_FgapC_L1ZDC']
triggers += ['HLT_hi_upc_FgapAC_L1ZDC']
triggers += ['HLT_noalg_L1MBTS_2_C_VZDC_A']
triggers += ['HLT_noalg_L1MBTS_2_A_VZDC_C']
triggers += ['HLT_noalg_L1ZDC_C_VZDC_A']
triggers += ['HLT_noalg_L1ZDC_A_VZDC_C']
triggers += ['HLT_noalg_L1EM3_VZDC_A']
triggers += ['HLT_noalg_L1EM3_VZDC_C']
triggers += ['HLT_noalg_L12EM3_VZDC_A']
triggers += ['HLT_noalg_L12EM3_VZDC_C']
triggers += ['HLT_noalg_L1EM5_VZDC_A']
triggers += ['HLT_noalg_L1EM5_VZDC_C']
triggers += ['HLT_noalg_L1J5_VZDC_A']
triggers += ['HLT_noalg_L1J5_VZDC_C']
triggers += ['HLT_noalg_L1J10_VZDC_A']
triggers += ['HLT_noalg_L1J10_VZDC_C']
triggers += ['HLT_noalg_L1MU4_VZDC_A']
triggers += ['HLT_noalg_L1MU4_VZDC_C']
triggers += ['HLT_noalg_L1MU6_VZDC_A']
triggers += ['HLT_noalg_L1MU6_VZDC_C']
triggers += ['HLT_noalg_L1TE5_VZDC_A']
triggers += ['HLT_noalg_L1TE5_VZDC_C']
triggers += ['HLT_hi_upc_FgapA_mb_sptrk_L1RD0_FILLED']
triggers += ['HLT_hi_upc_FgapC_mb_sptrk_L1RD0_FILLED']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED']
triggers += ['HLT_hi_upc_FgapA_mb_sptrk_L1MBTS_2_C']
triggers += ['HLT_hi_upc_FgapC_mb_sptrk_L1MBTS_2_A']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_L1MBTS_2']
triggers += ['HLT_hi_upc_FgapA_mb_sptrk_L1ZDC']
triggers += ['HLT_hi_upc_FgapC_mb_sptrk_L1ZDC']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_L1ZDC']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose_L1ZDC']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose_L1VTE10_VZDC_A']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose_L1VTE10_VZDC_C']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight_L1ZDC']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight_L1VTE10_VZDC_A']
triggers += ['HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight_L1VTE10_VZDC_C']
triggers += ['HLT_hi_upc_FgapAC_mu4']
triggers += ['HLT_hi_upc_FgapAC_mu6']
triggers += ['HLT_hi_upc_FgapAC_2mu4']
triggers += ['HLT_hi_upc_FgapAC_mu4_mu4noL1']
triggers += ['HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10']
triggers += ['HLT_hi_upc_FgapAC_e5_etcut']
triggers += ['HLT_hi_upc_FgapAC_e5_lhloose_nod0']
triggers += ['HLT_hi_upc_FgapAC_2e5_etcut']
triggers += ['HLT_hi_upc_FgapAC_2e5_lhloose_nod0']
triggers += ['HLT_hi_upc_FgapA_g5_loose']
triggers += ['HLT_hi_upc_FgapC_g5_loose']
triggers += ['HLT_hi_upc_FgapA_g10_loose']
triggers += ['HLT_hi_upc_FgapC_g10_loose']
triggers += ['HLT_hi_upc_FgapA_g5_etcut']
triggers += ['HLT_hi_upc_FgapC_g5_etcut']
triggers += ['HLT_hi_upc_FgapA_g10_etcut']
triggers += ['HLT_hi_upc_FgapC_g10_etcut']

triggers += ['HLT_mb_sptrk_exclusiveloose1_L1VTE10_VZDC_A']
triggers += ['HLT_mb_sptrk_exclusiveloose1_L1VTE10_VZDC_C']
triggers += ['HLT_mb_sptrk_exclusiveloose1_L1RD0_FILLED']
triggers += ['HLT_mb_sptrk_exclusiveloose1_L1ZDC']
triggers += ['HLT_mb_sptrk_exclusiveloose1_L1MU4_VTE10'] 

triggers += ['HLT_mb_sptrk_exclusiveloose2_L1VTE10_VZDC_A']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L1VTE10_VZDC_C']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L1RD0_FILLED']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L1ZDC']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L1MU4_VTE10'] 

#2018 triggers

#primary and backup triggers
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE4_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE4_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE5_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE5_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TAU1_TE4_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TAU1_TE5_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L12TAU1_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU2_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU2_VTE100']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU2_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU3_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU3_VTE100']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU3_VTE200']

# supporting triggers
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L12TAU1_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L12TAU2_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose1_L1MU4_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1MU4_VTE50']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L12TAU2_VTE50']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L12TAU2_VTE100']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L12TAU2_VTE200']
triggers += ['HLT_mb_sptrk_vetombts2in_exclusiveloose2_L12TAU1_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose1_L1ZDC_XOR_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1ZDC_XOR_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1ZDC_A_C_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1VZDC_A_C_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE4_VTE200_EMPTY']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE5_VTE200_EMPTY']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE50_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE4_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE4_VTE100']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE4_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE5_VTE50']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE5_VTE100']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TE5_VTE200']
triggers += ['HLT_hi_gg_upc_L1TE4_VTE50']
triggers += ['HLT_hi_gg_upc_L1TE4_VTE100']
triggers += ['HLT_hi_gg_upc_L1TE4_VTE200']
triggers += ['HLT_hi_gg_upc_L1TE5_VTE50']
triggers += ['HLT_hi_gg_upc_L1TE5_VTE100']
triggers += ['HLT_hi_gg_upc_L1TE5_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TAU1_TE4_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L1TAU1_TE5_VTE200']
triggers += ['HLT_hi_upc_FgapAC3_hi_gg_upc_L12TAU1_VTE50']
triggers += ['HLT_mb_sptrk_vetombts2in_exclusiveloose2_L12TAU2_VTE50']
triggers += ['HLT_mb_sptrk_vetombts2in_exclusiveloose2_L12TAU2_VTE100']
triggers += ['HLT_mb_sptrk_vetombts2in_exclusiveloose2_L12TAU2_VTE200']
triggers += ['HLT_mb_sp_L1VTE50']
triggers += ['HLT_mb_sptrk_exclusiveloose2_L12TAU1_VTE50']
triggers += ['HLT_mu4_hi_upc_FgapAC3_L1MU4_VTE50']

VMtrigger = 'HLT_mb_sptrk_exclusiveloose_vetosp1500_L1VTE20'

expression = '( (' + ' || '.join(triggers) + ') && '+objectSelection+') || ( '+VMtrigger+ ' && '+tightTrackOnlySelection+')'
print expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HION4SkimmingTool",
                                                                   expression = expression)
ToolSvc += HION4SkimmingTool
print "HION4 skimming tool:", HION4SkimmingTool

#====================================================================
# Cell sum decoration tool
#====================================================================

#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CellDecorator
#HION4_CellDecoratorTool = DerivationFramework__CellDecorator( name                    = "HION4_CellDecoratorTool",
#                                                              SGKey_electrons         = "Electrons",
#                                                              SGKey_photons           = "Photons",
#                                                              CaloFillRectangularTool_5x5  = EGAMCOM_caloFillRect55,
#                                                              CaloFillRectangularTool_3x5  = EGAMCOM_caloFillRect35,
#                                                              CaloFillRectangularTool_3x7  = EGAMCOM_caloFillRect37,
#                                                              CaloFillRectangularTool_7x11  = EGAMCOM_caloFillRect711
#                                                              )
#ToolSvc += HION4_CellDecoratorTool
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations
HION4_GainDecoratorTool = GainDecorator()
ToolSvc += HION4_GainDecoratorTool


#====================================================================                                                                              
# Max Cell sum decoration tool
#====================================================================                                                        

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
HION4_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "HION4_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += HION4_MaxCellDecoratorTool


#====================================================================                                                                              
# Initialize flavour tagging algoritmhs
#====================================================================                                                        

FlavorTagInit(JetCollections  = ['AntiKt4EMTopoJets'], Sequencer = DerivationFrameworkJob)


#================
# THINNING
#================
thinningTools=[]




# Truth thinning
truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HION4TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HION4TruthThinningTool",
                                                                   ThinningService         = "HION4ThinningSvc",
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = False,
                                                                   PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "HION4 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()=='geant4':
    ToolSvc += HION4TruthThinningTool
    thinningTools.append(HION4TruthThinningTool)
print "HION4 thinningTools: ", thinningTools


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HION4Kernel",
                                                                       #AugmentationTools = [HION4_CellDecoratorTool, HION4_MaxCellDecoratorTool],
                                                                       AugmentationTools = [HION4_GainDecoratorTool, HION4_MaxCellDecoratorTool],
                                                                       SkimmingTools = [HION4SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                       )


#============ Create Derivation HION4 cell collection ==================

# Keep only calo cells associated with the egammaClusters collection
from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
                                       outputCellKey="DFEGAMCellContainer")

#========================================================================


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HION4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION4Stream )
HION4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
HION4Stream.AcceptAlgs(["HION4Kernel"])


#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HION4ThinningSvc", outStreams=[evtStream] )

#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION4SlimmingHelper = SlimmingHelper("HION4SlimmingHelper")

# same content as EGAM1
from DerivationFrameworkEGamma.EGAM1ExtraContent import *
HION4SlimmingHelper.SmartCollections = [
				        "Electrons",
					"Photons",
					"Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"
                                        ]

# Add egamma trigger objects
HION4SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
HION4SlimmingHelper.ExtraVariables = ExtraContentAll
HION4SlimmingHelper.AllVariables = ExtraContainersElectrons
HION4SlimmingHelper.AllVariables += ExtraContainersTrigger



AddContent=[  "CaloSums",
              "ZdcModules",
              "ZdcSums",
              "ZdcTriggerTowers",
              "MBTSForwardEventInfo",
              "MBTSModules",
              "HLT_xAOD__HIEventShapeContainer_HIFCAL",
              "HLT_xAOD__HIEventShapeContainer_HIUE",
              "HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts",
              "HLT_xAOD__TrigTrackCountsContainer_trackcounts",
              "HLT_xAOD__TrigSpacePointCountsContainer_spacepoints",
              "LVL1MuonRoIs",
              "HIEventShape",
              "CaloCalTopoClusters",
              "egammaClusters",
              "egammaTopoSeededClusters",
	      "InDetPixelTrackParticles",
	      "Photons",
	      "Electrons"
]

 
HION4SlimmingHelper.AllVariables+=AddContent


if globalflags.DataSource()!='geant4':
    HION4SlimmingHelper.AllVariables += ExtraContainersTriggerDataOnly

if globalflags.DataSource()=='geant4':
    HION4SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    HION4SlimmingHelper.AllVariables += ExtraContainersTruth

# This line must come after we have finished configuring HION4SlimmingHelper
HION4SlimmingHelper.AppendContentToStream(HION4Stream)

HION4Stream.AddItem("xAOD::EnergySumRoI#*")
HION4Stream.AddItem("xAOD::EnergySumRoIAuxInfo#*")

# Add AODCellContainer (thinned)
HION4Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
HION4Stream.AddItem("CaloCellContainer#DFEGAMCellContainer")

