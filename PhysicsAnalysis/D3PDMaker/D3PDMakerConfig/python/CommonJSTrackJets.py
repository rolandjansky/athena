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
  JS_MyJetTrackZClusterTool.TrackJetMinPt = 50000 # MeV
  JS_MyJetTrackZClusterTool.UseVtxSeeding = True
  JS_MyJetTrackZClusterTool.DeltaZRange = 10000.0
  JS_MyJetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
  JS_MyJetTrackZClusterTool.VxContainerName = "VxPrimaryCandidate"
  JS_MyJetTrackZClusterTool.OutputLevel = 3

  #--------------------------------------------------------------
  # TrackSelector Tool Options
  #--------------------------------------------------------------
  from AthenaCommon.AppMgr import ToolSvc
  from D3PDMakerConfig.CommonTrackJetSeletorTool import GetCachedTrackSelectorTool
  if hasattr(ToolSvc, "CachedTrackSelectorTool"):
    JS_MyJetTrackZClusterTool.TrackSelector = ToolSvc.CachedTrackSelectorTool
  else:
    JS_MyJetTrackZClusterTool.TrackSelector = GetCachedTrackSelectorTool()

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
