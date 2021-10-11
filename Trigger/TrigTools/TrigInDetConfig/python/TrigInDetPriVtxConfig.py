#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
def makeVertices( whichSignature, inputTrackCollection, outputVtxCollection=None, config=None, adaptiveVertex=None ) :

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

def vertexFinderCfg(flags, signature, inputTracks, outputVertices, adaptiveVertexing):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from InDetPriVxFinder.InDetPriVxFinderMonitoring import (
        InDetPriVxFinderMonitoringTool,
    )

    acc = ComponentAccumulator()
    if adaptiveVertexing:
        vtxFinderTool = acc.popToolsAndMerge(
            adaptiveMultiVertexFinderCfg(flags, signature)
        )
        alg_name = f"InDetTrigAMVFPriVxFinder{signature}"
    else:
        vtxFinderTool = acc.popToolsAndMerge(iterativeVertexFinderCfg(flags, signature))
        alg_name = f"InDetTrigIterativePriVxFinder{signature}"

    acc.addEventAlgo(
        CompFactory.InDet.InDetPriVxFinder(
            alg_name,
            VertexFinderTool=vtxFinderTool,
            TracksName=inputTracks,
            VxCandidatesOutputName=outputVertices,
            VertexCollectionSortingTool=CompFactory.Trk.VertexCollectionSortingTool(
                f"InDetVertexCollectionSortingTool{signature}",
                VertexWeightCalculator=CompFactory.Trk.SumPtVertexWeightCalculator(
                    f"InDetSumPtVertexWeightCalculator{signature}",
                    DoSumPt2Selection=True,
                ),
            ),
            doVertexSorting=True,
            PriVxMonTool=InDetPriVxFinderMonitoringTool(),
        ),
        primary=True,
    )
    return acc
    

def iterativeVertexFinderCfg(flags, signature):
    """ Configure the iterative vertex finder """
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolCfg
    from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
    from InDetTrigRecExample.TrigInDetConfiguredVtxCuts import ConfiguredTrigVtxCuts 

    acc = ComponentAccumulator()
    vtx_cuts = ConfiguredTrigVtxCuts()
    # TODO - should this have isHLT = True? This isn't set in the non-CA version
    summary_tool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    extrapolator_acc = InDetExtrapolatorCfg(flags)
    extrapolator = extrapolator_acc.getPrimary()
    acc.merge(extrapolator_acc)
    linear_track_factory = CompFactory.Trk.FullLinearizedTrackFactory(
        f"FullLinearizedTrackFactory{signature}",
        Extrapolator=extrapolator,
    )
    acc.setPrivateTools(
        CompFactory.InDet.InDetIterativePriVxFinderTool(
            f"InDetTrigPriVxFinderTool{signature}",
            VertexFitterTool=CompFactory.Trk.FastVertexFitter(
                f"InDetTrigFastVertexFitterTool{signature}",
                LinearizedTrackFactory=linear_track_factory,
                Extrapolator=extrapolator,
            ),
            TrackSelector=acc.popToolsAndMerge(
                trackSelectorToolCfg(flags, signature, summary_tool, extrapolator),
            ),
            SeedFinder=CompFactory.Trk.TrackDensitySeedFinder(
                f"TrigGaussianDensitySeed{signature}",
                DensityEstimator=CompFactory.Trk.GaussianTrackDensity(
                    f"TrigGaussianDensity{signature}"
            ),
            ),
            ImpactPoint3dEstimator=CompFactory.Trk.ImpactPoint3dEstimator(
                f"InDetTrigImpactPoint3dEstimator{signature}",
                Extrapolator=extrapolator,
            ),
            LinearizedTrackFactory=linear_track_factory,
            useBeamConstraint=True,
            significanceCutSeeding=12,
            maximumChi2cutForSeeding=29,
            maxVertices=200,
            createSplitVertices=False,
            doMaxTracksCut=vtx_cuts.doMaxTracksCut(),
            MaxTracks=vtx_cuts.MaxTracks(),
        )
    )
    return acc


def adaptiveMultiVertexFinderCfg(flags, signature):
    """ Configure the adaptive multi-vertex finder """
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolCfg
    from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    acc = ComponentAccumulator()
    # TODO - should this have isHLT = True? This isn't set in the non-CA version
    summary_tool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    extrapolator_acc = InDetExtrapolatorCfg(flags)
    extrapolator = extrapolator_acc.getPrimary()
    acc.merge(extrapolator_acc)
    config = getInDetTrigConfig(signature)

    acc.setPrivateTools(
        CompFactory.InDet.InDetAdaptiveMultiPriVxFinderTool(
            f"InDetTrigAdaptiveMultiPriVxFinderTool{signature}",
            SeedFinder=CompFactory.Trk.TrackDensitySeedFinder(
                f"TrigGaussianDensitySeed{signature}",
                DensityEstimator=CompFactory.Trk.GaussianTrackDensity(
                    f"TrigGaussianDensity{signature}"
            ),
            ),
            VertexFitterTool=CompFactory.Trk.AdaptiveMultiVertexFitter(
                f"InDetTrigAdaptivemultiVertexFitterTool{signature}",
                LinearizedTrackFactory=CompFactory.Trk.FullLinearizedTrackFactory(
                    f"FullLinearizedTrackFactory{signature}",
                    Extrapolator=extrapolator,
                ),
                ImpactPoint3dEstimator=CompFactory.Trk.ImpactPoint3dEstimator(
                    f"InDetTrigImpactPoint3dEstimator{signature}",
                    Extrapolator=extrapolator,
            ),
                AnnealingMaker=CompFactory.Trk.DetAnnealingMaker(
                    f"InDetTrigAnnealingMaker{signature}",
                    SetOfTemperatures=[1.0],
                ),
                DoSmoothing=True,
            ),
            TrackSelector=acc.popToolsAndMerge(
                trackSelectorToolCfg(flags, signature, summary_tool, extrapolator),
            ),
            useBeamConstraint=True,
            TracksMaxZinterval=config.TracksMaxZinterval,
            addSingleTrackVertices=config.addSingleTrackVertices,
            selectiontype=0,
            do3dSplitting=True,  # NB: comment from original function suggests that this should be flags.InDet.doPrimaryVertex3DFinding
        )
    )
    return acc


def trackSelectorToolCfg(flags, signature, summaryTool, extrapolator):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from InDetTrigRecExample.TrigInDetConfiguredVtxCuts import ConfiguredTrigVtxCuts

    acc = ComponentAccumulator()
    cuts = ConfiguredTrigVtxCuts()
    cuts.printInfo()

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    config = getInDetTrigConfig(signature)

    minNSiHits_vtx = (
        config.minNSiHits_vtx if config.minNSiHits_vtx is not None else cuts.nHitSi()
    )

    acc.setPrivateTools(
        CompFactory.InDet.InDetTrackSelectionTool(
        f"InDetTrigDetailedTrackSelectionTool{signature}",
        Extrapolator=extrapolator,
        TrackSummaryTool=summaryTool,
        CutLevel=cuts.TrackCutLevel(),
        minPt=cuts.minPT(),
        maxD0=cuts.IPd0Max(),
        maxZ0=cuts.z0Max(),
        maxZ0SinTheta=cuts.IPz0Max(),
        maxSigmaD0=cuts.sigIPd0Max(),
        maxSigmaZ0SinTheta=cuts.sigIPz0Max(),
        maxChiSqperNdf=cuts.fitChi2OnNdfMax(),
        maxAbsEta=cuts.etaMax(),
        minNInnermostLayerHits=cuts.nHitInnermostLayer(),
        minNPixelHits=cuts.nHitPix(),
        maxNPixelHoles=cuts.nHolesPix(),
        minNSctHits=cuts.nHitSct(),
        minNTrtHits=cuts.nHitTrt(),
        minNSiHits=minNSiHits_vtx,
        )
    )
    return acc


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
    
    acts = False

    if acts is False:
        vertexFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetTrigAdaptiveMultiPriVxFinderTool" + signature,
                                                                    SeedFinder        = seedFinder,
                                                                    VertexFitterTool  = vertexFitterTool,
                                                                    TrackSelector     = trackSelectorTool,
                                                                    useBeamConstraint = True,
                                                                    TracksMaxZinterval = tracksMaxZinterval,
                                                                    addSingleTrackVertices = singleTrackVertices,
                                                                    selectiontype     = 0, # what is this?
                                                                    do3dSplitting     = doVtx3DFinding )
    # not yet     
    # else:
    #     from ActsPriVtxFinder.ActsPriVtxFinderConf import ActsAdaptiveMultiPriVtxFinderTool
        
    #     actsTrackingGeometryTool = getattr(ToolSvc,"ActsTrackingGeometryTool")
        
    #     actsExtrapolationTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
    #     actsExtrapolationTool.TrackingGeometryTool = actsTrackingGeometryTool
        
    #     vertexFinderTool = ActsAdaptiveMultiPriVtxFinderTool(name  = "ActsAdaptiveMultiPriVtxFinderTool" + signature,
    #                                                          TrackSelector      = trackSelectorTool,
    #                                                          useBeamConstraint  = True,
    #                                                          tracksMaxZinterval = 3,#mm 
    #                                                          do3dSplitting      = doVtx3DFinding, 
    #                                                          TrackingGeometryTool = actsTrackingGeometryTool,
    #                                                          ExtrapolationTool  = actsExtrapolationTool )
         

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




