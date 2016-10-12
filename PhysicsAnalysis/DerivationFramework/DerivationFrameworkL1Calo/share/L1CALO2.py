#====================================================================
# L1CALO2.py 
# reductionConf flag L1CALO2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
#from DerivationFrameworkInDet.InDetCommon import *
#from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

#from DerivationFrameworkEGamma.EGammaCommon import *
from AthenaCommon.GlobalFlags import globalflags

expression = "true"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
L1CALO2StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "L1CALO2StringSkimmingTool", expression = expression)

ToolSvc += L1CALO2StringSkimmingTool
print "L1CALO2.py L1CALO2StringSkimmingTool: ", L1CALO2StringSkimmingTool
'''
from DerivationFrameworkL1Calo.DerivationFrameworkL1CaloConf import DerivationFramework__SaturatedTriggerTower
L1CALO2SkimmingTool = DerivationFramework__SaturatedTriggerTower(   name                    = "SaturatedTowerTool",
                                                                  adcThreshold            = 900)
ToolSvc += L1CALO2SkimmingTool
'''
#=======================================
# THINNING
#=======================================
thinningTools=[]


augmentationTools=[]
#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel( "L1CALO2Kernel",
                                                                        SkimmingTools = [L1CALO2StringSkimmingTool,
#										L1CALO2SkimmingTool
										],
                                                                        ThinningTools = thinningTools,
                                                                        AugmentationTools = augmentationTools)
#====================================================================
# SET UP STREAM   
#====================================================================
#streamName = derivationFlags.WriteDAOD_L1CALO2Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_L1CALO2Stream )
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_L1CALO2.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_L1CALO2 )
L1CALO2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
L1CALO2Stream.AcceptAlgs(["L1CALO2Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="L1CALO2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
L1CALO2SlimmingHelper = SlimmingHelper("L1CALO2SlimmingHelper")

##Smart Slimming
L1CALO2SlimmingHelper.SmartCollections = ["Electrons",
					  "Photons",
					  "PrimaryVertices",
#					  "MET_Reference_AntiKt4EMTopo",
#					  "AntiKt4EMTopoJets",
					  ]
L1CALO2SlimmingHelper.ExtraVariables = [
#AntiKt4EMTopoJets Smart Slimming List:
					"Kt4EMTopoEventShape",
					"Kt4EMTopoEventShapeAux.Density",
					"AntiKt4EMTopoJets",
					"AntiKt4EMTopoJetsAux.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.HECFrac.HECQuality.JVF.LArQuality.NegativeE.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.Timing.TrackWidthPt1000.Width.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.EMFrac.EnergyPerSampling.FracSamplingMax.GhostMuonSegment.GhostMuonSegmentCount.OriginCorrected.PileupCorrected.DetectorEta.JetOriginConstitScaleMomentum_pt.JetPileupScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_m.JetEtaJESScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_m.JetGSCScaleMomentum_pt.JetGSCScaleMomentum_eta.JetGSCScaleMomentum_phi.JetGSCScaleMomentum_m.JetInsituScaleMomentum_pt.JetInsituScaleMomentum_eta.JetInsituScaleMomentum_phi.JetInsituScaleMomentum_m.constituentLinks.btaggingLink.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.GhostTruthAssociationLink.HighestJVFVtx.JetOriginConstitScaleMomentum_eta.JetOriginConstitScaleMomentum_m.JetOriginConstitScaleMomentum_phi.JvtJvfcorr.JvtRpt.Jvt.FracSamplingMaxIndex.LeadingClusterPt.ECPSFraction.N90Constituents.LeadingClusterSecondLambda.LeadingClusterCenterLambda.LeadingClusterSecondR.CentroidR.OotFracClusters5.OotFracClusters10.ConeTruthLabelID.PartonTruthLabelID",
					"MuonSegments",
					"MuonSegmentsAux.",
					"PrimaryVertices",
					"PrimaryVerticesAux.trackParticleLinks.neutralParticleLinks"
#MET_Reference_AntiKt4EMTopo SS List:
					"AntiKt4EMTopoJets",
					"AntiKt4EMTopoJetsAux.originalObjectLink.pt.eta.phi.m.constituentLinks.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.btaggingLink.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostMuonSegment.GhostTrack.GhostTruthAssociationLink.HighestJVFLooseVtx.HighestJVFVtxGhost.AntiKt2TrackJet.GhostAntiKt2TrackJet.HighestJVFVtx.JetLCScaleMomentum_pt.JetLCScaleMomentum_eta.JetLCScaleMomentum_phi.JetLCScaleMomentum_m",
					"Electrons",
					"METAssoc_AntiKt4EMTopo",
					"METAssoc_AntiKt4EMTopoAux.jetLink.objectLinks.calkey.calpx.calpy.calpz.cale.calsumpt.trkkey.trkpx.trkpy.trkpz.trke.trksumpt.jettrkpx.jettrkpy.jettrkpz.jettrke.jettrksumpt.overlapIndices.overlapTypes.isMisc",
					"MET_Core_AntiKt4EMTopo",
					"MET_Core_AntiKt4EMTopoAux.name.mpx.mpy.sumet.source",
					"MET_Reference_AntiKt4EMTopo",
					"MET_Reference_AntiKt4EMTopoAux.name.mpx.mpy.sumet.source",
					"MET_Truth",
					"MET_TruthAux.name.mpx.mpy.sumet.source",
					"Muons",
					"MuonsAux.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus.muonType",
					"Photons",
					"TauJets",
					"PrimaryVertices",
					"PrimaryVerticesAux.vertexType.trackParticleLinks"
					]
L1CALO2SlimmingHelper.AllVariables = ["egammaClusters",
		                      "CaloCalTopoClusters",
				      "TauPi0Clusters",
				      "TauJets",
				      "CMXCPHits",
				      "CMXCPTobs",
				      "CMXEtSums",
				      "CMXJetHits",
				      "CMXJetTobs",
				      "CMXRoIs",
				      
				      "CPMTobRoIs",
				      "CPMTobRoIsRoIB",
				      "CPMTowers",
				      "CPMTowersOverlap",
				      "JEMEtSums",
				      "JEMTobRoIs",
				      "JEMTobRoIsRoIB",
			      
				      "JetElements",
				      "JetElementsOverlap",
				      "LVL1EmTauRoIs",
				      "LVL1EnergySumRoIs",
				      "LVL1JetEtRoIs",
				      "LVL1JetRoIs",
				      "LVL1MuonRoIs",
				      "RODHeaders",
				      
				      "xAODTriggerTowers",

				      "L1TopoRawData",
				      
				      ]

L1CALO2SlimmingHelper.IncludeMuonTriggerContent   = True
L1CALO2SlimmingHelper.IncludeEGammaTriggerContent = True
L1CALO2SlimmingHelper.IncludeJetTriggerContent    = True
L1CALO2SlimmingHelper.IncludeTauTriggerContent    = True
L1CALO2SlimmingHelper.IncludeEtMissTriggerContent = True
L1CALO2SlimmingHelper.IncludeBJetTriggerContent = True
L1CALO2SlimmingHelper.IncludeBPhysTriggerContent = True
L1CALO2SlimmingHelper.IncludeMinBiasTriggerContent = True


L1CALO2SlimmingHelper.AppendContentToStream(L1CALO2Stream)

