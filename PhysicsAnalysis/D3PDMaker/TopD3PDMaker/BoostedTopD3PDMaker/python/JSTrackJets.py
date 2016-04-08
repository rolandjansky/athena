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
  JS_MyJetTrackZClusterTool.TrackJetMinPt = 5000 # MeV
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
  ToolSvc += InDet__InDetDetailedTrackSelectorTool( "JS_MyDetailedTrackSelectorTool" )

  #See InDetDetailedTrackSelectorTool.h for additional options and defaults

  ToolSvc.JS_MyDetailedTrackSelectorTool.pTMin                = 500 # MeV
  ToolSvc.JS_MyDetailedTrackSelectorTool.etaMax               = 2.5
  ToolSvc.JS_MyDetailedTrackSelectorTool.nHitBLayer           = 0
  ToolSvc.JS_MyDetailedTrackSelectorTool.nHitPix              = 0 
  ToolSvc.JS_MyDetailedTrackSelectorTool.nHitBLayerPlusPix    = 1 #0 
  ToolSvc.JS_MyDetailedTrackSelectorTool.nHitSct              = 6 #0
  ToolSvc.JS_MyDetailedTrackSelectorTool.nHitSi               = 7 #7
  ToolSvc.JS_MyDetailedTrackSelectorTool.nHitTrt              = 0
  ToolSvc.JS_MyDetailedTrackSelectorTool.IPd0Max              = 1.0 #1 # d0 cut
  ToolSvc.JS_MyDetailedTrackSelectorTool.IPz0Max              = 1.5 # z0*sin(theta) cut
  ToolSvc.JS_MyDetailedTrackSelectorTool.z0Max                = 200 # z0 cut
  ToolSvc.JS_MyDetailedTrackSelectorTool.fitChi2OnNdfMax      = 1000  #3 #Should we use a tighter cut here !???
  ToolSvc.JS_MyDetailedTrackSelectorTool.d0significanceMax    = -1.
  ToolSvc.JS_MyDetailedTrackSelectorTool.z0significanceMax    = -1.
  ToolSvc.JS_MyDetailedTrackSelectorTool.Extrapolator = ToolSvc.InDetExtrapolator

  ToolSvc.JS_MyDetailedTrackSelectorTool.OutputLevel = 3

  from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
  atst = AtlasTrackSummaryTool()
  ToolSvc += atst
  ToolSvc.JS_MyDetailedTrackSelectorTool.TrackSummaryTool = atst

##
  from JetSubStructure.JetSubStructureConf import JetSubStructure__CachedTrackSelectorTool
  ToolSvc += JetSubStructure__CachedTrackSelectorTool("JS_CachedTrackSelectorTool")
  
  ToolSvc.JS_CachedTrackSelectorTool.TrackSelector = ToolSvc.JS_MyDetailedTrackSelectorTool

##
  # Tell "JetTrackZClusterTool" to use this tool
  JS_MyJetTrackZClusterTool.TrackSelector = ToolSvc.JS_CachedTrackSelectorTool #ToolSvc.JS_MyDetailedTrackSelectorTool


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
