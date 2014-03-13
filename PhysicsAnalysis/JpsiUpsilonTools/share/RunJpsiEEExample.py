# ------------------------------------------------------------
# RunJpsiExample.py
# J. Catmore (James.Catmore@cern.ch)
# Adapted by Cameron Cuthbert (17/07/2011)
# ------------------------------------------------------------

#-------------------------------------------------------------
# User analysis steering algorithm
#-------------------------------------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiEEAlg
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiEEExample

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += JpsiEEAlg()
topSequence.JpsiEEAlg = Algorithm( "JpsiEEAlg" )

topSequence += JpsiEEExample(OutputLevel = INFO)

# ------------------------
# SET UP FITTER 
# ------------------------
include( "configureServices.py" )
include( "configureElectronServices.py" )

# ----------------------------------
# User's analysis requirements here:
# ----------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiEEFinder

ExampleJpsiEEFinder = Analysis__JpsiEEFinder(name                       = "JpsiEEFinderName",
                                             OutputLevel                = INFO,
                                             authorA                    = 1,  #1=std,2=soft,8=forward
                                             authorB                    = 1,
                                             identificationA            = 15,  #1=loose,2=medium,3=tight  x5 refers to the ++ ID. E.g. 15 is Loose++
                                             identificationB            = 15,
                                             UsePlusPlusHelper          = True,
                                             diElectrons                = True,
                                             track1Mass                 = 0.510999,
                                             track2Mass                 = 0.510999,
                                             UseGSFCaloTracks           = False,
                                             TrackOnlyFourVec           = False,
                                             UncombinedFourVec          = True,
                                             CombinedFourVec            = False,
					     UpdateTracksWithClusterEnergy = True,
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
                                             #EgammaTrkRefitterTool      = electronRefitterToolGSFCalo,
                                             TrackSelectorTool          = InDetTrackSelectorToolEE,
                                             ConversionFinderHelperTool = InDetConversionHelper,
                                             VertexPointEstimator       = VtxPointEstimator)

ToolSvc += ExampleJpsiEEFinder
topSequence.JpsiEEAlg.JpsiEEFinderName = ExampleJpsiEEFinder

JpsiEEExample.overwriteMass = False      # keep this consistent with overwriteMass above


# Name of output file
topSequence.JpsiEEExample.outputNTupleName = "JpsiEEExample.root"

