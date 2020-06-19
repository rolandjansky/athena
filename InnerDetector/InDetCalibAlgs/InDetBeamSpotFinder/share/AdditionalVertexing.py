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

doD3PD = True
primaryVertices = []  # holder for the names of the PV collections
pvFinders       = []  # holder for references to the PV finder algs




def GeneralVertexer(system='Combined', setup=None,tracksName=None, suffix=""):
    """ attempt to make a simple tool for any type of combination of vertexing """
    ToolSvc = Service('ToolSvc')
    hasPix = True if system in ['Combined', 'Pix', 'Resolved']  else False
    hasSCT = True if system in ['Combined', 'SCT', 'Resolved']  else False
    hasTRT = True if system in ['Combined', 'TRT']              else False


    nHitBLayer          = InDetPrimaryVertexingCuts.nHitBLayer() if hasPix  else 0
    nHitPix             = InDetPrimaryVertexingCuts.nHitPix()    if hasPix  else 0
    nHitBLayerPlusPix   = InDetPrimaryVertexingCuts.nHitBLayerPlusPix() if hasPix else 0
    nHitSct             = InDetPrimaryVertexingCuts.nHitSct()    if hasSCT else 0
    nHitSi              = InDetPrimaryVertexingCuts.nHitSi()     if hasSCT else 0
    nHitTrt             = InDetPrimaryVertexingCuts.nHitTrt()    if hasTRT else 0

    vxSetup = InDetFlags.primaryVertexSetup() if setup==None else setup

    print "JW: Setup is: ", system, hasPix, hasSCT, hasTRT, tracksName, setup, vxSetup


    #Extras

    try:
        InDetTrackZ0SortingTool = ToolSvc.InDetTrackZ0SortingTool
    except:
        from InDetMultipleVertexSeedFinderUtils.InDetMultipleVertexSeedFinderUtilsConf import InDet__InDetTrackZ0SortingTool
        InDetTrackZ0SortingTool =  InDet__InDetTrackZ0SortingTool(name = "InDetTrackZ0SortingTool")
        ToolSvc += InDetTrackZ0SortingTool
        if (InDetFlags.doPrintConfigurables()):
            print InDetTrackZ0SortingTool

    try:
        InDetSlidingWindowMultiSeedFinder = ToolSvc.InDetSlidingWindowMultiSeedFinder
    except:
        from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
        InDetSlidingWindowMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name = "InDetSlidingWindowMultiSeedFinder",
                                                                                clusterLength = 5.*mm,
                                                                                TrackSelector = InDetTrackSelectorTool,
                                                                                Extrapolator  = InDetExtrapolator,
                                                                                SortingTool   = InDetTrackZ0SortingTool,
                                                                                #UseMaxInCluster = True
                                                                                )
        ToolSvc += InDetSlidingWindowMultiSeedFinder


    #Load additional fitters:
    try: 
        InDetVKalVrtFitter = ToolSvc.InDetVKalVrtFitter
        print " JW Found InDetVKalVrtFitter"
    except:
        print " JW no InDetVKalVrtFitter"
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        InDetVKalVrtFitter = Trk__TrkVKalVrtFitter(name = "InDetVKalVrtFitter")
        ToolSvc += InDetVKalVrtFitter
        if (InDetFlags.doPrintConfigurables()):
            print      InDetVKalVrtFitter        


    try:
        InDetVxFitterToolFastFinder = ToolSvc.InDetVxFitterToolFastFinder
        print " JW: found InDetVxFitterToolFastFinder"
    except:
        print "JW no InDetVxFitterToolFastFinder"
        from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
        InDetVxFitterToolFastFinder = Trk__FastVertexFitter(name                   = "InDetVxFitterToolFastFinder",
                                                            LinearizedTrackFactory = InDetLinFactory,
                                                            Extrapolator           = InDetExtrapolator)
        ToolSvc += InDetVxFitterToolFastFinder
        if (InDetFlags.doPrintConfigurables()):
            print      InDetVxFitterToolFastFinder

    try:
        InDetVxFitterToolFullFinder = ToolSvc.InDetVxFitterToolFullFinder
        print "JW found InDetVxFitterToolFullFinder"
    except:
        print "JW no InDetVxFitterToolFullFinder"
        from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FullVertexFitter
        InDetVxFitterToolFullFinder = Trk__FullVertexFitter(name                   = "InDetVxFitterToolFullFinder",
                                                            LinearizedTrackFactory = InDetLinFactory,
                                                            Extrapolator           = InDetExtrapolator)
        ToolSvc += InDetVxFitterToolFullFinder
        if (InDetFlags.doPrintConfigurables()):
            print      InDetVxFitterToolFullFinder
        
    try:
        print "JW Found InDetVxFitterToolSequential"
    except:
        print "JW no InDetVxFitterToolSequential"
        from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
        InDetVertexSmoother = Trk__SequentialVertexSmoother(name = "InDetVertexSmoother")
        ToolSvc += InDetVertexSmoother
        if (InDetFlags.doPrintConfigurables()):
            print      InDetVertexSmoother
               
        from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexFitter
        InDetVxFitterToolSequential = Trk__SequentialVertexFitter(name                   = "InDetSequentialVxFitterTool",
                                                                  LinearizedTrackFactory = InDetLinFactory,
                                                                  VertexSmoother  = InDetVertexSmoother
                                                                  #VertexUpdator   = # no setting required 
                                                                  )
        ToolSvc += InDetVxFitterToolSequential
        if (InDetFlags.doPrintConfigurables()):
            print      InDetVxFitterToolSequential
    

    
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    InDetTrackSelectorTool_vx = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool_"+system+"_"+vxSetup+suffix,
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
                                                                       nHitBLayer          =    nHitBLayer,
                                                                       nHitPix             =    nHitPix,
                                                                       nHitBLayerPlusPix   =    nHitBLayerPlusPix,
                                                                       nHitSct             =    nHitSct,
                                                                       nHitSi              =    nHitSi,
                                                                       nHitTrt             =    nHitTrt,
                                                                       nHitTrtHighEFractionMax     =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                                       nHitTrtHighEFractionWithOutliersMax =    InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                                       useSharedHitInfo            =    InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                                       useTrackQualityInfo         =    InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                                       fitChi2OnNdfMax         =    InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                                       TrtMaxEtaAcceptance         =    InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                                       TrackSummaryTool            =    InDetTrackSummaryTool,
                                                                       Extrapolator            =    InDetExtrapolator)


    ToolSvc += InDetTrackSelectorTool_vx
    if (InDetFlags.doPrintConfigurables()):
        print InDetTrackSelectorTool_vx


    # -----------------------------------------
    #
    # ----- load primary vertex finder tool
    #
    # -----------------------------------------
    
    if (not (vxSetup == 'AdaptiveFinding') and
        not (vxSetup == 'AdaptiveMultiFinding')  and
        not (vxSetup == 'DefaultVKalVrtFinding') and 
        not (vxSetup == 'InDetPriVxFinderFullFinder') and
        not (vxSetup == 'InDetPriVxFinderFastFinder') 
        ):
        #
        # --- load primary vertex finder tool
        #
        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
        InDetPriVxFinderTool_vx = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool_"+system+"_"+vxSetup+suffix,
                                                               PriVxSeedFinder   = InDetMultiSeedFinder,
                                                               TrackSelector     = InDetTrackSelectorTool_vx,
                                                               VertexFitterTool  = InDetVxFitterTool,
                                                               chi2CutMethod     = 2,
                                                               useBeamConstraint = InDetFlags.useBeamConstraint())
        if jobproperties.Beam.numberOfCollisions() > 2.0: # 900 GeV running at "zero lumi" has 2.0 for this variable
            InDetPriVxFinderTool_vx.enableMultipleVertices = 1;
        else:
            InDetPriVxFinderTool_vx.enableMultipleVertices = 0;

        if rec.Commissioning():
            InDetPriVxFinderTool_vx.maxChi2PerTrack = 15.
    
    elif vxSetup == 'AdaptiveFinding':
        #
        # --- load adaptive primary vertex finder
        #
        
        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
        InDetPriVxFinderTool_vx = InDet__InDetIterativePriVxFinderTool(name             = "InDetIterativePriVxFinderTool_"+system+"_"+vxSetup+suffix,
                                                                        VertexFitterTool = InDetVxFitterTool,
                                                                        TrackSelector    = InDetTrackSelectorTool_vx,
                                                                        SeedFinder = InDetVtxSeedFinder,
                                                                        ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                                                        LinearizedTrackFactory = InDetLinFactory,
                                                                        useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                        significanceCutSeeding = 12,
                                                                        maximumChi2cutForSeeding = 49,
                                                                        maxVertices = 25
                                                                        )
                                  
        
    elif vxSetup == 'AdaptiveMultiFinding':
        #
        # --- load adaptive multi primary vertex finder
        #
        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
        InDetPriVxFinderTool_vx = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool_"+system+"_"+vxSetup+suffix,
                                                                            SeedFinder        = InDetVtxSeedFinder,
                                                                            VertexFitterTool  = InDetVxFitterTool,
                                                                            TrackSelector     = InDetTrackSelectorTool_vx,
                                                                            useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                            selectiontype     = 0,
                                                                            do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
  

    #Add additional finder tools
    elif vxSetup == 'InDetPriVxFinderFastFinder':# not the official name
        
        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
        InDetPriVxFinderTool_vx = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderToolFastFinder_"+system+"_"+vxSetup+suffix,
                                                              VertexFitterTool  = InDetVxFitterToolFastFinder,
                                                              PriVxSeedFinder   = InDetSlidingWindowMultiSeedFinder,
                                                              TrackSelector     = InDetTrackSelectorTool_vx,
                                                              useBeamConstraint = InDetFlags.useBeamConstraint())
        if jobproperties.Beam.zeroLuminosity():
            InDetPriVxFinderTool_vx.enableMultipleVertices = 0
        else:
            InDetPriVxFinderTool_vx.enableMultipleVertices = 1
            
    elif vxSetup == 'InDetPriVxFinderFullFinder':
        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
        InDetPriVxFinderTool_vx = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderToolFullFinder_"+system+"_"+vxSetup+suffix,
                                                              VertexFitterTool  = InDetVxFitterToolFullFinder,
                                                              PriVxSeedFinder   = InDetSlidingWindowMultiSeedFinder,
                                                              TrackSelector     = InDetTrackSelectorTool_vx,
                                                              useBeamConstraint = InDetFlags.useBeamConstraint())
        if jobproperties.Beam.zeroLuminosity():
            InDetPriVxFinderTool_vx.enableMultipleVertices = 0
        else:
            InDetPriVxFinderTool_vx.enableMultipleVertices = 1



    ToolSvc += InDetPriVxFinderTool_vx
    if (InDetFlags.doPrintConfigurables()):
        print InDetPriVxFinderTool_vx


    

    from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
    InDetPriVxFinder_vx = InDet__InDetPriVxFinder(name                        = "InDetPriVxFinder_"+system+"_"+vxSetup,
                                                   VertexFinderTool            = InDetPriVxFinderTool_vx,
                                                   TracksName                  = tracksName,
                                                   VxCandidatesOutputName      = InDetKeys.PrimaryVertices()+"_"+system+"_"+vxSetup+suffix,
                                                   VertexCollectionSortingTool = VertexCollectionSortingTool,
                                                   doVertexSorting             = doSorting,
                                                   OutputLevel = DEBUG)
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += InDetPriVxFinder_vx
    pvFinders.append( InDetPriVxFinder_vx )
    primaryVertices.append( InDetPriVxFinder_vx.VxCandidatesOutputName)
    if InDetFlags.doPrintConfigurables():
        print InDetPriVxFinder_vx

    Stream.AddItem([ 'VxContainer#'+InDetKeys.PrimaryVertices()+"_"+system+"_"+vxSetup+suffix ])


    return (InDetTrackSelectorTool_vx,InDetPriVxFinderTool_vx,InDetPriVxFinder_vx )




vx1 = GeneralVertexer(system='Pix', setup=None,      tracksName="ResolvedPixelTracks")
vx2 = GeneralVertexer(system='SCT', setup=None,      tracksName="ResolvedSCTTracks")
vx3 = GeneralVertexer(system='TRT', setup=None,      tracksName="StandaloneTRTTracks")
vx4 = GeneralVertexer(system='Combined', setup=None, tracksName="CombinedInDetTracks")
vx5 = GeneralVertexer(system='Resolved', setup=None, tracksName=InDetKeys.ResolvedTracks())
vxFull = GeneralVertexer(system='Combined', setup="InDetPriVxFinderFullFinder",      tracksName="CombinedInDetTracks")
vxFast = GeneralVertexer(system='Combined', setup="InDetPriVxFinderFastFinder",      tracksName="CombinedInDetTracks")
vxFullPix = GeneralVertexer(system='Pix', setup="InDetPriVxFinderFullFinder",      tracksName="ResolvedPixelTracks")
vxFastPix = GeneralVertexer(system='Pix', setup="InDetPriVxFinderFastFinder",      tracksName="ResolvedPixelTracks")
vxFullSCT = GeneralVertexer(system='SCT', setup="InDetPriVxFinderFullFinder",      tracksName="ResolvedSCTTracks")
vxFastSCT = GeneralVertexer(system='SCT', setup="InDetPriVxFinderFastFinder",      tracksName="ResolvedSCTTracks")
vxFullRes = GeneralVertexer(system='Resolved', setup="InDetPriVxFinderFullFinder", tracksName=InDetKeys.ResolvedTracks())
vxFastRes = GeneralVertexer(system='Resolved', setup="InDetPriVxFinderFastFinder", tracksName=InDetKeys.ResolvedTracks())

## vx7 = GeneralVertexer(system='SCT', setup="DefaultVKalVrtFinding",      tracksName="ResolvedSCTTracks")
## vx8 = GeneralVertexer(system='TRT', setup="DefaultVKalVrtFinding",      tracksName="StandaloneTRTTracks")
## vx9 = GeneralVertexer(system='Combined', setup="DefaultVKalVrtFinding", tracksName="CombinedInDetTracks")
## vx10 = GeneralVertexer(system='Resolved', setup="DefaultVKalVrtFinding", tracksName=InDetKeys.ResolvedTracks())






if doD3PD:
    import D3PDMakerCoreComps
    import EventCommonD3PDMaker
    from D3PDMakerCoreComps.D3PDObject import D3PDObject
    from TrackD3PDMaker.TrackD3PDObject import ResolvedTracksD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
    ### Make the core
    alg = D3PDMakerCoreComps.MakerAlg('BeamSpotTree',
                                      topSequence,
                                      "VtxNtuple.root")
    alg.OutputLevel=ERROR
    # ## Add the event headers
    from EventCommonD3PDMaker.EventInfoD3PDObject import makeEventInfoD3PDObject
    EventInfoD3PDObject = D3PDObject (makeEventInfoD3PDObject, 'ei_')
    EventInfoD3PDObject.defineBlock (0, 'eventID', EventCommonD3PDMaker.EventIDFillerTool)
    alg += EventInfoD3PDObject(10)
    
    
    from MinBiasD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject
    #alg += BeamSpotD3PDObject(10)

    from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
    from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
    if InDetFlags.doTruth():
        alg += TruthTrackD3PDObject(10)
        alg += TruthVertexD3PDObject(10)
        pass



    #index = 8 if not InDetFlags.doTruth() else 10
    index = 8
     
    from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import ResolvedTracksD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject

    from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
    
    alg += TrackParticleD3PDObject(1)
    alg += PrimaryVertexD3PDObject(index)

    index = 3
    #alg += PixelTrackD3PDObject(index)
    #alg += SCTTrackD3PDObject(index)
    #alg += TRTTrackD3PDObject(index)
    #alg += ResolvedTracksD3PDObject(index)

    index = 8
    from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
    PrimaryVertexD3PDObjectPix = BuildVertexD3PDObject(_prefix='vxPix_',
                                                       _label='vxPix',
                                                       _sgkey='VxPrimaryCandidate_Pix_AdaptiveFinding',
                                                       trackTarget='trkPix',
                                                       trackPrefix='trkPix_',
                                                       trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectPix(index)
    PrimaryVertexD3PDObjectSCT = BuildVertexD3PDObject(_prefix='vxSCT_',
                                                       _label='vxSCT',
                                                       _sgkey='VxPrimaryCandidate_SCT_AdaptiveFinding',
                                                       trackTarget='trkSCT',
                                                       trackPrefix='trkSCT_',
                                                       trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectSCT(index)

    PrimaryVertexD3PDObjectRes = BuildVertexD3PDObject(_prefix='vxSi_',
                                                       _label='vxSi',
                                                       _sgkey='VxPrimaryCandidate_Resolved_AdaptiveFinding',
                                                       trackTarget='trkResolvedTrack',
                                                       trackPrefix='trkResolvedTrack_',
                                                       trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectRes(index)


    PrimaryVertexD3PDObjectFull = BuildVertexD3PDObject(_prefix='vxFull_',
                                                       _label='vxFull',
                                                       _sgkey=vxFull[2].VxCandidatesOutputName,
                                                       trackTarget='trk',
                                                       trackPrefix='trk_',
                                                       trackType='Rec::TrackParticleContainer')
    alg += PrimaryVertexD3PDObjectFull(index)

    PrimaryVertexD3PDObjectFast = BuildVertexD3PDObject(_prefix='vxFast_',
                                                       _label='vxFast',
                                                       _sgkey=vxFast[2].VxCandidatesOutputName,
                                                       trackTarget='trk',
                                                       trackPrefix='trk_',
                                                       trackType='Rec::TrackParticleContainer')
    alg += PrimaryVertexD3PDObjectFast(index)

    PrimaryVertexD3PDObjectFullPix = BuildVertexD3PDObject(_prefix='vxFullPix_',
                                                           _label='vxFullPix',
                                                           _sgkey=vxFullPix[2].VxCandidatesOutputName,
                                                           trackTarget='trkPix',
                                                           trackPrefix='trkPix_',
                                                           trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectFullPix(index)
    
    PrimaryVertexD3PDObjectFastPix = BuildVertexD3PDObject(_prefix='vxFastPix_',
                                                           _label='vxFastPix',
                                                           _sgkey=vxFastPix[2].VxCandidatesOutputName,
                                                           trackTarget='trkPix',
                                                           trackPrefix='trkPix_',
                                                           trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectFastPix(index)
    PrimaryVertexD3PDObjectFullSCT = BuildVertexD3PDObject(_prefix='vxFullSCT_',
                                                           _label='vxFullSCT',
                                                           _sgkey=vxFullSCT[2].VxCandidatesOutputName,
                                                           trackTarget='trkSCT',
                                                           trackPrefix='trkSCT_',
                                                           trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectFullSCT(index)
    
    PrimaryVertexD3PDObjectFastSCT = BuildVertexD3PDObject(_prefix='vxFastSCT_',
                                                           _label='vxFastSCT',
                                                           _sgkey=vxFastSCT[2].VxCandidatesOutputName,
                                                           trackTarget='trkSCT',
                                                           trackPrefix='trkSCT_',
                                                           trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectFastSCT(index)

    PrimaryVertexD3PDObjectFullRes = BuildVertexD3PDObject(_prefix='vxFullSi_',
                                                           _label='vxFullSi',
                                                           _sgkey=vxFullRes[2].VxCandidatesOutputName,
                                                           trackTarget='trkResolvedTrack',
                                                           trackPrefix='trkResolvedTrack_',
                                                           trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectFullRes(index)
    
    PrimaryVertexD3PDObjectFastRes = BuildVertexD3PDObject(_prefix='vxFastSi_',
                                                           _label='vxFastSi',
                                                           _sgkey=vxFastRes[2].VxCandidatesOutputName,
                                                           trackTarget='trkResolvedTrack',
                                                           trackPrefix='trkResolvedTrack_',
                                                           trackType='TrackCollection')
    alg += PrimaryVertexD3PDObjectFastRes(index)

# def addVtxD3PD( finderAlg, alg):
    


## if InDetFlags.doVtxD3PD() or InDetFlags.doVtxNtuple():
##     print InDetKeys.McEventCollection()
##     finderlist = { 'Pix': (InDetKeys.PrimaryVertices()+'_Pix',"ResolvedPixelTracks") , 'SCT':(InDetKeys.PrimaryVertices()+'_SCT','ResolvedSCTTracks')  , 'TRT':(InDetKeys.PrimaryVertices()+'_TRT','StandaloneTRTTracks'),  'Si':(InDetKeys.PrimaryVertices()+'_Si',InDetKeys.ResolvedTracks())   }
##     for k,(vx,trk) in finderlist.items():
##         print k,vx,trk
##         from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
##         topSequence += Trk__PUVertexTest(name = "InDetVertexNTupleWriter"+k,
##                                          NtupleFileName           = 'TRKVAL',
##                                          NtupleDirectoryName      = 'Validation'+k,   # move the +k to the line below when fixed
##                                          NtupleTreeName           = 'VertexTree',
##                                          VertexCollectionName = vx,
##                                          TracksName           = trk,
##                                          MonteCarloCollection = InDetKeys.McEventCollection(),
##                                          PurityTool           = InDetPriVxPurityTool,
##                                          TrackSelector        = InDetTrackSelectorTool,
##                                          useBeamConstraint    = InDetFlags.useBeamConstraint(),
##                                          McAvailable          = InDetFlags.doTruth()
##                                          )
