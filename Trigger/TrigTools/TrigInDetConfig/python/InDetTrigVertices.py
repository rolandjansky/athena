#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__ =   "Mark Sutton and Lukas Novotny"
__doc__    =   "vertexFinder_builder"
__all__    = [ "vertexFinder_builder", "makeInDetTrigVertices" ]


#  These routines create the vertex finder for 
#  use in the trigger
#  
#  This is done in the 
# 
#     vertexFinder_builder() 
#
#  the rest are just helper 
#  functions to create the relevant tools that are 
#  needed along the way 


# old function for backwards compatability
#TODO inputTrackCollection is obsolete, remove in the next MR iteration
def makeInDetTrigVertices( whichSignature, inputTrackCollection, outputVtxCollection=None, config=None, adaptiveVertex=None ) :

    if config is None:
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        config = getInDetTrigConfig( whichSignature )

    if outputVtxCollection is None: 
        outputVtxCollection = config.vertex

    if adaptiveVertex is None:
        adaptiveVertex = config.adaptiveVertex 

    return vertexFinder_builder( signature         = whichSignature, 
                                 config            = config,
                                 inputTracks       = inputTrackCollection,
                                 outputVertices    = outputVtxCollection,
                                 adaptiveVertexing = adaptiveVertex )




# actual function to create the vertex finder instance
# needs the tool to actually create the vertices, plus the 
# tool to sort them into the desired order, and some monitoring
# here the vertex finder tool is chosen (iterative vs adaptive)
def vertexFinder_builder( signature, config, inputTracks, outputVertices, adaptiveVertexing ) :

    from AthenaCommon.Logging import logging
    log = logging.getLogger("InDetVtx")

    # create the three subtools for use by the vertexFinder itself ...
    
    # the actual tool which finds the vertices ...
    # and actual place which choose between iterative and adaptive vertex finder tools
    if adaptiveVertexing :
        vertexFinderTool = adaptiveMultiVertexFinderTool_builder( signature, config ) 
    else :   
        vertexFinderTool = iterativeVertexFinderTool_builder( signature, config ) 

    # which are then sorted ...
    vertexSortingTool = vertexSortingTool_builder( signature, config )

    # and finally some monitoring ...
    vertexMonitoringTool = vertexMonitoringTool_builder( signature, config )

    # no create the vertex finder ...
    from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder

    vertexFinder = InDet__InDetPriVxFinder( name                        = "InDetTrigPriVxFinder" + signature,
                                            VertexFinderTool            = vertexFinderTool,
                                            TracksName                  = inputTracks, 
                                            VxCandidatesOutputName      = outputVertices, 
                                            VertexCollectionSortingTool = vertexSortingTool,
                                            doVertexSorting             = True,
                                            PriVxMonTool                = vertexMonitoringTool )
    
    log.debug(vertexFinder)
    
    return  [ vertexFinder ]



# linearised track factory, whatever that does, for the vertex finder

def  linearTrackFactory_builder( signature, config, extrapolator ) :

    from AthenaCommon.AppMgr import ToolSvc

    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory

    linearTrackFactory = Trk__FullLinearizedTrackFactory( name         = "FullLinearizedTrackFactory" + signature,
                                                          Extrapolator = extrapolator )
    ToolSvc += linearTrackFactory

    return linearTrackFactory



# vertex fitter for the vertex finder 

def  iterativeVertexFitterTool_builder( signature, config, linearTrackFactory, extrapolator ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
    vertexFitterTool = Trk__FastVertexFitter( name                   = "InDetTrigFastVertexFitterTool" + signature,
                                              LinearizedTrackFactory = linearTrackFactory,
                                              Extrapolator           = extrapolator ) 
    ToolSvc += vertexFitterTool
    
    return vertexFitterTool




# impact parameter estimator

def  impactEstimator_builder( signature, config, extrapolator ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
    impactPoint3dEstimator = Trk__ImpactPoint3dEstimator( name         = "InDetTrigImpactPoint3dEstimator" + signature,
                                                          Extrapolator = extrapolator )
   
    ToolSvc += impactPoint3dEstimator

    return impactPoint3dEstimator





def  trackSelectorTool_builder( signature, config, trackSummaryTool, extrapolator, cuts ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    # now create the track selection tool itself ...
    
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
   

    # config the number of hits needed for this signature ...
 
    minNSiHits_vtx = cuts.nHitSi()
    
    if config.minNSiHits_vtx is not None:
        minNSiHits_vtx = config.minNSiHits_vtx

    trackSelectorTool = InDet__InDetTrackSelectionTool( name     = "InDetTrigDetailedTrackSelectionTool" + signature,
                                                        Extrapolator     = extrapolator, 
                                                        TrackSummaryTool = trackSummaryTool,
                                                        CutLevel      = cuts.TrackCutLevel(),
                                                        # maybe have all these cuts passed in just by passing in the configuredVtsCuts object 
                                                        minPt         = cuts.minPT(),
                                                        maxD0         = cuts.IPd0Max(),
                                                        maxZ0         = cuts.z0Max(),
                                                        maxZ0SinTheta = cuts.IPz0Max(),
                                                        maxSigmaD0    = cuts.sigIPd0Max(),
                                                        maxSigmaZ0SinTheta = cuts.sigIPz0Max(),
                                                        maxChiSqperNdf     = cuts.fitChi2OnNdfMax(), # Seems not to be implemented?
                                                        maxAbsEta          = cuts.etaMax(),
                                                        minNInnermostLayerHits = cuts.nHitInnermostLayer(),
                                                        minNPixelHits    = cuts.nHitPix(),
                                                        maxNPixelHoles   = cuts.nHolesPix(),
                                                        minNSctHits      = cuts.nHitSct(),
                                                        minNTrtHits      = cuts.nHitTrt(),
                                                        minNSiHits       = minNSiHits_vtx )

    ToolSvc += trackSelectorTool
   
    return trackSelectorTool




# the trackdensity seed finder builder ...

def  trackDensitySeedFinder_builder( signature, config ) :
    
    from AthenaCommon.AppMgr import ToolSvc

    # Gaussian Density finder needed by the seed finder ...
    
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__GaussianTrackDensity
    gaussianTrackDensity = Trk__GaussianTrackDensity( name = "TrigGaussianDensity" + signature )
   
    ToolSvc += gaussianTrackDensity
   
    # TrackDensitySeed Finder for the iterative vtx finder tool ...
    
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__TrackDensitySeedFinder
    trackDensitySeedFinder = Trk__TrackDensitySeedFinder( name             = "TrigGaussianDensitySeedFinder" + signature,
                                                          DensityEstimator = gaussianTrackDensity )
   
    ToolSvc += trackDensitySeedFinder

    return trackDensitySeedFinder
   




# create the actual vertex finder tool ...

def iterativeVertexFinderTool_builder( signature, config ) : 

    from AthenaCommon.AppMgr import ToolSvc

    # use the getters from TrackingCommon ... 
    import InDetRecExample.TrackingCommon as TrackingCommon
   
    # the track summary tool, and extrapolator will be needed by multiple 
    # tools so create them once and pass them into the builders ...  
    trackSummaryTool = TrackingCommon.getInDetTrackSummaryTool()
    extrapolator     = TrackingCommon.getInDetExtrapolator()

    # get the selection cuts use to select the actual tracks in the tool ...
    from InDetTrigRecExample.TrigInDetConfiguredVtxCuts import ConfiguredTrigVtxCuts 
    vtxcuts = ConfiguredTrigVtxCuts() 
    vtxcuts.printInfo()

    
    # now create the five sub tools needed ...
    linearTrackFactory     =        linearTrackFactory_builder( signature, config, extrapolator )
    vertexFitterTool       = iterativeVertexFitterTool_builder( signature, config, linearTrackFactory, extrapolator )
    impactEstimator        =           impactEstimator_builder( signature, config, extrapolator )
    trackSelectorTool      =         trackSelectorTool_builder( signature, config, trackSummaryTool, extrapolator, vtxcuts )
    trackDensitySeedFinder =    trackDensitySeedFinder_builder( signature, config )
    
    # now create the actual vertex finder tool ...
    # this is the main part of the actual working part of the code - 
    # the vertoces are found by this class, in this instance it includes
    # a beam line constraint - it we want this to allow this constrain 
    # to be disabled we can add a flag and some additional logic 

    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    
    vertexFinderTool = InDet__InDetIterativePriVxFinderTool( name                     = "InDetTrigPriVxFinderTool" + signature,
                                                             VertexFitterTool         = vertexFitterTool,
                                                             TrackSelector            = trackSelectorTool,
                                                             SeedFinder               = trackDensitySeedFinder,
                                                             ImpactPoint3dEstimator   = impactEstimator,
                                                             LinearizedTrackFactory   = linearTrackFactory,
                                                             useBeamConstraint        = True,
                                                             significanceCutSeeding   = 12,
                                                             maximumChi2cutForSeeding = 49,
                                                             maxVertices              = 200,
                                                             createSplitVertices      = False,
                                                             doMaxTracksCut           = vtxcuts.doMaxTracksCut(),
                                                             MaxTracks                = vtxcuts.MaxTracks() )
                                                           
    
    ToolSvc += vertexFinderTool
   
    return vertexFinderTool




# create the vertex sorting tool - this is to sort the vertex candidates into 
# some order so clients can pick the best one off the front and so on ...

def vertexSortingTool_builder( signature, config ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    # create the vertex weight calculator, to be passed into the sorting tool ... 

    from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__SumPtVertexWeightCalculator
    vertexWeightCalculator = Trk__SumPtVertexWeightCalculator( name  = "InDetSumPtVertexWeightCalculator" + signature,
                                                               DoSumPt2Selection = True)
    ToolSvc += vertexWeightCalculator
    
    # and now create the sorting tool ...

    from TrkVertexTools.TrkVertexToolsConf import Trk__VertexCollectionSortingTool
    vertexSortingTool = Trk__VertexCollectionSortingTool( name = "InDetVertexCollectionSortingTool" + signature,
                                                            VertexWeightCalculator = vertexWeightCalculator)
    ToolSvc += vertexSortingTool
    
    return vertexSortingTool
    
# create online vertex monitoring histograms
def vertexMonitoringTool_builder( signature, config ) : 
    from InDetPriVxFinder.InDetPriVxFinderMonitoring import InDetPriVxFinderMonitoringTool
    return  InDetPriVxFinderMonitoringTool()





#------------------------------------------------------------------------------------------------------------------
#                         BUILDERS FOR ADAPTIVE MULTI VERTEX TOOL
#------------------------------------------------------------------------------------------------------------------

# create the actual vertex finder tool ...
def adaptiveMultiVertexFinderTool_builder( signature, config ) : 

    from AthenaCommon.Logging import logging
    log = logging.getLogger("InDetVtx")

    from AthenaCommon.AppMgr import ToolSvc

    # use the getters from TrackingCommon ... 
    import InDetRecExample.TrackingCommon as TrackingCommon
   
    # the track summary tool, and extrapolator will be needed by multiple 
    # tools so create them once and pass them into the builders ...  
    trackSummaryTool = TrackingCommon.getInDetTrackSummaryTool()
    extrapolator     = TrackingCommon.getInDetExtrapolator()
    doVtx3DFinding   = True # TODO!!!! InDetFlags.doPrimaryVertex3DFinding()

    # get the selection cuts use to select the actual tracks in the tool ...
    from InDetTrigRecExample.TrigInDetConfiguredVtxCuts import ConfiguredTrigVtxCuts 
    vtxcuts = ConfiguredTrigVtxCuts() 
    vtxcuts.printInfo()

    # now create the five sub tools needed ...
    linearTrackFactory     =            linearTrackFactory_builder( signature, config, extrapolator )
    impactEstimator        =               impactEstimator_builder( signature, config, extrapolator )
    vertexFitterTool       =      adaptiveVertexFitterTool_builder( signature, config, linearTrackFactory, extrapolator,impactEstimator )
    trackSelectorTool      =             trackSelectorTool_builder( signature, config, trackSummaryTool, extrapolator, vtxcuts )
    seedFinder             =        trackDensitySeedFinder_builder( signature, config )
    # leave this here, but commented for the time being while we investigate ...
    # seedFinder           = adaptiveMultiVertexSeedFinder_builder( signature, doVtx3DFinding)


    # now create the actual vertex finder tool ...
    # this is the main part of the actual working part of the code - 
    # the vertoces are found by this class, in this instance it includes
    # a beam line constraint - it we want this to allow this constrain 
    # to be disabled we can add a flag and some additional logic 
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool

    singleTrackVertices = config.addSingleTrackVertices 
    tracksMaxZinterval  = config.TracksMaxZinterval 
    

    ##################################################################################    

    acts = config.actsVertex

    if acts is False:

        log.info( "ID Trigger: using standard vertex" ) 

        vertexFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetTrigAdaptiveMultiPriVxFinderTool" + signature,
                                                                    SeedFinder        = seedFinder,
                                                                    VertexFitterTool  = vertexFitterTool,
                                                                    TrackSelector     = trackSelectorTool,
                                                                    useBeamConstraint = True,
                                                                    m_useSeedConstraint = False,  # default parameter is True sop use ACTS default value
                                                                    TracksMaxZinterval  = tracksMaxZinterval,
                                                                    addSingleTrackVertices = singleTrackVertices,
                                                                    selectiontype     = 0, # what is this? - ACTS equivalent ? 
                                                                    do3dSplitting     = doVtx3DFinding )
    else:
        
        log.info( "ID Trigger: using Acts vertex" ) 
        
        # Don't actually need to check whether the ActsGeometry has been configured, because it is 
        # included at the top level once the module is loaded, but we add the explicit check and the 
        # possibility to create it for  the impossible case that it is not there to avoid getting the python
        # warning that we have included a function that is not then used.

        # it would perhaps be better is the acts geometry were instead configured in some function that could
        # be called by any tool that actually needed it to be configured, then we would avoid this python error 
        # without unecessary python code 

        from ActsGeometry.ActsTrackingGeometryTool import ConfiguredActsTrackingGeometry
        
        if not hasattr(ToolSvc, "ActsTrackingGeometryTool"):
            log.info( "ID Trigger: creating new acts geometry" )
            actsTrackingGeometryTool = ConfiguredActsTrackingGeometry(name = "ActsTrackingGeometryTool")            
            ToolSvc += actsTrackingGeometryTool
   
        # add it to the ServiceManager
        actsTrackingGeometryTool = getattr(ToolSvc,"ActsTrackingGeometryTool")

        import AthenaCommon.CfgMgr as CfgMgr
    
        actsExtrapolationTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
        actsExtrapolationTool.TrackingGeometryTool = actsTrackingGeometryTool
        
        from ActsPriVtxFinder.ActsPriVtxFinderConf import ActsAdaptiveMultiPriVtxFinderTool

        vertexFinderTool = ActsAdaptiveMultiPriVtxFinderTool(name  = "ActsAdaptiveMultiPriVtxFinderTool" + signature,
                                                             TrackSelector      = trackSelectorTool,
                                                             useBeamConstraint  = True,
                                                             useSeedConstraint  = False, # use explicit ACTS default parameter
                                                             tracksMaxZinterval = tracksMaxZinterval,
                                                             do3dSplitting      = doVtx3DFinding, 
                                                             addSingleTrackVertices = singleTrackVertices,
                                                             TrackingGeometryTool = actsTrackingGeometryTool,
                                                             ExtrapolationTool  = actsExtrapolationTool )
                
    ToolSvc += vertexFinderTool
   
    return vertexFinderTool



# annealing for adaptive vertex finder
def annealingMaker_builder(signature, config ) :

    from AthenaCommon.AppMgr import ToolSvc

    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
    annealingMaker = Trk__DetAnnealingMaker(name = "InDetTrigAnnealingMaker" + signature,
                                                     SetOfTemperatures = [1.]) 
    ToolSvc += annealingMaker
    
    return annealingMaker



# vertex fitter for the vertex finder 
# this one is for adaptive vertex finder tool
def  adaptiveVertexFitterTool_builder( signature, config, linearTrackFactory, extrapolator,impactEstimator ) :

    from AthenaCommon.AppMgr import ToolSvc

    annealingMaker  = annealingMaker_builder ( signature, config )
    from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveMultiVertexFitter
    vertexFitterTool = Trk__AdaptiveMultiVertexFitter(name                         = "InDetTrigAdaptiveMultiVertexFitterTool" + signature,
                                                      LinearizedTrackFactory       = linearTrackFactory,
                                                      ImpactPoint3dEstimator       = impactEstimator,
                                                      AnnealingMaker               = annealingMaker,
                                                      DoSmoothing                  = True) # false is default 
 
    ToolSvc += vertexFitterTool
    
    return vertexFitterTool




def adaptiveMultiVertexSeedFinder_builder( signature, config, doVtx3DFinding ):

    from AthenaCommon.AppMgr import ToolSvc
    
    if (doVtx3DFinding): #InDetFlags.doPrimaryVertex3DFinding()

        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
        seedFinder = Trk__CrossDistancesSeedFinder( name              = "InDetTrigCrossDistancesSeedFinder" + signature,
                                                    trackdistcutoff   = 1.,
                                                    trackdistexppower = 2)
    
    else:
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import getInDetTrigTrackToVertexIPEstimator
        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
        seedFinder = Trk__ZScanSeedFinder(name = "InDetTrigZScanSeedFinder" + signature,
                                          IPEstimator = getInDetTrigTrackToVertexIPEstimator() )
                                          # Mode1dFinder = # default, no setting needed
        
        
    ToolSvc += seedFinder

    return seedFinder




