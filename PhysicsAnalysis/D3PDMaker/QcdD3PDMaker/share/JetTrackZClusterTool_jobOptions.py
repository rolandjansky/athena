###############################################################
#
# JetTrackZClusterTool jobOptions file
#
#==============================================================

#--------------------------------------------------------------
# JetTrackZClusterTool Options
#--------------------------------------------------------------

# Import configurable for tool 
from JetRecTools.JetRecToolsConf import JetTrackZClusterTool

# Setup tool so that it can be used
MyAKt4JetTrackZClusterTool = JetTrackZClusterTool( "MyAKt4TrackJetTool" )
MyAKt4JetTrackZClusterTool.TrackJetMinMulti           = 2
MyAKt4JetTrackZClusterTool.TrackJetMinPt              = 4000 # MeV
MyAKt4JetTrackZClusterTool.UseVtxSeeding              = True
MyAKt4JetTrackZClusterTool.DeltaZRange                = 10000.0
MyAKt4JetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
MyAKt4JetTrackZClusterTool.VxContainerName            = "VxPrimaryCandidate"
MyAKt4JetTrackZClusterTool.OutputLevel                = INFO

MyAKt6JetTrackZClusterTool = JetTrackZClusterTool( "MyAKt6TrackJetTool" )
MyAKt6JetTrackZClusterTool.TrackJetMinMulti           = 2
MyAKt6JetTrackZClusterTool.TrackJetMinPt              = 4000 # MeV
MyAKt6JetTrackZClusterTool.UseVtxSeeding              = True
MyAKt6JetTrackZClusterTool.DeltaZRange                = 10000.0
MyAKt6JetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
MyAKt6JetTrackZClusterTool.VxContainerName            = "VxPrimaryCandidate"
MyAKt6JetTrackZClusterTool.OutputLevel                = INFO

MyAKt5JetTrackZClusterTool = JetTrackZClusterTool( "MyAKt5TrackJetTool" )
MyAKt5JetTrackZClusterTool.TrackJetMinMulti           = 2
MyAKt5JetTrackZClusterTool.TrackJetMinPt              = 4000 # MeV
MyAKt5JetTrackZClusterTool.UseVtxSeeding              = True
MyAKt5JetTrackZClusterTool.DeltaZRange                = 10000.0
MyAKt5JetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
MyAKt5JetTrackZClusterTool.VxContainerName            = "VxPrimaryCandidate"
MyAKt5JetTrackZClusterTool.OutputLevel                = INFO

#MyKt4JetTrackZClusterTool = JetTrackZClusterTool( "MyKt4TrackJetTool" )
#MyKt4JetTrackZClusterTool.TrackJetMinMulti           = 2
#MyKt4JetTrackZClusterTool.TrackJetMinPt              = 1000 # MeV
#MyKt4JetTrackZClusterTool.UseVtxSeeding              = False
#MyKt4JetTrackZClusterTool.DeltaZRange                = 10.0
#MyKt4JetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
#MyKt4JetTrackZClusterTool.VxContainerName            = "VxPrimaryCandidate"
#MyKt4JetTrackZClusterTool.OutputLevel                = INFO

#MyKt6JetTrackZClusterTool = JetTrackZClusterTool( "MyKt6TrackJetTool" )
#MyKt6JetTrackZClusterTool.TrackJetMinMulti           = 2
#MyKt6JetTrackZClusterTool.TrackJetMinPt              = 1000 # MeV
#MyKt6JetTrackZClusterTool.UseVtxSeeding              = False
#MyKt6JetTrackZClusterTool.DeltaZRange                = 10.0
#MyKt6JetTrackZClusterTool.TrackParticleContainerName = "TrackParticleCandidate"
#MyKt6JetTrackZClusterTool.VxContainerName            = "VxPrimaryCandidate"
#MyKt6JetTrackZClusterTool.OutputLevel                = INFO

#--------------------------------------------------------------
# TrackSelector Tool Options
#--------------------------------------------------------------

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
ToolSvc += InDet__InDetDetailedTrackSelectorTool( "MyDetailedTrackSelectorTool",
                                                  TrackSummaryTool = ToolSvc.InDetTrackSummaryTool,
                                                  Extrapolator     = ToolSvc.InDetExtrapolator )
#See InDetDetailedTrackSelectorTool.h for additional options and defaults
ToolSvc.MyDetailedTrackSelectorTool.pTMin                = 500.
ToolSvc.MyDetailedTrackSelectorTool.etaMax               = 2.5
ToolSvc.MyDetailedTrackSelectorTool.nHitBLayer           = 0
ToolSvc.MyDetailedTrackSelectorTool.nHitPix              = 0 
ToolSvc.MyDetailedTrackSelectorTool.nHitBLayerPlusPix    = 1 
ToolSvc.MyDetailedTrackSelectorTool.nHitSct              = 6
ToolSvc.MyDetailedTrackSelectorTool.nHitSi               = 7
ToolSvc.MyDetailedTrackSelectorTool.nHitTrt              = 0
ToolSvc.MyDetailedTrackSelectorTool.IPd0Max              = 1.5
ToolSvc.MyDetailedTrackSelectorTool.IPz0Max              = 1.5 
ToolSvc.MyDetailedTrackSelectorTool.z0Max                = 200. 
ToolSvc.MyDetailedTrackSelectorTool.fitChi2OnNdfMax      = 10000
ToolSvc.MyDetailedTrackSelectorTool.d0significanceMax    = -1.
ToolSvc.MyDetailedTrackSelectorTool.z0significanceMax    = -1.
##ToolSvc.MyDetailedTrackSelectorTool.RequireInsideOut     = True

ToolSvc.MyDetailedTrackSelectorTool.OutputLevel = INFO

# Tell "JetTrackZClusterTool" to use this tool
MyAKt4JetTrackZClusterTool.TrackSelector = ToolSvc.MyDetailedTrackSelectorTool
MyAKt6JetTrackZClusterTool.TrackSelector = ToolSvc.MyDetailedTrackSelectorTool
MyAKt5JetTrackZClusterTool.TrackSelector = ToolSvc.MyDetailedTrackSelectorTool
#MyKt4JetTrackZClusterTool.TrackSelector = ToolSvc.MyDetailedTrackSelectorTool
#MyKt6JetTrackZClusterTool.TrackSelector = ToolSvc.MyDetailedTrackSelectorTool

#--------------------------------------------------------------
# JetFinder Tool Options (Anti-Kt)
#--------------------------------------------------------------
from JetRec.JetRecFlags import jetFlags
#from JetRec.JetRecConf import JetFastKtFinderTool

# Algorithm configuration taken from Reconstruction/Jet/JetRec/JetGetters.py (JetRec-01-01-58)
# Anti-Kt D=0.4
ToolSvc += JetFastJetFinderTool("myAntiKt4")
ToolSvc.myAntiKt4.Algorithm           = 'anti-kt'
ToolSvc.myAntiKt4.Radius              = 0.4
ToolSvc.myAntiKt4.RecombScheme        = "E"
ToolSvc.myAntiKt4.Strategy            = "Best"
ToolSvc.myAntiKt4.FailIfMisconfigured = True
ToolSvc.myAntiKt4.Inclusive           = True
ToolSvc.myAntiKt4.CalculateJetArea    = False
ToolSvc.myAntiKt4.StoreNFlipValues    = 2
MyAKt4JetTrackZClusterTool.JetFinder = ToolSvc.myAntiKt4

# Anti-Kt D=0.6
ToolSvc += JetFastJetFinderTool("myAntiKt6")
ToolSvc.myAntiKt6.Algorithm           = 'anti-kt'
ToolSvc.myAntiKt6.Radius              = 0.6
ToolSvc.myAntiKt6.RecombScheme        = "E"
ToolSvc.myAntiKt6.Strategy            = "Best"
ToolSvc.myAntiKt6.FailIfMisconfigured = True
ToolSvc.myAntiKt6.Inclusive           = True
ToolSvc.myAntiKt6.CalculateJetArea    = False
ToolSvc.myAntiKt6.StoreNFlipValues    = 2
MyAKt6JetTrackZClusterTool.JetFinder = ToolSvc.myAntiKt6

# Anti-Kt D=0.5
ToolSvc += JetFastJetFinderTool("myAntiKt5")
ToolSvc.myAntiKt5.Algorithm           = 'anti-kt'
ToolSvc.myAntiKt5.Radius              = 0.5
ToolSvc.myAntiKt5.RecombScheme        = "E"
ToolSvc.myAntiKt5.Strategy            = "Best"
ToolSvc.myAntiKt5.FailIfMisconfigured = True
ToolSvc.myAntiKt5.Inclusive           = True
ToolSvc.myAntiKt5.CalculateJetArea    = False
ToolSvc.myAntiKt5.StoreNFlipValues    = 2
MyAKt5JetTrackZClusterTool.JetFinder = ToolSvc.myAntiKt5

# Kt D=0.4
#ToolSvc += JetFastJetFinderTool("myKt4")
#ToolSvc.myKt4.Algorithm           = 'kt'
#ToolSvc.myKt4.Radius              = 0.4
#ToolSvc.myKt4.RecombScheme        = "E"
#ToolSvc.myKt4.Strategy            = "Best"
#ToolSvc.myKt4.FailIfMisconfigured = True
#ToolSvc.myKt4.Inclusive           = True
#ToolSvc.myKt4.CalculateJetArea    = False
#ToolSvc.myKt4.StoreNFlipValues    = 2
#MyKt4JetTrackZClusterTool.JetFinder = ToolSvc.myKt4

# Kt D=0.6
#ToolSvc += JetFastJetFinderTool("myKt6")
#ToolSvc.myKt6.Algorithm           = 'kt'
#ToolSvc.myKt6.Radius              = 0.6
#ToolSvc.myKt6.RecombScheme        = "E"
#ToolSvc.myKt6.Strategy            = "Best"
#ToolSvc.myKt6.FailIfMisconfigured = True
#ToolSvc.myKt6.Inclusive           = True
#ToolSvc.myKt6.CalculateJetArea    = False
#ToolSvc.myKt6.StoreNFlipValues    = 2
#MyKt6JetTrackZClusterTool.JetFinder = ToolSvc.myKt6

#==============================================================
#
# End of job options file
#
###############################################################
