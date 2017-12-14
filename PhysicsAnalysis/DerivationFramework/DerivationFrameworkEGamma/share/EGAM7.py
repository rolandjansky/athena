#********************************************************************
# EGAM7.py - keep events passing or of electron triggers, to select
#            fake electron candidates 
# reductionConf flag EGAM7 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM7ExtraContent import *


#====================================================================
# SET UP STREAM (to be done early in the game to set up thinning Svc
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM7Stream )
EGAM7Stream = MSMgr.NewPoolRootStream( streamName, fileName )


#====================================================================
# SET UP SKIMMING
#====================================================================

# SELECTION FOR BACKGROUND ESTIMATES

#====================================================================
# prescaled _etcut triggers
# prescaled _loose triggers
# prescaled _lhloose triggers
#====================================================================

requirement_object = 'Electrons.pt > 4.5*GeV'
objectSelection = 'count('+requirement_object+') >= 1'

triggers =  ['HLT_e4_etcut'        ]
triggers += ['HLT_e5_etcut'        ]
triggers += ['HLT_e9_etcut'        ]            
triggers += ['HLT_e10_etcut_L1EM7' ]            
triggers += ['HLT_e14_etcut'       ]            
triggers += ['HLT_e15_etcut_L1EM7' ]
triggers += ['HLT_e17_etcut_L1EM15']            
triggers += ['HLT_e20_etcut_L1EM12']            
triggers += ['HLT_e25_etcut_L1EM15']            
triggers += ['HLT_e30_etcut_L1EM15']            
triggers += ['HLT_e40_etcut_L1EM15']            
triggers += ['HLT_e50_etcut_L1EM15']            
triggers += ['HLT_e60_etcut'       ]            
triggers += ['HLT_e80_etcut'       ]            
triggers += ['HLT_e100_etcut'      ]            
triggers += ['HLT_e120_etcut'      ]            
triggers += ['HLT_g10_etcut'       ]            
triggers += ['HLT_g20_etcut_L1EM12']            
triggers += ['HLT_g200_etcut'      ]            

triggers += ['HLT_e5_lhloose'                      ]
triggers += ['HLT_e5_lhvloose'                     ]
triggers += ['HLT_e5_loose'                        ]
triggers += ['HLT_e5_vloose'                       ]
triggers += ['HLT_e10_lhvloose_L1EM7'              ]
triggers += ['HLT_e10_vloose_L1EM7'                ]
triggers += ['HLT_e12_lhloose'                     ]
triggers += ['HLT_e12_lhloose_L1EM10VH'            ]
triggers += ['HLT_e12_lhvloose_L1EM10VH'           ]
triggers += ['HLT_e12_loose'                       ]
triggers += ['HLT_e12_loose_L1EM10VH'              ]
triggers += ['HLT_e12_vloose_L1EM10VH'             ]
triggers += ['HLT_e15_lhloose_L1EM13VH'            ]
triggers += ['HLT_e15_lhvloose_L1EM13VH'           ]
triggers += ['HLT_e15_lhvloose_L1EM7'              ]
triggers += ['HLT_e15_loose_L1EM13VH'              ]
triggers += ['HLT_e15_vloose_L1EM13VH'             ]
triggers += ['HLT_e15_vloose_L1EM7'                ]
triggers += ['HLT_e17_lhloose'                     ]
triggers += ['HLT_e17_lhloose_L1EM15'              ]
triggers += ['HLT_e17_lhloose_cutd0dphideta_L1EM15']
triggers += ['HLT_e17_lhloose_nod0_L1EM15'         ]
triggers += ['HLT_e17_lhloose_nodeta_L1EM15'       ]
triggers += ['HLT_e17_lhloose_nodphires_L1EM15'    ]
triggers += ['HLT_e17_lhloose_L1EM15VHJJ1523ETA49' ]
triggers += ['HLT_e17_lhvloose'                    ]
triggers += ['HLT_e17_loose'                       ]
triggers += ['HLT_e17_loose_L1EM15'                ]
triggers += ['HLT_e17_loose_L1EM15VHJJ1523ETA49'   ]
triggers += ['HLT_e17_vloose'                      ]
triggers += ['HLT_e20_lhvloose'                    ]
triggers += ['HLT_e20_lhvloose_L1EM12'             ]
triggers += ['HLT_e20_vloose'                      ]
triggers += ['HLT_e20_vloose_L1EM12'               ]
triggers += ['HLT_e25_lhvloose_L1EM15'             ]
triggers += ['HLT_e25_vloose_L1EM15'               ]
triggers += ['HLT_e30_lhvloose_L1EM15'             ]
triggers += ['HLT_e30_vloose_L1EM15'               ]
triggers += ['HLT_e40_lhvloose'                    ]
triggers += ['HLT_e40_lhvloose_L1EM15'             ]
triggers += ['HLT_e40_vloose_L1EM15'               ]
triggers += ['HLT_e50_lhvloose_L1EM15'             ]
triggers += ['HLT_e50_vloose_L1EM15'               ]
triggers += ['HLT_e60_loose'                       ]
triggers += ['HLT_e60_vloose'                      ]
triggers += ['HLT_e60_lhvloose'                    ]
triggers += ['HLT_e70_etcut'                       ]
triggers += ['HLT_e70_lhloose'                     ]
triggers += ['HLT_e70_lhvloose'                    ]
triggers += ['HLT_e70_loose'                       ]
triggers += ['HLT_e70_vloose'                      ]
triggers += ['HLT_e80_lhvloose'                    ]
triggers += ['HLT_e80_vloose'                      ]
triggers += ['HLT_e100_lhvloose'                   ]
triggers += ['HLT_e100_vloose'                     ]
triggers += ['HLT_e120_lhvloose'                   ]
triggers += ['HLT_e120_lhloose'                    ]
triggers += ['HLT_e120_loose'                      ]
triggers += ['HLT_e120_vloose'                     ]
triggers += ['HLT_e140_etcut'                      ]
triggers += ['HLT_e160_etcut'                      ]
triggers += ['HLT_e180_etcut'                      ]
triggers += ['HLT_e200_etcut'                      ]
triggers += ['HLT_e250_etcut'                      ]
triggers += ['HLT_e300_etcut'                      ]
triggers += ['HLT_g250_etcut'                      ]
triggers += ['HLT_g300_etcut'                      ]

expression = '(' + ' || '.join(triggers) + ') && '+objectSelection
print expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM7_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM7_SkimmingTool",
                                                                   expression = expression)
ToolSvc += EGAM7_SkimmingTool
print "EGAM7 skimming tool:", EGAM7_SkimmingTool



#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM7_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM7_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM7_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]


#====================================================================
# Max Cell sum decoration tool
#====================================================================                                                        

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM7_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM7_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM7_MaxCellDecoratorTool


#====================================================================
# SET UP THINNING
#====================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EGAM7ThinningHelper = ThinningHelper( "EGAM7ThinningHelper" )
EGAM7ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))|HLT_e.*_Zee.*'
if globalflags.DataSource()!='geant4':
    ExtraContainersTrigger += ExtraContainersTriggerDataOnly
EGAM7ThinningHelper.AppendToStream( EGAM7Stream, ExtraContainersTrigger )

thinningTools=[]


# Truth thinning
truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EGAM7TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM7TruthThinningTool",
                                                                   ThinningService         = EGAM7ThinningHelper.ThinningSvc(),
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = False,
                                                                   PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "EGAM7 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()=='geant4':
    ToolSvc += EGAM7TruthThinningTool
    thinningTools.append(EGAM7TruthThinningTool)
print "EGAM7 thinningTools: ", thinningTools




#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam7Seq = CfgMgr.AthSequencer("EGAM7Sequence")
DerivationFrameworkJob += egam7Seq


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
egam7Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM7Kernel",
                                                         AugmentationTools = [EGAM7_GainDecoratorTool, EGAM7_MaxCellDecoratorTool] + EGAM7_ClusterEnergyPerLayerDecorators,
                                                         SkimmingTools = [EGAM7_SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
# od syntax
# addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=egam7Seq, outputGroup="EGAM7")
# new syntax
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4PV0TrackJets", "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam7Seq,"EGAM7")


#============ Create Derivation EGAM7 cell collection ==================

# Keep only calo cells associated with the egammaClusters collection
from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
                                       outputCellKey="DFEGAMCellContainer")
#========================================================================



#====================================================================
# SET UP STREAM SELECTION
#====================================================================
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM7Stream.AcceptAlgs(["EGAM7Kernel"])


#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM7SlimmingHelper = SlimmingHelper("EGAM7SlimmingHelper")

EGAM7SlimmingHelper.SmartCollections = [
				        "Electrons",
					"Photons",
					"Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"
                                        ]

# Add egamma trigger objects
EGAM7SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM7SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM7SlimmingHelper.AllVariables = ExtraContainersElectrons

if globalflags.DataSource()=='geant4':
    EGAM7SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM7SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM7_ClusterEnergyPerLayerDecorators:
    EGAM7SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM7SlimmingHelper
EGAM7SlimmingHelper.AppendContentToStream(EGAM7Stream)
