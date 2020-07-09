# Requires InDetRec or similar to already be loaded

from AthenaCommon.AppMgr import ToolSvc

#import copy
#InDetPriVxFinderTool_Pix = copy.deepcopy(ToolSvc.InDetPriVxFinderTool)
#InDetPriVxFinderTool_SCT = copy.deepcopy(ToolSvc.InDetPriVxFinderTool)

from AthenaCommon.JobProperties import jobproperties
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#MSMgr.Print()
try:
    StreamName = jobproperties.PrimaryDPDFlags.WriteDESDM_BEAMSPOTStream.StreamName
except:
    StreamName = "StreamDESDM_BEAMSPOT"
#print xxx
try:
    Stream = MSMgr.GetStream(StreamName)
except:
    raise NameError("Stream %s undefined!"%StreamName)
    
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool_Pix = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool_Pix",
                                                                   pTMin               =    InDetPrimaryVertexingCuts.minPT(),
                                                                   IPd0Max             =    InDetPrimaryVertexingCuts.IPd0Max(),
                                                                   IPz0Max             =    InDetPrimaryVertexingCuts.IPz0Max(),
                                                                   z0Max               =    InDetPrimaryVertexingCuts.z0Max(),
                                                                   sigIPd0Max              =    InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                                   sigIPz0Max              =    InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                                   d0significanceMax           =    InDetPrimaryVertexingCuts.d0significanceMax(),
                                                                   z0significanceMax           =    InDetPrimaryVertexingCuts.z0significanceMax(),
                                                                   etaMax              =    InDetPrimaryVertexingCuts.etaMax(),
                                                                   useTrackSummaryInfo         =    InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                                   nHitBLayer              =    InDetPrimaryVertexingCuts.nHitBLayer(),
                                                                   nHitPix             =    InDetPrimaryVertexingCuts.nHitPix(),
                                                                   nHitBLayerPlusPix           =    InDetPrimaryVertexingCuts.nHitBLayerPlusPix(),
                                                                   nHitSct             =    0,
                                                                   nHitSi              =    0,
                                                                   nHitTrt             =    0,
                                                                   nHitTrtHighEFractionMax     =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                                   nHitTrtHighEFractionWithOutliersMax =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                                   useSharedHitInfo            =    InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                                   useTrackQualityInfo         =    InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                                   fitChi2OnNdfMax         =    InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                                   TrtMaxEtaAcceptance         =    InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                                   TrackSummaryTool            =    InDetTrackSummaryTool,
                                                                   Extrapolator            =    InDetExtrapolator)


ToolSvc += InDetTrackSelectorTool_Pix
if (InDetFlags.doPrintConfigurables()):
    print InDetTrackSelectorTool_Pix
    

# -----------------------------------------
#
# ----- load primary vertex finder tool
#
# -----------------------------------------

if (not (InDetFlags.primaryVertexSetup() == 'AdaptiveFinding') and
    not (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding') and
    not (InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding')):
    #
    # --- load primary vertex finder tool
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
    InDetPriVxFinderTool_Pix = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool_Pix",
                                                       PriVxSeedFinder   = InDetMultiSeedFinder,
                                                       TrackSelector     = InDetTrackSelectorTool_Pix,
                                                       VertexFitterTool  = InDetVxFitterTool,
                                                       chi2CutMethod     = 2,
                                                       useBeamConstraint = InDetFlags.useBeamConstraint())
    if jobproperties.Beam.numberOfCollisions() > 2.0: # 900 GeV running at "zero lumi" has 2.0 for this variable
        InDetPriVxFinderTool_Pix.enableMultipleVertices = 1;
    else:
        InDetPriVxFinderTool_Pix.enableMultipleVertices = 0;

    if rec.Commissioning():
      InDetPriVxFinderTool_Pix.maxChi2PerTrack = 15.
    
elif InDetFlags.primaryVertexSetup() == 'AdaptiveFinding':
    #
    # --- load adaptive primary vertex finder
    #

    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    InDetPriVxFinderTool_Pix = InDet__InDetIterativePriVxFinderTool(name             = "InDetIterativePriVxFinderTool_Pix",
                                                                VertexFitterTool = InDetVxFitterTool,
                                                                TrackSelector    = InDetTrackSelectorTool_Pix,
                                                                SeedFinder = InDetVtxSeedFinder,
                                                                ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                                                LinearizedTrackFactory = InDetLinFactory,
                                                                useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                significanceCutSeeding = 12,
                                                                maximumChi2cutForSeeding = 49,
                                                                maxVertices = 25
                                                                )
                                  
        
elif InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
    #
    # --- load adaptive multi primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
    InDetPriVxFinderTool_Pix = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool_Pix",
                                                                    SeedFinder        = InDetVtxSeedFinder,
                                                                    VertexFitterTool  = InDetVxFitterTool,
                                                                    TrackSelector     = InDetTrackSelectorTool_Pix,
                                                                    useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                    selectiontype     = 0,
                                                                    do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
  
elif InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding':
    #
    # --- load vkal vertex finder tool
    #
    from InDetVKalPriVxFinderTool.InDetVKalPriVxFinderTool import InDet__InDetVKalPriVxFinderTool
    InDetPriVxFinderTool_Pix = InDet__InDetVKalPriVxFinderTool(name                   = "InDetVKalPriVxFinder_Pix",
                                                           TrackSummaryTool       = InDetTrackSummaryTool,
                                                           FitterTool             = InDetVxFitterTool,
                                                           BeamConstraint         = 0)
    if InDetFlags.useBeamConstraint():
      InDetPriVxFinderTool_Pix.BeamConstraint = 1

ToolSvc += InDetPriVxFinderTool_Pix
if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinderTool_Pix






from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
InDetPriVxFinder_Pix = InDet__InDetPriVxFinder(name                        = "InDetPriVxFinder_Pix",
                                               VertexFinderTool            = InDetPriVxFinderTool_Pix,
                                               TracksName                  = "ResolvedPixelTracks",
                                               VxCandidatesOutputName      = InDetKeys.PrimaryVertices()+'_Pix',
                                               VertexCollectionSortingTool = VertexCollectionSortingTool,
                                               doVertexSorting             = doSorting,
                                               OutputLevel = DEBUG)

topSequence += InDetPriVxFinder_Pix
if InDetFlags.doPrintConfigurables():
    print InDetPriVxFinder_Pix

print "JW" 
print InDetTrackSelectorTool_Pix
print InDetPriVxFinderTool_Pix
print InDetPriVxFinder_Pix
Stream.AddItem([ 'VxContainer#VxPrimaryCandidate_Pix' ])







from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool_SCT = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool_SCT",
                                                                   pTMin               =    InDetPrimaryVertexingCuts.minPT(),
                                                                   IPd0Max             =    InDetPrimaryVertexingCuts.IPd0Max(),
                                                                   IPz0Max             =    InDetPrimaryVertexingCuts.IPz0Max(),
                                                                   z0Max               =    InDetPrimaryVertexingCuts.z0Max(),
                                                                   sigIPd0Max              =    InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                                   sigIPz0Max              =    InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                                   d0significanceMax           =    InDetPrimaryVertexingCuts.d0significanceMax(),
                                                                   z0significanceMax           =    InDetPrimaryVertexingCuts.z0significanceMax(),
                                                                   etaMax              =    InDetPrimaryVertexingCuts.etaMax(),
                                                                   useTrackSummaryInfo         =    InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                                   nHitBLayer              = 0,
                                                                   nHitPix             =    0, 
                                                                   nHitBLayerPlusPix   =  0,
                                                                   nHitSct             =   InDetPrimaryVertexingCuts.nHitSct() ,
                                                                   nHitSi              =   InDetPrimaryVertexingCuts.nHitSi() ,
                                                                   nHitTrt             =   0,
                                                                   nHitTrtHighEFractionMax     =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                                   nHitTrtHighEFractionWithOutliersMax =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                                   useSharedHitInfo            =    InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                                   useTrackQualityInfo         =    InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                                   fitChi2OnNdfMax         =    InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                                   TrtMaxEtaAcceptance         =    InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                                   TrackSummaryTool            =    InDetTrackSummaryTool,
                                                                   Extrapolator            =    InDetExtrapolator)


ToolSvc += InDetTrackSelectorTool_SCT
if (InDetFlags.doPrintConfigurables()):
    print InDetTrackSelectorTool_SCT
    

# -----------------------------------------
#
# ----- load primary vertex finder tool
#
# -----------------------------------------

if (not (InDetFlags.primaryVertexSetup() == 'AdaptiveFinding') and
    not (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding') and
    not (InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding')):
    #
    # --- load primary vertex finder tool
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
    InDetPriVxFinderTool_SCT = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool_SCT",
                                                       PriVxSeedFinder   = InDetMultiSeedFinder,
                                                       TrackSelector     = InDetTrackSelectorTool_SCT,
                                                       VertexFitterTool  = InDetVxFitterTool,
                                                       chi2CutMethod     = 2,
                                                       useBeamConstraint = InDetFlags.useBeamConstraint())
    if jobproperties.Beam.numberOfCollisions() > 2.0: # 900 GeV running at "zero lumi" has 2.0 for this variable
        InDetPriVxFinderTool_SCT.enableMultipleVertices = 1;
    else:
        InDetPriVxFinderTool_SCT.enableMultipleVertices = 0;

    if rec.Commissioning():
      InDetPriVxFinderTool_SCT.maxChi2PerTrack = 15.
    
elif InDetFlags.primaryVertexSetup() == 'AdaptiveFinding':
    # --- load adaptive primary vertex finder
    #
    #from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptivePriVxFinderTool
    #InDetPriVxFinderTool_Si = InDet__InDetAdaptivePriVxFinderTool(name             = "InDetAdaptivePriVxFinderTool_Si",
    #                                                              VertexFitterTool = InDetVxFitterTool,
    #                                                              TrackSelector    = InDetTrackSelectorTool_Si
    #                                                              )

    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    InDetPriVxFinderTool_SCT = InDet__InDetIterativePriVxFinderTool(name             = "InDetIterativePriVxFinderTool_SCT",
                                                                VertexFitterTool = InDetVxFitterTool,
                                                                TrackSelector    = InDetTrackSelectorTool_SCT,
                                                                SeedFinder = InDetVtxSeedFinder,
                                                                ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                                                LinearizedTrackFactory = InDetLinFactory,
                                                                useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                significanceCutSeeding = 12,
                                                                maximumChi2cutForSeeding = 49,
                                                                maxVertices = 25
                                                                )
                                  
elif InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
    #
    # --- load adaptive multi primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
    InDetPriVxFinderTool_SCT = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool_SCT",
                                                                    SeedFinder        = InDetVtxSeedFinder,
                                                                    VertexFitterTool  = InDetVxFitterTool,
                                                                    TrackSelector     = InDetTrackSelectorTool_SCT,
                                                                    useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                    selectiontype     = 0,
                                                                    do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
  

ToolSvc += InDetPriVxFinderTool_SCT
if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinderTool_SCT







from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
InDetPriVxFinder_SCT = InDet__InDetPriVxFinder(name                        = "InDetPriVxFinder_SCT",
                                               VertexFinderTool            = InDetPriVxFinderTool_SCT,
                                               TracksName                  = "ResolvedSCTTracks",
                                               VxCandidatesOutputName      = InDetKeys.PrimaryVertices()+'_SCT',
                                               VertexCollectionSortingTool = VertexCollectionSortingTool,
                                               doVertexSorting             = doSorting)

topSequence += InDetPriVxFinder_SCT
if InDetFlags.doPrintConfigurables():
        print InDetPriVxFinder_SCT
        



print "JW"
print InDetTrackSelectorTool_SCT
print InDetPriVxFinderTool_SCT
print InDetPriVxFinder_SCT
Stream.AddItem([ 'VxContainer#VxPrimaryCandidate_SCT' ])













from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool_TRT = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool_TRT",
                                                                   pTMin               =    InDetPrimaryVertexingCuts.minPT(),
                                                                   IPd0Max             =    InDetPrimaryVertexingCuts.IPd0Max(),
                                                                   IPz0Max             =    InDetPrimaryVertexingCuts.IPz0Max(),
                                                                   z0Max               =    InDetPrimaryVertexingCuts.z0Max(),
                                                                   sigIPd0Max              =    InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                                   sigIPz0Max              =    InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                                   d0significanceMax           =    InDetPrimaryVertexingCuts.d0significanceMax(),
                                                                   z0significanceMax           =    InDetPrimaryVertexingCuts.z0significanceMax(),
                                                                   etaMax              =    InDetPrimaryVertexingCuts.etaMax(),
                                                                   useTrackSummaryInfo         =    InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                                   nHitBLayer              = 0,
                                                                   nHitPix             =    0, 
                                                                   nHitBLayerPlusPix   =  0,
                                                                   nHitSct             =   0 ,
                                                                   nHitSi              =   0 ,
                                                                   nHitTrt             =   InDetPrimaryVertexingCuts.nHitTrt(),
                                                                   nHitTrtHighEFractionMax     =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                                   nHitTrtHighEFractionWithOutliersMax =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                                   useSharedHitInfo            =    InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                                   useTrackQualityInfo         =    InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                                   fitChi2OnNdfMax         =    InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                                   TrtMaxEtaAcceptance         =    InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                                   TrackSummaryTool            =    InDetTrackSummaryTool,
                                                                   Extrapolator            =    InDetExtrapolator)


ToolSvc += InDetTrackSelectorTool_TRT
if (InDetFlags.doPrintConfigurables()):
    print InDetTrackSelectorTool_TRT
    

# -----------------------------------------
#
# ----- load primary vertex finder tool
#
# -----------------------------------------

if (not (InDetFlags.primaryVertexSetup() == 'AdaptiveFinding') and
    not (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding') and
    not (InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding')):
    #
    # --- load primary vertex finder tool
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
    InDetPriVxFinderTool_TRT = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool_TRT",
                                                       PriVxSeedFinder   = InDetMultiSeedFinder,
                                                       TrackSelector     = InDetTrackSelectorTool_TRT,
                                                       VertexFitterTool  = InDetVxFitterTool,
                                                       chi2CutMethod     = 2,
                                                       useBeamConstraint = InDetFlags.useBeamConstraint())
    if jobproperties.Beam.numberOfCollisions() > 2.0: # 900 GeV running at "zero lumi" has 2.0 for this variable
        InDetPriVxFinderTool_TRT.enableMultipleVertices = 1;
    else:
        InDetPriVxFinderTool_TRT.enableMultipleVertices = 0;

    if rec.Commissioning():
      InDetPriVxFinderTool_TRT.maxChi2PerTrack = 15.
    
elif InDetFlags.primaryVertexSetup() == 'AdaptiveFinding':
 
    # --- load adaptive primary vertex finder
    #
    #from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptivePriVxFinderTool
    #InDetPriVxFinderTool_Si = InDet__InDetAdaptivePriVxFinderTool(name             = "InDetAdaptivePriVxFinderTool_Si",
    #                                                              VertexFitterTool = InDetVxFitterTool,
    #                                                              TrackSelector    = InDetTrackSelectorTool_Si
    #                                                              )

    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    InDetPriVxFinderTool_TRT = InDet__InDetIterativePriVxFinderTool(name             = "InDetIterativePriVxFinderTool_TRT",
                                                                VertexFitterTool = InDetVxFitterTool,
                                                                TrackSelector    = InDetTrackSelectorTool_TRT,
                                                                SeedFinder = InDetVtxSeedFinder,
                                                                ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                                                LinearizedTrackFactory = InDetLinFactory,
                                                                useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                significanceCutSeeding = 12,
                                                                maximumChi2cutForSeeding = 49,
                                                                maxVertices = 25
                                                                )
                                  
                           
elif InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
    #
    # --- load adaptive multi primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
    InDetPriVxFinderTool_TRT = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool_TRT",
                                                                    SeedFinder        = InDetVtxSeedFinder,
                                                                    VertexFitterTool  = InDetVxFitterTool,
                                                                    TrackSelector     = InDetTrackSelectorTool_TRT,
                                                                    useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                    selectiontype     = 0,
                                                                    do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
  
elif InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding':
    #
    # --- load vkal vertex finder tool
    #
    from InDetVKalPriVxFinderTool.InDetVKalPriVxFinderTool import InDet__InDetVKalPriVxFinderTool
    InDetPriVxFinderTool_TRT = InDet__InDetVKalPriVxFinderTool(name                   = "InDetVKalPriVxFinder_TRT",
                                                           TrackSummaryTool       = InDetTrackSummaryTool,
                                                           FitterTool             = InDetVxFitterTool,
                                                           BeamConstraint         = 0)
    if InDetFlags.useBeamConstraint():
      InDetPriVxFinderTool_TRT.BeamConstraint = 1

ToolSvc += InDetPriVxFinderTool_TRT
if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinderTool_TRT







from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
InDetPriVxFinder_TRT = InDet__InDetPriVxFinder(name                        = "InDetPriVxFinder_TRT",
                                               VertexFinderTool            = InDetPriVxFinderTool_TRT,
                                               TracksName                  = "StandaloneTRTTracks",
                                               VxCandidatesOutputName      = InDetKeys.PrimaryVertices()+'_TRT',
                                               VertexCollectionSortingTool = VertexCollectionSortingTool,
                                               doVertexSorting             = doSorting)

topSequence += InDetPriVxFinder_TRT
if InDetFlags.doPrintConfigurables():
        print InDetPriVxFinder_TRT
        



print "JW"
print InDetTrackSelectorTool_TRT
print InDetPriVxFinderTool_TRT
print InDetPriVxFinder_TRT
Stream.AddItem([ 'VxContainer#VxPrimaryCandidate_TRT' ])









        
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool_Si = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool_Si",
                                                                  pTMin               =    InDetPrimaryVertexingCuts.minPT(),
                                                                  IPd0Max             =    InDetPrimaryVertexingCuts.IPd0Max(),
                                                                  IPz0Max             =    InDetPrimaryVertexingCuts.IPz0Max(),
                                                                  z0Max               =    InDetPrimaryVertexingCuts.z0Max(),
                                                                  sigIPd0Max              =    InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                                  sigIPz0Max              =    InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                                  d0significanceMax           =    InDetPrimaryVertexingCuts.d0significanceMax(),
                                                                  z0significanceMax           =    InDetPrimaryVertexingCuts.z0significanceMax(),
                                                                  etaMax              =    InDetPrimaryVertexingCuts.etaMax(),
                                                                  useTrackSummaryInfo         =    InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                                  nHitBLayer              =    InDetPrimaryVertexingCuts.nHitBLayer(),
                                                                  nHitPix             =    InDetPrimaryVertexingCuts.nHitPix(),
                                                                  nHitBLayerPlusPix           =    InDetPrimaryVertexingCuts.nHitBLayerPlusPix(),
                                                                  nHitSct             =   InDetPrimaryVertexingCuts.nHitSct() ,
                                                                  nHitSi              =   InDetPrimaryVertexingCuts.nHitSi() ,
                                                                  nHitTrt             =    0,
                                                                  nHitTrtHighEFractionMax     =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                                  nHitTrtHighEFractionWithOutliersMax =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                                  useSharedHitInfo            =    InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                                  useTrackQualityInfo         =    InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                                  fitChi2OnNdfMax         =    InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                                  TrtMaxEtaAcceptance         =    InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                                  TrackSummaryTool            =    InDetTrackSummaryTool,
                                                                  Extrapolator            =    InDetExtrapolator)


ToolSvc += InDetTrackSelectorTool_Si
if (InDetFlags.doPrintConfigurables()):
    print InDetTrackSelectorTool_Si
    

# -----------------------------------------
#
# ----- load primary vertex finder tool
#
# -----------------------------------------

if (not (InDetFlags.primaryVertexSetup() == 'AdaptiveFinding') and
    not (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding') and
    not (InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding')):
    #
    # --- load primary vertex finder tool
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
    InDetPriVxFinderTool_Si = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool_Si",
                                                       PriVxSeedFinder   = InDetMultiSeedFinder,
                                                       TrackSelector     = InDetTrackSelectorTool_Si,
                                                       VertexFitterTool  = InDetVxFitterTool,
                                                       chi2CutMethod     = 2,
                                                       useBeamConstraint = InDetFlags.useBeamConstraint())
    if jobproperties.Beam.numberOfCollisions() > 2.0: # 900 GeV running at "zero lumi" has 2.0 for this variable
        InDetPriVxFinderTool_Si.enableMultipleVertices = 1;
    else:
        InDetPriVxFinderTool_Si.enableMultipleVertices = 0;

    if rec.Commissioning():
      InDetPriVxFinderTool_Si.maxChi2PerTrack = 15.
    
elif InDetFlags.primaryVertexSetup() == 'AdaptiveFinding':
    #
    # --- load adaptive primary vertex finder
    #
    #from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptivePriVxFinderTool
    #InDetPriVxFinderTool_Si = InDet__InDetAdaptivePriVxFinderTool(name             = "InDetAdaptivePriVxFinderTool_Si",
    #                                                              VertexFitterTool = InDetVxFitterTool,
    #                                                              TrackSelector    = InDetTrackSelectorTool_Si
    #                                                              )

    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    InDetPriVxFinderTool_Si = InDet__InDetIterativePriVxFinderTool(name             = "InDetIterativePriVxFinderTool_Si",
                                                                VertexFitterTool = InDetVxFitterTool,
                                                                TrackSelector    = InDetTrackSelectorTool_Si,
                                                                SeedFinder = InDetVtxSeedFinder,
                                                                ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                                                LinearizedTrackFactory = InDetLinFactory,
                                                                useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                significanceCutSeeding = 12,
                                                                maximumChi2cutForSeeding = 49,
                                                                maxVertices = 25
                                                                )

    
elif InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
    #
    # --- load adaptive multi primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
    InDetPriVxFinderTool_Si = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool_Si",
                                                                    SeedFinder        = InDetVtxSeedFinder,
                                                                    VertexFitterTool  = InDetVxFitterTool,
                                                                    TrackSelector     = InDetTrackSelectorTool_Si,
                                                                    useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                    selectiontype     = 0,
                                                                    do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
  
elif InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding':
    #
    # --- load vkal vertex finder tool
    #
    from InDetVKalPriVxFinderTool.InDetVKalPriVxFinderTool import InDet__InDetVKalPriVxFinderTool
    InDetPriVxFinderTool_Si = InDet__InDetVKalPriVxFinderTool(name                   = "InDetVKalPriVxFinder_Si",
                                                           TrackSummaryTool       = InDetTrackSummaryTool,
                                                           FitterTool             = InDetVxFitterTool,
                                                           BeamConstraint         = 0)
    if InDetFlags.useBeamConstraint():
      InDetPriVxFinderTool_Si.BeamConstraint = 1

ToolSvc += InDetPriVxFinderTool_Si
if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinderTool_Si






from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
InDetPriVxFinder_Si = InDet__InDetPriVxFinder(name                        = "InDetPriVxFinder_Si",
                                               VertexFinderTool            = InDetPriVxFinderTool_Si,
                                               TracksName                  = InDetKeys.ResolvedTracks(),
                                               VxCandidatesOutputName      = InDetKeys.PrimaryVertices()+'_Si',
                                               VertexCollectionSortingTool = VertexCollectionSortingTool,
                                               doVertexSorting             = doSorting,
                                               OutputLevel = DEBUG)

topSequence += InDetPriVxFinder_Si
if InDetFlags.doPrintConfigurables():
    print InDetPriVxFinder_Si


## if InDetFlags.doVtxNtuple():
##     from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
##     InDetVertexNTupleWriter_Pix = Trk__PUVertexTest(name = "InDetVertexNTupleWriter_Pix",
##                                                     NtupleFileName       = 'TRKVAL',
##                                                     NtupleDirectoryName  = 'Validation',
##                                                     NtupleTreeName       = 'PixVertexTree',
##                                                     VertexCollectionName = InDetKeys.PrimaryVertices()+'_Pix',
##                                                     MonteCarloCollection = InDetKeys.McEventCollection(),
##                                                     PurityTool           = InDetPriVxPurityTool,
##                                                     TrackSelector        = InDetTrackSelectorTool_Pix,
##                                                     TracksName                  = "ResolvedPixelTracks",
##                                                     McAvailable          = InDetFlags.doTruth() )
##     topSequence += InDetVertexNTupleWriter_Pix
##     if (InDetFlags.doPrintConfigurables()):
##         print InDetVertexNTupleWriter_Pix
        
##     from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
##     InDetVertexNTupleWriter_SCT = Trk__PUVertexTest(name = "InDetVertexNTupleWriter_SCT",
##                                                     NtupleFileName       = 'TRKVAL',
##                                                     NtupleDirectoryName  = 'Validation',
##                                                     NtupleTreeName       = 'SCTVertexTree',
##                                                     VertexCollectionName = InDetKeys.PrimaryVertices()+'_SCT',
##                                                     MonteCarloCollection = InDetKeys.McEventCollection(),
##                                                     PurityTool           = InDetPriVxPurityTool,
##                                                     TrackSelector        = InDetTrackSelectorTool_SCT,
##                                                     TracksName                  = "ResolvedSCTTracks",
##                                                     McAvailable          = InDetFlags.doTruth() )
##     topSequence += InDetVertexNTupleWriter_SCT
##     if (InDetFlags.doPrintConfigurables()):
##         print InDetVertexNTupleWriter_SCT

        
##     from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
##     InDetVertexNTupleWriter_TRT = Trk__PUVertexTest(name = "InDetVertexNTupleWriter_TRT",
##                                                     NtupleFileName       = 'TRKVAL',
##                                                     NtupleDirectoryName  = 'Validation',
##                                                     NtupleTreeName       = 'TRTVertexTree',
##                                                     VertexCollectionName = InDetKeys.PrimaryVertices()+'_TRT',
##                                                     MonteCarloCollection = InDetKeys.McEventCollection(),
##                                                     PurityTool           = InDetPriVxPurityTool,
##                                                     TrackSelector        = InDetTrackSelectorTool_TRT,
##                                                     TracksName                  = "StandaloneTRTTracks",
##                                                     McAvailable          = InDetFlags.doTruth() )
##     topSequence += InDetVertexNTupleWriter_TRT
##     if (InDetFlags.doPrintConfigurables()):
##         print InDetVertexNTupleWriter_TRT
        
        
##     from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
##     InDetVertexNTupleWriter_Si = Trk__PUVertexTest(name = "InDetVertexNTupleWriter_Si",
##                                                     NtupleFileName       = 'TRKVAL',
##                                                     NtupleDirectoryName  = 'Validation',
##                                                     NtupleTreeName       = 'SiVertexTree',
##                                                     VertexCollectionName = InDetKeys.PrimaryVertices()+'_Si',
##                                                     MonteCarloCollection = InDetKeys.McEventCollection(),
##                                                     PurityTool           = InDetPriVxPurityTool,
##                                                     TrackSelector        = InDetTrackSelectorTool_Si,
##                                                    TracksName                  =InDetKeys.ResolvedTracks() ,
##                                                     McAvailable          = InDetFlags.doTruth() )
##     topSequence += InDetVertexNTupleWriter_Si
##     if (InDetFlags.doPrintConfigurables()):
##         print InDetVertexNTupleWriter_Si


if InDetFlags.doVtxD3PD() or InDetFlags.doVtxNtuple():
    print InDetKeys.McEventCollection()
    finderlist = { 'Pix': (InDetKeys.PrimaryVertices()+'_Pix',"ResolvedPixelTracks") , 'SCT':(InDetKeys.PrimaryVertices()+'_SCT','ResolvedSCTTracks')  , 'TRT':(InDetKeys.PrimaryVertices()+'_TRT','StandaloneTRTTracks'),  'Si':(InDetKeys.PrimaryVertices()+'_Si',InDetKeys.ResolvedTracks())   }
    for k,(vx,trk) in finderlist.items():
        print k,vx,trk
        from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
        topSequence += Trk__PUVertexTest(name = "InDetVertexNTupleWriter"+k,
                                         NtupleFileName           = 'TRKVAL',
                                         NtupleDirectoryName      = 'Validation'+k,   # move the +k to the line below when fixed
                                         NtupleTreeName           = 'VertexTree',
                                         VertexCollectionName = vx,
                                         TracksName           = trk,
                                         MonteCarloCollection = InDetKeys.McEventCollection(),
                                         PurityTool           = InDetPriVxPurityTool,
                                         TrackSelector        = InDetTrackSelectorTool,
                                         useBeamConstraint    = InDetFlags.useBeamConstraint(),
                                         McAvailable          = InDetFlags.doTruth()
                                         )

if InDetFlags.doVtxD3PD():
    #
    #TrackParticleD3PDObject1 = D3PDObject (makeTrackParticleD3PDObject, 'trk1_')
    #
    #TrackParticleD3PDObject2 = D3PDObject (makeTrackParticleD3PDObject, 'trk2_')
    # 
    #b1 = BuildVertexD3PDObject(_prefix='vx1_',
    #                           _label='vx1',
    #                           _sgkey='VxPrimaryCandidate',
    #                           trackTarget='trk1',
    #                           trackPrefix='trk1_',
    #                           trackType='Rec::TrackParticleContainer')
    #b2 = BuildVertexD3PDObject(_prefix='vx2_',
    #                           _label='vx2',
    #                           _sgkey='VxPrimaryCandidate_Si',
    #                           trackTarget='trk2',
    #                           trackPrefix='trk2_',
    #                           trackType='TrackCollection')
    #InDetTrackD3PDMaker += TrackParticleD3PDObject1
    #InDetTrackD3PDMaker += TrackParticleD3PDObject2
    #InDetTrackD3PDMaker += b1
    #InDetTrackD3PDMaker += b2
    import D3PDMakerCoreComps
    import EventCommonD3PDMaker
    from D3PDMakerCoreComps.D3PDObject import D3PDObject
    from TrackD3PDMaker.TrackD3PDObject import ResolvedTracksD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject

    alg = D3PDMakerCoreComps.MakerAlg('MinBiasTree',
                                      topSequence,
                                      "TestName.root")
    

    #InDetTrackD3PDMaker += TrackParticleD3PDObject(8)
    alg += ResolvedTracksD3PDObject(8)
    from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
    #InDetTrackD3PDMaker += PrimaryVertexD3PDObject(8)
    from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
    ResolvedTracksD3PDObject = TrackD3PDObject(_label='myResolvedTrack',
                                               _prefix='myResolvedTrack_',
                                               _sgkey='ResolvedPixelTracks',
                                               typeName='TrackCollection', #TrackCollection
                                               truthTarget='mc',
                                               truthPrefix='mc_',
                                               detailedTruthPrefix='detailed_mc_',
                                               trkTrackSGKey='ResolvedPixelTracks',
                                               truthMapKey='ResolvedTracksTruthCollection',
                                               detailedTruthMapKey='ResolvedTracksDetailedTruth')
    alg += ResolvedTracksD3PDObject(8)
    
    #PrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vx',
    #                                                  _label='vx_',
    #                                                  _sgkey='VxPrimaryCandidate',
    #                                                  trackTarget='trk',
    #                                                  trackPrefix='trk_',
    #                                                  trackType='Rec::TrackParticleContainer')
    #alg += PrimaryVertexD3PDObject(8)
            
    PrimaryVertexD3PDObjectSi = BuildVertexD3PDObject(_prefix='vx2',
                                                      _label='vx2_',
                                                      _sgkey='VxPrimaryCandidate_Pix',
                                                      trackTarget='myResolvedTrack',
                                                      trackPrefix='myResolvedTrack_',
                                                      trackType='TrackCollection')
    print PrimaryVertexD3PDObjectSi
    
    alg += PrimaryVertexD3PDObjectSi(8)
    #include("myD3PDObject.py")
    alg += PrimaryVertexD3PDObject(8)

if InDetFlags.doVtxD3PD():
    #alg += TrackParticleD3PDObject(levelOfDetail)
    #alg += PrimaryVertexD3PDObject(10)
    #alg += PrimaryVertexD3PDObject(levelOfDetail)
    pass

print "JW" 
print InDetTrackSelectorTool_Si
print InDetPriVxFinderTool_Si
print InDetPriVxFinder_Si
Stream.AddItem([ 'VxContainer#VxPrimaryCandidate_Si' ])
