# ------------------------------------------------------------
# RunJpsiExample.py
# J. Catmore (James.Catmore@cern.ch)
# Adapted by Cameron Cuthbert (17/07/2011)
# ------------------------------------------------------------

#-------------------------------------------------------------
# User analysis steering algorithm
#-------------------------------------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiAlg
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiEEAlg
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiJpsiEEExample


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += JpsiAlg()
topSequence += JpsiEEAlg()
topSequence.JpsiAlg = Algorithm( "JpsiAlg" )
topSequence.JpsiEEAlg = Algorithm( "JpsiEEAlg" )

topSequence += JpsiJpsiEEExample(OutputLevel = INFO)

# ------------------------
# SET UP FITTER 
# ------------------------
include( "configureServices.py" )
include( "configureElectronServices.py" )

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
                                         thresholdPt                = 0.0,
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
                                         VertexPointEstimator       = VtxPointEstimator)


from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiEEFinder

ExampleJpsiEEFinder = Analysis__JpsiEEFinder(name                       = "JpsiEEFinderName",
                                             OutputLevel                = INFO,
                                             authorA                    = 1,  #1=std,2=soft,8=forward
                                             authorB                    = 1,
                                             identificationA            = 3,  #1=loose,2=medium,3=tight  x5 refers to the ++ ID. E.g. 15 is Loose++
                                             identificationB            = 3,
                                             diElectrons                = True,
                                             track1Mass                 = 0.510999,
                                             track2Mass                 = 0.510999,
                                             UseGSFCaloTracks           = False,
                                             TrackOnlyFourVec           = True,
                                             UncombinedFourVec          = False,
                                             CombinedFourVec            = False,
                                             thresholdPt                = 0.0,
                                             higherPt                   = 0.0,
                                             invMassUpper               = 100000.0,
                                             invMassLower               = 0.0,
                                             collAngleTheta             = 0.0,
                                             collAnglePhi               = 0.0,
                                             Chi2Cut                    = 200.,
                                             oppChargesOnly             = True,
                                             sameChargesOnly            = False,
                                             allChargeCombinations      = False,
                                             electronCollectionKey      = "ElectronAODCollection",
                                             TrackParticleCollection    = "TrackParticleCandidate",
                                             TrackParticleCreatorTool   = InDetParticleCreatorTool,
                                             V0VertexFitterTool         = TrkV0Fitter,             # V0 vertex fitter
                                             useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
                                             TrkVertexFitterTool        = TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                             #TrkVertexFitterTool       = InDetFullVxFitterTool,   # Full Billoir vertex fitter
                                             #TrkVertexFitterTool       = InDetFastVxFitterTool,   # Fast Billoir vertex fitter
                                             EgammaTrkRefitterTool      = electronRefitterToolGSFCalo,
                                             TrackSelectorTool          = InDetTrackSelectorToolEE,
                                             ConversionFinderHelperTool = InDetConversionHelper,
                                             VertexPointEstimator       = VtxPointEstimator)

ToolSvc += ExampleJpsiFinder
ToolSvc += ExampleJpsiEEFinder
topSequence.JpsiAlg.JpsiFinderName = ExampleJpsiFinder
topSequence.JpsiEEAlg.JpsiEEFinderName = ExampleJpsiEEFinder

JpsiJpsiEEExample.overwriteMass = False;      # keep this consistent with overwriteMass above


# Name of output file
topSequence.JpsiJpsiEEExample.outputNTupleName = "JpsiJpsiEEExample.root"

