# ------------------------------------------------------------
# RunJpsiExample.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------

#-------------------------------------------------------------
# User analysis steering algorithm
#-------------------------------------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiAlg
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiExample

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += JpsiAlg()
topSequence.JpsiAlg = Algorithm( "JpsiAlg" )

topSequence += JpsiExample(OutputLevel = INFO)

# ------------------------
# SET UP FITTER 
# ------------------------
include( "configureServices.py" )

# ----------------------------------
# User's analysis requirements here:
# ----------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
ExampleJpsiFinder = Analysis__JpsiFinder(name                       = "JpsiFinderName",
                                         OutputLevel                = INFO,
					 muAndMu	            = True,
					 muAndTrack	            = False,
					 TrackAndTrack	            = False,
                                         assumeDiMuons              = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
				    	 track1Mass                 = 105.66,  # Ignored if above set to true
					 track2Mass		    = 105.66,  # Ignored if above set to true 	
                                         muonThresholdPt            = 0.0,
                                         higherPt                   = 0.0,
                                         invMassUpper               = 100000.0,
                                         invMassLower               = 0.0,
                                         collAngleTheta             = 0.0,
                                         collAnglePhi               = 0.0,
                                         Chi2Cut                    = 200.,
					 oppChargesOnly	            = True,
					 sameChargesOnly            = False,
					 allChargeCombinations      = False,
                                         allMuons                   = False,
                                         combOnly                   = False,
                                         atLeastOneComb             = True,
					 useCombinedMeasurement     = False, # Only takes effect if combOnly=True	
                                         muonCollectionKey          = "StacoMuonCollection",
					 TrackParticleCollection    = "TrackParticleCandidate",
                                         V0VertexFitterTool         = TrkV0Fitter,             # V0 vertex fitter
                                         useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
                                         TrkVertexFitterTool        = TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                         #TrkVertexFitterTool        = InDetFullVxFitterTool,   # Full Billoir vertex fitter
                                         #TrkVertexFitterTool        = InDetFastVxFitterTool,   # Fast Billoir vertex fitter
                                         TrackSelectorTool          = InDetTrackSelectorTool,
                                         ConversionFinderHelperTool = InDetConversionHelper,
                                         VertexPointEstimator       = VtxPointEstimator,
					 useMCPCuts		    = True)
ToolSvc += ExampleJpsiFinder
print      ExampleJpsiFinder
topSequence.JpsiAlg.JpsiFinderName = ExampleJpsiFinder

JpsiExample.overwriteMass = False;      # keep this consistent with overwriteMass above

# Name of output file
topSequence.JpsiExample.outputNTupleName = "JpsiExample.root"

