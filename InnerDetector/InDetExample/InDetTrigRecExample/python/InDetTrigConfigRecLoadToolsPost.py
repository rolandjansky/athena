# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" InDetTrigConfigRecLoadToolsPost
    various tools for postprocessing
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.2 $"
__doc__    = "InDetTrigConfigRecLoadToolsPost"


# common things
from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
InDetTrigCutValues = EFIDTrackingCuts



from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator,InDetTrigTrackSelectorTool
import AthenaCommon.SystemOfUnits as Units

from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv


if InDetTrigFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or \
      InDetTrigFlags.primaryVertexSetup() == 'IterativeFinding' or \
      InDetTrigFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
  
  #common tools
  if (InDetTrigFlags.doPrimaryVertex3DFinding()):
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
    InDetTrigVtxSeedFinder = Trk__CrossDistancesSeedFinder(name = "InDetTrigCrossDistancesSeedFinder",
                                                           trackdistcutoff = 1.,
                                                           trackdistexppower = 2)
  else:
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
    InDetTrigVtxSeedFinder = Trk__ZScanSeedFinder(name = "InDetTrigZScanSeedFinder"
                                                  #Mode1dFinder = # default, no setting needed
                                                  )
  ToolSvc += InDetTrigVtxSeedFinder
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigVtxSeedFinder

  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import \
      Trk__ImpactPoint3dEstimator, Trk__DetAnnealingMaker
  
  InDetTrigImpactPoint3dEstimator = \
      Trk__ImpactPoint3dEstimator(name         = "InDetTrigTrkImpactPoint3dEstimator",
                                  Extrapolator = InDetTrigExtrapolator,
                                  )
      
  ToolSvc += InDetTrigImpactPoint3dEstimator
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigImpactPoint3dEstimator


  InDetTrigAnnealingMaker = Trk__DetAnnealingMaker(name = "InDetTrigTrkAnnealingMaker")
  InDetTrigAnnealingMaker.SetOfTemperatures = [64.,16.,4.,2.,1.5,1.] # not default
  ToolSvc += InDetTrigAnnealingMaker
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigAnnealingMaker

      
  from TrkVertexFitters.TrkVertexFittersConf import Trk__DummyVertexSmoother
  InDetTrigDummyVertexSmoother =  Trk__DummyVertexSmoother(name="InDetTrigDummyVertexSmoother",
                                                           ExtrapolatorInstance = "InDetTrigExtrapolator",
                                                           ExtrapolatorName="Trk::Extrapolator")
  ToolSvc += InDetTrigDummyVertexSmoother
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigDummyVertexSmoother


#smoother
if InDetTrigFlags.primaryVertexSetup() == 'DefaultKalmanFinding' or \
      InDetTrigFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or \
      InDetTrigFlags.primaryVertexSetup() == 'IterativeFinding':
  
  from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
  InDetTrigVertexSmoother =  Trk__SequentialVertexSmoother(name="InDetTrigSequentialVertexSmoother")
  
  ToolSvc += InDetTrigVertexSmoother
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigVertexSmoother


#
if InDetTrigFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or \
      InDetTrigFlags.primaryVertexSetup() == 'IterativeFinding':
  from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
  InDetTrigVxFitterTool = Trk__AdaptiveVertexFitter(name                         = "InDetTrigAdaptiveVxFitterTool",
                                                    SeedFinder                   = InDetTrigVtxSeedFinder,
                                                    LinearizedTrackFactory       = InDetTrigLinFactory,
                                                    ImpactPoint3dEstimator       = InDetTrigImpactPoint3dEstimator,
                                                    AnnealingMaker               = InDetTrigAnnealingMaker,
                                                    #TrackCompatibilityEstimator = 
                                                    VertexSmoother = InDetTrigVertexSmoother,
                                                    #VertexUpdator = ,  #does not use any tools?
                                                    #onlyzseed = ,
                                                    )

elif InDetTrigFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':


  from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveMultiVertexFitter
  InDetTrigVxFitterTool = \
      Trk__AdaptiveMultiVertexFitter(name= "InDetTrigAdaptiveMultiVertexFitter",
                                     LinearizedTrackFactory   = InDetTrigLinFactory,
                                     ImpactPoint3dEstimator   = InDetTrigImpactPoint3dEstimator,

                                     AnnealingMaker           = InDetTrigAnnealingMaker,
                                     VertexSmoother           = InDetTrigSequentialVertexSmoother,
                                     DoSmoothing              = True) # false is default

else:
   
  # load fast vertex finding tool and configure it
  #
  from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
  InDetTrigVxFitterTool = Trk__FastVertexFitter( name = "InDetTrigVxFitterTool",
                                                 LinearizedTrackFactory       = InDetTrigLinFactory,
                                                 Extrapolator = InDetTrigExtrapolator )

InDetTrigVxFitterTool.XAODConverter = InDetTrigVxEdmCnv
ToolSvc += InDetTrigVxFitterTool
if (InDetTrigFlags.doPrintConfigurables()):
   print  InDetTrigVxFitterTool    


from InDetMultipleVertexSeedFinderUtils.InDetMultipleVertexSeedFinderUtilsConf import InDet__InDetTrackZ0SortingTool
InDetTrigTrackZ0SortingTool =  InDet__InDetTrackZ0SortingTool(name = "InDetTrigTrackZ0SortingTool")
ToolSvc += InDetTrigTrackZ0SortingTool
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigTrackZ0SortingTool



if (not InDetTrigFlags.useBeamConstraint()):
  from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2dDistanceSeeder
  TrkTrig2dDistanceSeeder = Trk__Trk2dDistanceSeeder(name = "TrkTrig2dDistanceSeederFor2D",
                                                     SolveAmbiguityUsingZ = False
  )
  
  ToolSvc+=TrkTrig2dDistanceSeeder
  if (InDetTrigFlags.doPrintConfigurables()):
    print TrkTrig2dDistanceSeeder
  #
  from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2DDistanceFinder
  TrkTrig2DDistanceFinder = Trk__Trk2DDistanceFinder(name = "TrkTrig2DDistanceFinder",
                                                     Trk2dDistanceSeeder = TrkTrig2dDistanceSeeder)

  ToolSvc+=TrkTrig2DDistanceFinder
  if (InDetTrigFlags.doPrintConfigurables()):
    print TrkTrig2DDistanceFinder                                     

  from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
  InDetTrig2DVtxSeedFinder = Trk__CrossDistancesSeedFinder(name = "InDetTrig2DCrossDistancesSeedFinder",
                                                           TrkDistanceFinder = TrkTrig2DDistanceFinder,
                                                           trackdistcutoff = 1.,
                                                           trackdistexppower = 2,
                                                           useweights=True
                                                           #Mode1dFinder = # default, no setting needed
                                                       )
  ToolSvc+=InDetTrig2DVtxSeedFinder
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrig2DVtxSeedFinder   

if InDetTrigFlags.vertexSeedFinder() == 'SlidingWindowMultiSeedFinder':
  #now setup new stuff
  from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
  InDetTrigMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name = "InDetTrigSlidingWindowMultiSeedFinder",
                                                                 clusterLength = 8.*Units.mm,
                                                                 TrackSelector = InDetTrigTrackSelectorTool,
                                                                 Extrapolator  = InDetTrigExtrapolator,
                                                                 SortingTool   = InDetTrigTrackZ0SortingTool,
                                                                 #IgnoreBeamSpot = True,
                                                                 #VertexSeedFinder = InDetTrig2DVtxSeedFinder
                                                                 #UseMaxInCluster = True
                                                                 )
  if (InDetTrigFlags.useBeamConstraint()):
    InDetTrigMultiSeedFinder.clusterLength = 5.*Units.mm
    InDetTrigMultiSeedFinder.IgnoreBeamSpot = False
  else:
    InDetTrigMultiSeedFinder.VertexSeedFinder = InDetTrig2DVtxSeedFinder

  ToolSvc += InDetTrigMultiSeedFinder



if InDetTrigFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
   from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
   InDetTrigPriVxFinderTool = \
       InDet__InDetAdaptiveMultiPriVxFinderTool( name = "InDetTrigAdaptiveMultiPriVxFinderTool",
                                                 VertexFitterTool = InDetTrigVxFitterTool,
                                                 TrackSelector = InDetTrigTrackSelectorTool,
                                                 useBeamConstraint = InDetTrigFlags.useBeamConstraint(),
                                                 selectiontype = 0,
                                                 InternalEdmFactory =InDetTrigVxEdmCnv
                                                 )
elif InDetTrigFlags.primaryVertexSetup() == 'IterativeFinding':
  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
  InDetTrigPriVxFinderTool = \
      InDet__InDetIterativePriVxFinderTool(name             = "InDetTrigIterativePriVxFinderTool",
                                           VertexFitterTool = InDetTrigVxFitterTool,
                                           TrackSelector    = InDetTrigTrackSelectorTool,
                                           SeedFinder = InDetTrigVtxSeedFinder,
                                           ImpactPoint3dEstimator = InDetTrigImpactPoint3dEstimator,
                                           LinearizedTrackFactory = InDetTrigLinFactory,
                                           useBeamConstraint = InDetTrigFlags.useBeamConstraint(),
                                           significanceCutSeeding = 12,
                                           maximumChi2cutForSeeding = 49,
                                           maxVertices = 200,
                                           InternalEdmFactory =InDetTrigVxEdmCnv
                                           )

else:
  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
  InDetTrigPriVxFinderTool = \
      InDet__InDetPriVxFinderTool( name = "InDetTrigPriVxFinderTool",
                                   VertexFitterTool = InDetTrigVxFitterTool,
                                   TrackSelector = InDetTrigTrackSelectorTool,
                                   useBeamConstraint = InDetTrigFlags.useBeamConstraint(),
                                   InternalEdmFactory =InDetTrigVxEdmCnv
                                   )
      
  #from AthenaCommon.BeamFlags import jobproperties
  # if jobproperties.Beam.zeroLuminosity():
  #    InDetTrigPriVxFinderTool.enableMultipleVertices = 0;
  # else:
  InDetTrigPriVxFinderTool.enableMultipleVertices = 1;
  #    InDetTrigPriVxFinderTool.useBeamConstraint = InDetTrigFlags.useBeamConstraint()
  if InDetTrigFlags.vertexSeedFinder() == 'SlidingWindowMultiSeedFinder':
    InDetTrigPriVxFinderTool.PriVxSeedFinder = InDetTrigMultiSeedFinder

       
  #can be switched off by enableMultipleVertices=0?

InDetTrigPriVxFinderTool.useBeamConstraint = InDetTrigFlags.useBeamConstraint()
    
ToolSvc += InDetTrigPriVxFinderTool
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigPriVxFinderTool

                            

