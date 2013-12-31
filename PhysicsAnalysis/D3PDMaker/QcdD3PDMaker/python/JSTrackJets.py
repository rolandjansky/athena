# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# JetTrackZClusterTool jobOptions file
# # PhysicsAnalysis/JetEtMissDPDModifier/share/JetTrackZClusterTool_jobOptions.py
# # https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/D3PDMaker/SUSYD3PDMaker/trunk/share/JetTrackZClusterTool_jobOptions.py
#==============================================================

#--------------------------------------------------------------
# JetTrackZClusterTool Options
#--------------------------------------------------------------

# Import configurable for tool 
from JetRecTools.JetRecToolsConf import JetTrackZClusterTool
from JetRec.JetGetters import *
from JetRec.JetRecConf import *

def createJSTrackJets ( theseq, myjetfinder, myjetdr ):

  # Setup tool so that it can be used
  JS_MyJetTrackZClusterTool = JetTrackZClusterTool( "JS_JetTrackZClusterTool_%s%d" % (myjetfinder,myjetdr*10) )

  JS_MyJetTrackZClusterTool.TrackJetMinMulti = 2
  JS_MyJetTrackZClusterTool.TrackJetMinPt = 4000 # MeV
  JS_MyJetTrackZClusterTool.UseVtxSeeding = True
  JS_MyJetTrackZClusterTool.DeltaZRange = 10000.0
  JS_MyJetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
  JS_MyJetTrackZClusterTool.VxContainerName = "VxPrimaryCandidate"
  JS_MyJetTrackZClusterTool.OutputLevel = 3

  #--------------------------------------------------------------
  # TrackSelector Tool Options
  #--------------------------------------------------------------

  from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
  from AthenaCommon.AppMgr import ToolSvc
  trackSelector = InDet__InDetDetailedTrackSelectorTool( "JS_MyDetailedTrackSelectorTool" )
  ToolSvc += trackSelector

  #See InDetDetailedTrackSelectorTool.h for additional options and defaults

  trackSelector.pTMin                = 500 # MeV
  trackSelector.etaMax               = 2.5
  trackSelector.nHitBLayer           = 0
  trackSelector.nHitPix              = 0 
  trackSelector.nHitBLayerPlusPix    = 1 #0 
  trackSelector.nHitSct              = 6 #0
  trackSelector.nHitSi               = 7 #7
  trackSelector.nHitTrt              = 0
  trackSelector.IPd0Max              = 1.0 #1 # d0 cut
  trackSelector.IPz0Max              = 1.5 # z0*sin(theta) cut
  trackSelector.z0Max                = 200 # z0 cut
  trackSelector.fitChi2OnNdfMax      = 10000 #1000  #3.5 #3
  trackSelector.d0significanceMax    = -1.
  trackSelector.z0significanceMax    = -1.

  # Try to set InDet default tools to avoid strange bugs
  try:
    trackSelector.Extrapolator = ToolSvc.InDetExtrapolator
  except:
    from AthenaCommon.Logging import logging
    l = logging.getLogger("TrackSelectionForJets::setupTrackSelectorTool")
    l.warning("No ToolSvc.InDetExtrapolator available. Tracking might cause infinite loop")
    pass

  #trackSelector.OutputLevel = 3

  from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
  atst = AtlasTrackSummaryTool()
  ToolSvc += atst
  trackSelector.TrackSummaryTool = atst

##
  from JetSubStructure.JetSubStructureConf import JetSubStructure__CachedTrackSelectorTool
  CachedTrackSelector = JetSubStructure__CachedTrackSelectorTool("JS_CachedTrackSelectorTool")
  ToolSvc += CachedTrackSelector
  
  CachedTrackSelector.TrackSelector = trackSelector

##
  # Tell "JetTrackZClusterTool" to use this tool
  JS_MyJetTrackZClusterTool.TrackSelector = CachedTrackSelector.TrackSelector

  #--------------------------------------------------------------
  # JetFinder Tool Options (Anti-Kt)
  #--------------------------------------------------------------

  from JetRec.JetRecConf import JetFastJetFinderTool
  myfastfinder = JetFastJetFinderTool("JS_%s%dTrackJetFinder" % (myjetfinder,myjetdr*10))

  if myjetfinder == 'AntiKt':
    myfastfinder.Algorithm         = "anti-kt"
  elif myjetfinder == 'CamKt':
    myfastfinder.Algorithm         = "cambridge"

  myfastfinder.Radius              = myjetdr
  myfastfinder.RecombScheme        = "E"
  myfastfinder.Strategy            = "Best"
  myfastfinder.FailIfMisconfigured = True
  myfastfinder.Inclusive           = True
  myfastfinder.CalculateJetArea    = False
  myfastfinder.StoreNFlipValues    = 0

  ToolSvc += myfastfinder

  # Tell "TrackZClusterTool" to use this tool
  JS_MyJetTrackZClusterTool.JetFinder = myfastfinder

  #-------------------------------------------------------------
  # Jet Getter
  #-------------------------------------------------------------

  JS_TrackZToolList = [JS_MyJetTrackZClusterTool, 
                       JetSignalSelectorTool('JSTZ_JetFinalPtCut',UseTransverseMomentum = True,MinimumSignal= jetFlags.finalMinEt()), 
                       JetSorterTool('JSTZ_JetSorter',SortOrder="ByPtDown") ]

  mytrackzjetgetter = make_StandardJetGetter(myjetfinder, myjetdr,'TrackZ',seq = theseq, allTools = JS_TrackZToolList)

  return mytrackzjetgetter
#==============================================================
#
# End of job options file
#
###############################################################
