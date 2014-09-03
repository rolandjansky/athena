# ------------------------------------------------------------
# RunJpsiExample.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------

# ------------------------
# SET UP FITTER 
# ------------------------
include( "JpsiUpsilonTools/configureServices.py" )

# ----------------------------------
# User's analysis requirements here:
# ----------------------------------
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
ExampleJpsiFinder = Analysis__JpsiFinder(name                        = "JpsiFinderName",
                                         OutputLevel                 = INFO,
                                         muAndMu                     = True,
                                         muAndTrack                  = False,
                                         TrackAndTrack               = False,
                                         assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
                                         invMassUpper                = 100000.0,
                                         invMassLower                = 0.0,
                                         Chi2Cut                     = 200.,
                                         oppChargesOnly              = True,
                                         atLeastOneComb              = True,
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
from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiExample

topSequence += JpsiAlg(JpsiCandidatesOutputName = "JpsiCandidates",
                       JpsiFinderName           = ExampleJpsiFinder)
                       
topSequence += JpsiExample(outputNTupleName = "JpsiExample.root",
                           JpsiCandidates   = topSequence.JpsiAlg.JpsiCandidatesOutputName)

