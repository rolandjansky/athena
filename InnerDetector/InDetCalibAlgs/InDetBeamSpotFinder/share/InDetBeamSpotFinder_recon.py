# $Id: InDetBeamSpotFinder_recon.py,v 1.11 2009-01-15 13:57:09 jwalder Exp $
#
# Job options fragment to run beamspot determination within a production job.
# Juerg Beringer, May 2008.


#
# The following setup follows the standard one in InDetRecPostProcessing.py, except
# for loosening the impact parameter cut d0Max in the InDetDetailedTrackSelectorTool
# and disabling the beam constraint. Note that tracking should be unbiased wrt the
# beamspot, and so we don't need to redo the standard tracking collection.
#
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
BeamSpotTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "BeamSpotDetailedTrackSelectorTool",
                                                                  pTMin                = 500,
                                                                  IPd0Max                = 50,
                                                                  IPz0Max                = 1000,
                                                                  sigIPd0Max             = 0.35,
                                                                  sigIPz0Max             = 2.5,
                                                                  etaMax               = 9999.,
                                                                  useTrackSummaryInfo  = True,
                                                                  nHitBLayer           = 0,
                                                                  nHitPix              = 1,
                                                                  nHitSct              = 5,
                                                                  nHitSi               = 7,
                                                                  nHitTrt              = 0,
                                                                  useSharedHitInfo     = False,
                                                                  useTrackQualityInfo  = True,
                                                                  fitChi2OnNdfMax      = 3.5,
                                                                  TrackSummaryTool     = InDetTrackSummaryTool,
                                                                  Extrapolator         = InDetExtrapolator)
ToolSvc += BeamSpotTrackSelectorTool


from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
BeamSpotPriVxFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "BeamSpotAdaptiveMultiPriVxFinderTool",
                                                                   VertexFitterTool  = InDetVxFitterTool,
                                                                   TrackSelector     = BeamSpotTrackSelectorTool,
                                                                   useBeamConstraint = False,
                                                                   selectiontype     = 0)
ToolSvc += BeamSpotPriVxFinderTool


from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
BeamSpotPriVxFinder = InDet__InDetPriVxFinder(name                   = "BeamSpotPriVxFinder",
                                              VertexFinderTool       = BeamSpotPriVxFinderTool,
                                              TracksName             = InDetKeys.Tracks(),
                                              VxCandidatesOutputName = "BeamSpotVxPrimaryCandidate")
topSequence += BeamSpotPriVxFinder


#
# Setup of beamspot finder(s)
#
doVertexBasedBeamSpot = False 
doBeamSpotHistogramming = False

if doVertexBasedBeamSpot:
    from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotVertex
    InDetBeamSpotVertex = InDet__InDetBeamSpotVertex(VertexContainer = "BeamSpotVxPrimaryCandidate",
						     MinTracksPerVtx = 10,
						     MinVtxNum = 10,
                                                     DoHists   = doBeamSpotHistogramming)
    ToolSvc +=  InDetBeamSpotVertex
else:
    from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotTrackChi2
    InDetBeamSpotTrackChi2 = InDet__InDetBeamSpotTrackChi2(PtCut = 1500.0,
                                                           D0Match_0_Cut  = 1000.0,
                                                           D0Match_1_Cut  = 10.0,
                                                           D0Match_2_Cut  = 3.0,
                                                           BL_X_seed      = 0.0,
                                                           BL_a_seed      = 0.0,
                                                           BL_Y_seed      = 0.0,
                                                           BL_b_seed      = 0.0,
                                                           nTracks_0      = 500,
                                                           nTracks_1      = 2000,
                                                           nTracks        = 1000,
                                                           ErrorFactor_0  = 100.0,
                                                           ErrorFactor_1  = 10.0,
                                                           DoHists   = doBeamSpotHistogramming)
    ToolSvc +=  InDetBeamSpotTrackChi2

if doVertexBasedBeamSpot:
  InDetBeamSpotTool = InDetBeamSpotVertex
else:
  InDetBeamSpotTool = InDetBeamSpotTrackChi2

# For output to a Database
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotDbWriterTool
InDetBeamSpotDbWriterTool = InDet__InDetBeamSpotDbWriterTool(Tag = "nominal",
							     OutputLevel = INFO)
ToolSvc += InDetBeamSpotDbWriterTool

from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotFinder as InDetBeamSpotFinder
InDetBeamSpotFinder = InDetBeamSpotFinder(BeamSpotTool    = InDetBeamSpotTool,
					  BeamSpotWriterTool = InDetBeamSpotDbWriterTool,
					  MaxCount        = -1, 
                                          LumiRange       = 1,
                                          RunRange        = 1,
					  # General
                                          DoHists         = doBeamSpotHistogramming,
                                          WriteDb         = True,
                                          UseDefaultValues = True,
                                          Default_SigmaX  = 1.0,
                                          Default_SigmaY  = 1.0,
                                          Default_SigmaZ  = 53.0,
					  Default_SigmaXY = 0.0,
 					  #ForceRunNumber  = 123456,
					  #ForceRunRange   = 1,
                                          OutputLevel     = INFO)
topSequence += InDetBeamSpotFinder


#
# Setup writing out of beamspot into COOL file
#
IOVDbSvc = Service("IOVDbSvc")
#IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=beamspot.db;X:TESTCOOL"
IOVDbSvc.dbConnection  = "sqlite://;schema=beamspot.db;dbname=BEAMSPOT"

# Call the histogramming service
if doBeamSpotHistogramming:
    # produce ROOT ntuple using THistSvc
    if not hasattr(ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Output+=[ "INDETBEAMSPOTFINDER DATAFILE='beamspot.root' OPT='RECREATE'"]



#
# Print beamspot configuration
#
print 'Configuratin for beamspot determination:'
print BeamSpotTrackSelectorTool
print BeamSpotPriVxFinderTool
print BeamSpotPriVxFinder
print InDetBeamSpotFinder

