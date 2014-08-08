########################################
# Jet finding
########################################
# in case we want to do the JetFinding based
# on TrackParticles and not take the
# ESD JetCollection
# or
# AOD ParticleJetContainer
if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags

theApp.Dlls += [ "JetRec" ]
theApp.topAlg += [ "JetAlgorithm/BTagKtJets" ]
BTagKtJets = Algorithm( "BTagKtJets" )
BTagKtJets.InputCollectionNames = [ "TrackParticleCandidate" ]
BTagKtJets.JetCollectionName = "BTAGJetCollection"

BTagKtJets.AlgTools = [
        "JetSignalSelectorTool/InitialEtCut",
        "JetPreClusterTool/PreClustering",
        "JetKtFinderTool/KtFinder",
        "JetSignalSelectorTool/FinalEtCut" 
        ]

BTagKtJets.PreClustering.OutputLevel   = WARNING
BTagKtJets.PreClustering.ClusterRadius = 0.2
BTagKtJets.KtFinder.BeamType = "PP"
BTagKtJets.KtFinder.DistScheme = "DeltaR"
BTagKtJets.KtFinder.RecomScheme = "E"  # four vector addition
BTagKtJets.KtFinder.RParameter = 0.7   # defines the extend of the jet
BTagKtJets.InitialEtCut.UseTransverseEnergy = True
BTagKtJets.InitialEtCut.MinimumSignal       = 300*MeV
BTagKtJets.FinalEtCut.UseTransverseEnergy = True
BTagKtJets.FinalEtCut.MinimumSignal       = 15.*GeV # Marseille
BTagKtJets.FinalEtCut.OutputLevel = 3
BTagKtJets.OutputLevel = 3
  
# use this line if you want to have more info in the CBNT about the Jets used for b-tagging
if BTaggingFlags.doBTagJetCBNT:
  include( "BTagging/CBNT_BTagJetFinder_jobOptions.py" )

