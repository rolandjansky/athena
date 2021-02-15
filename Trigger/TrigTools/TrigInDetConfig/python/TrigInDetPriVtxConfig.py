#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__author__ =   "Mark Sutton and Lukas Novotny"
__doc__    =   "vertexFinder_builder"
__all__    = [ "vertexFinder_builder", "makeVertices" ]


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
def makeVertices( whichSignature, inputTrackCollection, outputVtxCollection, config, adaptiveVertexing=False ) :

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    adaptiveVertexing = getInDetTrigConfig( whichSignature ).adaptiveVertex 

    return vertexFinder_builder( signature         = whichSignature, 
                                 inputTracks       = config.FT.tracksFTF(),
                                 outputVertices    = outputVtxCollection,
                                 adaptiveVertexing = adaptiveVertexing )




# actual function to create the vertex finder instance
# needs the tool to actually create the vertices, plus the 
# tool to sort them into the desired order, and some monitoring
# here the vertex finder tool is chosen (iterative vs adaptive)
def vertexFinder_builder( signature, inputTracks, outputVertices, adaptiveVertexing ) :

    from AthenaCommon.Logging import logging
    log = logging.getLogger("InDetVtx")

    from TrigEDMConfig.TriggerEDMRun3 import recordable

    # create the three subtools for use by the vertexFinder itself ...
    
    # the actual tool which finds the vertices ...
    # and actual place which choose between iterative and adaptive vertex finder tools
    if adaptiveVertexing :
        vertexFinderTool = adaptiveMultiVertexFinderTool_builder( signature ) 
    else :   
        vertexFinderTool = iterativeVertexFinderTool_builder( signature ) 

    # which are then sorted ...
    vertexSortingTool = vertexSortingTool_builder( signature )

    # and finally some monitoring ...
    vertexMonitoringTool = vertexMonitoringTool_builder( signature )

    # no create the vertex finder ...
    from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder

    vertexFinder = InDet__InDetPriVxFinder( name                        = "InDetTrigPriVxFinder" + signature,
                                            VertexFinderTool            = vertexFinderTool,
                                            TracksName                  = recordable(inputTracks), 
                                            VxCandidatesOutputName      = recordable(outputVertices), 
                                            VertexCollectionSortingTool = vertexSortingTool,
                                            doVertexSorting             = True,
                                            PriVxMonTool                = vertexMonitoringTool )
    
    log.debug(vertexFinder)
    
    return  [ vertexFinder ]


# linearised track factory, whatever that does, for the vertex finder

def  linearTrackFactory_builder( signature, extrapolator ) :

    from AthenaCommon.AppMgr import ToolSvc

    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory

    linearTrackFactory = Trk__FullLinearizedTrackFactory( name         = "FullLinearizedTrackFactory" + signature,
                                                          Extrapolator = extrapolator )
    ToolSvc += linearTrackFactory

    return linearTrackFactory



# vertex fitter for the vertex finder 

def  iterativeVertexFitterTool_builder( signature, linearTrackFactory, extrapolator ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
    vertexFitterTool = Trk__FastVertexFitter( name                   = "InDetTrigFastVertexFitterTool" + signature,
                                              LinearizedTrackFactory = linearTrackFactory,
                                              Extrapolator           = extrapolator ) 
    ToolSvc += vertexFitterTool
    
    return vertexFitterTool




# impact parameter estimator

def  impactEstimator_builder( signature, extrapolator ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
    impactPoint3dEstimator = Trk__ImpactPoint3dEstimator( name         = "InDetTrigImpactPoint3dEstimator" + signature,
                                                          Extrapolator = extrapolator )
   
    ToolSvc += impactPoint3dEstimator

    return impactPoint3dEstimator





def  trackSelectorTool_builder( signature, trackSummaryTool, extrapolator, cuts ) :

    from AthenaCommon.AppMgr import ToolSvc
    
    # now create the track selection tool itself ...
    
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
   
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
                                                        minNSiHits       = cuts.nHitSi() )

    ToolSvc += trackSelectorTool
   
    return trackSelectorTool




# the trackdensity seed finder builder ...

def  trackDensitySeedFinder_builder( signature ) :
    
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

def iterativeVertexFinderTool_builder( signature ) : 

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
    linearTrackFactory     =        linearTrackFactory_builder( signature, extrapolator )
    vertexFitterTool       = iterativeVertexFitterTool_builder( signature, linearTrackFactory, extrapolator )
    impactEstimator        =           impactEstimator_builder( signature, extrapolator )
    trackSelectorTool      =         trackSelectorTool_builder( signature, trackSummaryTool, extrapolator, vtxcuts )
    trackDensitySeedFinder =    trackDensitySeedFinder_builder( signature )
    
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

def vertexSortingTool_builder( signature ) :

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
def vertexMonitoringTool_builder( signature ) : 
    from InDetPriVxFinder.InDetPriVxFinderMonitoring import InDetPriVxFinderMonitoringTool
    return  InDetPriVxFinderMonitoringTool()



#------------------------------------------------------------------------------------------------------------------
#                         BUILDERS FOR ADAPTIVE MULTI VERTEX TOOL
#------------------------------------------------------------------------------------------------------------------

# create the actual vertex finder tool ...
def adaptiveMultiVertexFinderTool_builder( signature ) : 

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
    linearTrackFactory     =            linearTrackFactory_builder( signature, extrapolator )
    impactEstimator        =               impactEstimator_builder( signature, extrapolator )
    vertexFitterTool       =      adaptiveVertexFitterTool_builder( signature, linearTrackFactory, extrapolator,impactEstimator )
    trackSelectorTool      =             trackSelectorTool_builder( signature, trackSummaryTool, extrapolator, vtxcuts )
    seedFinder             =        trackDensitySeedFinder_builder( signature )
    # leave this here, but commented for the time being while we investigate ...
    # seedFinder           = adaptiveMultiVertexSeedFinder_builder( signature, doVtx3DFinding)


    # now create the actual vertex finder tool ...
    # this is the main part of the actual working part of the code - 
    # the vertoces are found by this class, in this instance it includes
    # a beam line constraint - it we want this to allow this constrain 
    # to be disabled we can add a flag and some additional logic 
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool

    vertexFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetTrigAdaptiveMultiPriVxFinderTool" + signature,
                                                                        SeedFinder        = seedFinder,
                                                                        VertexFitterTool  = vertexFitterTool,
                                                                        TrackSelector     = trackSelectorTool,
                                                                        useBeamConstraint = True,
                                                                        selectiontype     = 0, # what is this?
                                                                        do3dSplitting     = doVtx3DFinding)
    
    ToolSvc += vertexFinderTool
   
    return vertexFinderTool



# annealing for adaptive vertex finder
def annealingMaker_builder(signature) :

    from AthenaCommon.AppMgr import ToolSvc

    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
    annealingMaker = Trk__DetAnnealingMaker(name = "InDetTrigAnnealingMaker" + signature,
                                                     SetOfTemperatures = [1.]) 
    ToolSvc += annealingMaker
    
    return annealingMaker



# vertex fitter for the vertex finder 
# this one is for adaptive vertex finder tool
def  adaptiveVertexFitterTool_builder( signature, linearTrackFactory, extrapolator,impactEstimator ) :

    from AthenaCommon.AppMgr import ToolSvc

    annealingMaker  = annealingMaker_builder ( signature )
    from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveMultiVertexFitter
    vertexFitterTool = Trk__AdaptiveMultiVertexFitter(name                         = "InDetTrigAdaptiveMultiVertexFitterTool" + signature,
                                                      LinearizedTrackFactory       = linearTrackFactory,
                                                      ImpactPoint3dEstimator       = impactEstimator,
                                                      AnnealingMaker               = annealingMaker,
                                                      DoSmoothing                  = True) # false is default 
 
    ToolSvc += vertexFitterTool
    
    return vertexFitterTool




def adaptiveMultiVertexSeedFinder_builder( signature , doVtx3DFinding):

    from AthenaCommon.AppMgr import ToolSvc
    
    if (doVtx3DFinding): #InDetFlags.doPrimaryVertex3DFinding()

        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
        seedFinder = Trk__CrossDistancesSeedFinder( name              = "InDetTrigCrossDistancesSeedFinder" + signature,
                                                    trackdistcutoff   = 1.,
                                                    trackdistexppower = 2)
    
    else:
    
        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
        seedFinder = Trk__ZScanSeedFinder(name = "InDetTrigZScanSeedFinder" + signature
                                                      # Mode1dFinder = # default, no setting needed
                                                     )
    ToolSvc += seedFinder

    return seedFinder




