# ------------------------------------------------------------
# RunJpsiExample.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------

from AthenaCommon.BeamFlags import jobproperties
bphysTrigCosmics = False
if jobproperties.Beam.beamType == 'cosmics':
    print "Setup Bphysics Monitoring in cosmics mode"
    bphysTrigCosmics = True

# ------------------------
# SET UP FITTER 
# ------------------------
include( "JpsiUpsilonTools/configureServices.py" )

TrkVKalVrtFitter.FirstMeasuredPoint = False

# ----------------------------------
# User's analysis requirements here:
# ----------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
ExampleJpsiFinder = Analysis__JpsiFinder(name                        = "BPhysJpsiFinder",
                                         OutputLevel                 = INFO,
                                         muAndMu                     = False if bphysTrigCosmics else True,
                                         muAndTrack                  = False,
                                         TrackAndTrack               = True if bphysTrigCosmics else False ,
                                         assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
                                         invMassUpper                = 100e3,
                                         invMassLower                = 0.0,
                                         Chi2Cut                     = 20.,
                                         oppChargesOnly              = False if bphysTrigCosmics else True,
                                         atLeastOneComb              = False if bphysTrigCosmics else True,
                                         useCombinedMeasurement      = False, # Only takes effect if combOnly=True
                                         muonCollectionKey           = "Muons",
                                         TrackParticleCollection     = "InDetTrackParticles",
                                         V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
                                         useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                         TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                         TrackSelectorTool           = InDetTrackSelectorTool,
                                         ConversionFinderHelperTool  = InDetConversionHelper,
                                         VertexPointEstimator        = VtxPointEstimator,
                                         useMCPCuts                  = False)
ToolSvc += ExampleJpsiFinder
print      ExampleJpsiFinder

#-------------------------------------------------------------
# User analysis steering algorithm
#-------------------------------------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiAlg

topSequence += JpsiAlg(JpsiCandidatesOutputName = "JpsiCandidates",
                       JpsiFinderName           = ExampleJpsiFinder,
                       OutputLevel  = INFO)
 
