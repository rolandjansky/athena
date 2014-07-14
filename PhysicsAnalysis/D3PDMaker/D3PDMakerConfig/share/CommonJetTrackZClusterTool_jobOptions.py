###############################################################
#
# JetTrackZClusterTool jobOptions file
#
# updated May 2011, with recommendations from:
#  TopInputsD3PDMaker/share/ PrepareTrackJets.py
#  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackJetsInAthena
#  https://svnweb.cern.ch/trac/atlasgrp/browser/Institutes/SLAC/JetCaloValidation/trunk/share/Configs/Configure7TeVTrackJets.py 
#==============================================================

from JetRec.JetGetters import make_StandardJetGetter
try:
    from JetRec.TrackSelectionForJets import getStandardTrackJetFinderTool
    TrackJetAlg1=make_StandardJetGetter('AntiKt', 0.4, 'TrackZ', outputCollectionName='AntiKt4TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    #TrackJetAlg2=make_StandardJetGetter('AntiKt', 0.6, 'TrackZ', outputCollectionName='AntiKt6TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    #TrackJetAlg3=make_StandardJetGetter('AntiKt', 0.5, 'TrackZ', outputCollectionName='AntiKt5TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    #TrackJetAlg4=make_StandardJetGetter('AntiKt', 0.7, 'TrackZ', outputCollectionName='AntiKt7TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    TrackJetAlg5=make_StandardJetGetter('AntiKt', 0.2, 'TrackZ', outputCollectionName='AntiKt2TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    TrackJetAlg6=make_StandardJetGetter('AntiKt', 0.3, 'TrackZ', outputCollectionName='AntiKt3TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    #TrackJetAlg7=make_StandardJetGetter('AntiKt', 0.8, 'TrackZ', outputCollectionName='AntiKt8TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
    TrackJetAlg8=make_StandardJetGetter('Kt', 0.3, 'TrackZ', outputCollectionName='Kt3TrackJets', doCalib=False, jetMoments=[] , minPt=5*GeV).jetAlgorithmHandle()
except:
    #--------------------------------------------------------------
    # JetTrackZClusterTool Options
    #--------------------------------------------------------------

    # Import configurable for tool
    from JetRecTools.JetRecToolsConf import JetTrackZClusterTool

    # Setup tool so that it can be used
    MyJetTrackZClusterTool1 = JetTrackZClusterTool( "IAmATool1" )
    MyJetTrackZClusterTool1.TrackJetMinMulti = 2   # min num tracks to create track jet         
    MyJetTrackZClusterTool1.TrackJetMinPt = 5000   # MeV, min pt to create a track jet    
    MyJetTrackZClusterTool1.UseVtxSeeding = True   # seed using primary vertices?        
    MyJetTrackZClusterTool1.DeltaZRange = 10000.0  # z-clustering window
    MyJetTrackZClusterTool1.TrackParticleContainerName = "TrackParticleCandidate"
    MyJetTrackZClusterTool1.VxContainerName = "VxPrimaryCandidate"
    MyJetTrackZClusterTool1.OutputLevel = INFO

    #MyJetTrackZClusterTool2 = JetTrackZClusterTool( "IAmATool2" )
    #MyJetTrackZClusterTool2.TrackJetMinMulti = 2
    #MyJetTrackZClusterTool2.TrackJetMinPt = 5000
    #MyJetTrackZClusterTool2.UseVtxSeeding = True
    #MyJetTrackZClusterTool2.DeltaZRange = 10000.0
    #MyJetTrackZClusterTool2.TrackParticleContainerName = "TrackParticleCandidate"
    #MyJetTrackZClusterTool2.VxContainerName = "VxPrimaryCandidate"
    #MyJetTrackZClusterTool2.OutputLevel = INFO

    #MyJetTrackZClusterTool3 = JetTrackZClusterTool( "IAmATool3" )
    #MyJetTrackZClusterTool3.TrackJetMinMulti = 2
    #MyJetTrackZClusterTool3.TrackJetMinPt = 5000
    #MyJetTrackZClusterTool3.UseVtxSeeding = True
    #MyJetTrackZClusterTool3.DeltaZRange = 10000.0
    #MyJetTrackZClusterTool3.TrackParticleContainerName = "TrackParticleCandidate"
    #MyJetTrackZClusterTool3.VxContainerName = "VxPrimaryCandidate"
    #MyJetTrackZClusterTool3.OutputLevel = INFO

    #MyJetTrackZClusterTool4 = JetTrackZClusterTool( "IAmATool4" )
    #MyJetTrackZClusterTool4.TrackJetMinMulti = 2
    #MyJetTrackZClusterTool4.TrackJetMinPt = 5000
    #MyJetTrackZClusterTool4.UseVtxSeeding = True
    #MyJetTrackZClusterTool4.DeltaZRange = 10000.0
    #MyJetTrackZClusterTool4.TrackParticleContainerName = "TrackParticleCandidate"
    #MyJetTrackZClusterTool4.VxContainerName = "VxPrimaryCandidate"
    #MyJetTrackZClusterTool4.OutputLevel = INFO

    MyJetTrackZClusterTool5 = JetTrackZClusterTool( "IAmATool5" )
    MyJetTrackZClusterTool5.TrackJetMinMulti = 2
    MyJetTrackZClusterTool5.TrackJetMinPt = 5000
    MyJetTrackZClusterTool5.UseVtxSeeding = True
    MyJetTrackZClusterTool5.DeltaZRange = 10000.0
    MyJetTrackZClusterTool5.TrackParticleContainerName = "TrackParticleCandidate"
    MyJetTrackZClusterTool5.VxContainerName = "VxPrimaryCandidate"
    MyJetTrackZClusterTool5.OutputLevel = INFO

    MyJetTrackZClusterTool6 = JetTrackZClusterTool( "IAmATool6" )
    MyJetTrackZClusterTool6.TrackJetMinMulti = 2
    MyJetTrackZClusterTool6.TrackJetMinPt = 5000
    MyJetTrackZClusterTool6.UseVtxSeeding = True
    MyJetTrackZClusterTool6.DeltaZRange = 10000.0
    MyJetTrackZClusterTool6.TrackParticleContainerName = "TrackParticleCandidate"
    MyJetTrackZClusterTool6.VxContainerName = "VxPrimaryCandidate"
    MyJetTrackZClusterTool6.OutputLevel = INFO

    #MyJetTrackZClusterTool7 = JetTrackZClusterTool( "IAmATool7" )
    #MyJetTrackZClusterTool7.TrackJetMinMulti = 2
    #MyJetTrackZClusterTool7.TrackJetMinPt = 5000
    #MyJetTrackZClusterTool7.UseVtxSeeding = True
    #MyJetTrackZClusterTool7.DeltaZRange = 10000.0
    #MyJetTrackZClusterTool7.TrackParticleContainerName = "TrackParticleCandidate"
    #MyJetTrackZClusterTool7.VxContainerName = "VxPrimaryCandidate"
    #MyJetTrackZClusterTool7.OutputLevel = INFO

    MyJetTrackZClusterTool8 = JetTrackZClusterTool( "IAmATool8" )
    MyJetTrackZClusterTool8.TrackJetMinMulti = 2
    MyJetTrackZClusterTool8.TrackJetMinPt = 5000
    MyJetTrackZClusterTool8.UseVtxSeeding = True
    MyJetTrackZClusterTool8.DeltaZRange = 10000.0
    MyJetTrackZClusterTool8.TrackParticleContainerName = "TrackParticleCandidate"
    MyJetTrackZClusterTool8.VxContainerName = "VxPrimaryCandidate"
    MyJetTrackZClusterTool8.OutputLevel = INFO

    from D3PDMakerConfig.CommonTrackJetSeletorTool import GetCachedTrackSelectorTool
    if not hasattr(ToolSvc, "CachedTrackSelectorTool"):
        GetCachedTrackSelectorTool()
    MyJetTrackZClusterTool1.TrackSelector = ToolSvc.CachedTrackSelectorTool
    #MyJetTrackZClusterTool2.TrackSelector = ToolSvc.CachedTrackSelectorTool
    #MyJetTrackZClusterTool3.TrackSelector = ToolSvc.CachedTrackSelectorTool
    #MyJetTrackZClusterTool4.TrackSelector = ToolSvc.CachedTrackSelectorTool
    MyJetTrackZClusterTool5.TrackSelector = ToolSvc.CachedTrackSelectorTool
    MyJetTrackZClusterTool6.TrackSelector = ToolSvc.CachedTrackSelectorTool    
    #MyJetTrackZClusterTool7.TrackSelector = ToolSvc.CachedTrackSelectorTool
    MyJetTrackZClusterTool8.TrackSelector = ToolSvc.CachedTrackSelectorTool

    #--------------------------------------------------------------
    # JetFinder Tool Options (Anti-Kt)
    #--------------------------------------------------------------

    from JetRec.JetRecConf import JetFastJetFinderTool

    fastfinder1 = JetFastJetFinderTool("fastfinder1")
    fastfinder1.Algorithm           = "anti-kt"
    fastfinder1.Radius              = 0.4
    fastfinder1.RecombScheme        = "E"
    fastfinder1.Strategy            = "Best"
    fastfinder1.FailIfMisconfigured = True
    fastfinder1.Inclusive           = True
    fastfinder1.CalculateJetArea    = False
    fastfinder1.StoreNFlipValues    = 2
    ToolSvc += fastfinder1

    #fastfinder2 = JetFastJetFinderTool("fastfinder2")
    #fastfinder2.Algorithm           = "anti-kt"
    #fastfinder2.Radius              = 0.6
    #fastfinder2.RecombScheme        = "E"
    #fastfinder2.Strategy            = "Best"
    #fastfinder2.FailIfMisconfigured = True
    #fastfinder2.Inclusive           = True
    #fastfinder2.CalculateJetArea    = False
    #fastfinder2.StoreNFlipValues    = 2
    #ToolSvc += fastfinder2

    #fastfinder3 = JetFastJetFinderTool("fastfinder3")
    #fastfinder3.Algorithm           = "anti-kt"
    #fastfinder3.Radius              = 0.5
    #fastfinder3.RecombScheme        = "E"
    #fastfinder3.Strategy            = "Best"
    #fastfinder3.FailIfMisconfigured = True
    #fastfinder3.Inclusive           = True
    #fastfinder3.CalculateJetArea    = False
    #fastfinder3.StoreNFlipValues    = 2
    #ToolSvc += fastfinder3

    #fastfinder4 = JetFastJetFinderTool("fastfinder4")
    #fastfinder4.Algorithm           = "anti-kt"
    #fastfinder4.Radius              = 0.7
    #fastfinder4.RecombScheme        = "E"
    #fastfinder4.Strategy            = "Best"
    #fastfinder4.FailIfMisconfigured = True
    #fastfinder4.Inclusive           = True
    #fastfinder4.CalculateJetArea    = False
    #fastfinder4.StoreNFlipValues    = 2
    #ToolSvc += fastfinder4

    fastfinder5 = JetFastJetFinderTool("fastfinder5")
    fastfinder5.Algorithm           = "anti-kt"
    fastfinder5.Radius              = 0.2
    fastfinder5.RecombScheme        = "E"
    fastfinder5.Strategy            = "Best"
    fastfinder5.FailIfMisconfigured = True
    fastfinder5.Inclusive           = True
    fastfinder5.CalculateJetArea    = False
    fastfinder5.StoreNFlipValues    = 2
    ToolSvc += fastfinder5

    fastfinder6 = JetFastJetFinderTool("fastfinder6")
    fastfinder6.Algorithm           = "anti-kt"
    fastfinder6.Radius              = 0.3
    fastfinder6.RecombScheme        = "E"
    fastfinder6.Strategy            = "Best"
    fastfinder6.FailIfMisconfigured = True
    fastfinder6.Inclusive           = True
    fastfinder6.CalculateJetArea    = False
    fastfinder6.StoreNFlipValues    = 2
    ToolSvc += fastfinder6

    #fastfinder7 = JetFastJetFinderTool("fastfinder7")
    #fastfinder7.Algorithm           = "anti-kt"
    #fastfinder7.Radius              = 0.8
    #fastfinder7.RecombScheme        = "E"
    #fastfinder7.Strategy            = "Best"
    #fastfinder7.FailIfMisconfigured = True
    #fastfinder7.Inclusive           = True
    #fastfinder7.CalculateJetArea    = False
    #fastfinder7.StoreNFlipValues    = 2
    #ToolSvc += fastfinder7

    fastfinder8 = JetFastJetFinderTool("fastfinder8")
    fastfinder8.Algorithm           = "kt"
    fastfinder8.Radius              = 0.3
    fastfinder8.RecombScheme        = "E"
    fastfinder8.Strategy            = "Best"
    fastfinder8.FailIfMisconfigured = True
    fastfinder8.Inclusive           = True
    fastfinder8.CalculateJetArea    = False
    fastfinder8.StoreNFlipValues    = 2
    ToolSvc += fastfinder8

    # Tell "TrackZClusterTool" to use this tool
    MyJetTrackZClusterTool1.JetFinder = fastfinder1
    #MyJetTrackZClusterTool2.JetFinder = fastfinder2
    #MyJetTrackZClusterTool3.JetFinder = fastfinder3
    #MyJetTrackZClusterTool4.JetFinder = fastfinder4
    MyJetTrackZClusterTool5.JetFinder = fastfinder5
    MyJetTrackZClusterTool6.JetFinder = fastfinder6
    #MyJetTrackZClusterTool7.JetFinder = fastfinder7
    MyJetTrackZClusterTool8.JetFinder = fastfinder8

    from JetRec.JetGetters import *
    TrackJetAlg1=make_StandardJetGetter('AntiKt', 0.4, 'Track', outputCollectionName='AntiKt4TrackJets', allTools=[MyJetTrackZClusterTool1,JetSorterTool()] ).jetAlgorithmHandle()
    #TrackJetAlg2=make_StandardJetGetter('AntiKt', 0.6, 'Track', outputCollectionName='AntiKt6TrackJets', allTools=[MyJetTrackZClusterTool2,JetSorterTool()] ).jetAlgorithmHandle()
    #TrackJetAlg3=make_StandardJetGetter('AntiKt', 0.5, 'Track', outputCollectionName='AntiKt5TrackJets', allTools=[MyJetTrackZClusterTool3,JetSorterTool()] ).jetAlgorithmHandle()
    #TrackJetAlg4=make_StandardJetGetter('AntiKt', 0.7, 'Track', outputCollectionName='AntiKt7TrackJets', allTools=[MyJetTrackZClusterTool4,JetSorterTool()] ).jetAlgorithmHandle()
    TrackJetAlg5=make_StandardJetGetter('AntiKt', 0.2, 'Track', outputCollectionName='AntiKt2TrackJets', allTools=[MyJetTrackZClusterTool5,JetSorterTool()] ).jetAlgorithmHandle()
    TrackJetAlg6=make_StandardJetGetter('AntiKt', 0.3, 'Track', outputCollectionName='AntiKt3TrackJets', allTools=[MyJetTrackZClusterTool6,JetSorterTool()] ).jetAlgorithmHandle()
    #TrackJetAlg7=make_StandardJetGetter('AntiKt', 0.8, 'Track', outputCollectionName='AntiKt8TrackJets', allTools=[MyJetTrackZClusterTool7,JetSorterTool()] ).jetAlgorithmHandle()
    TrackJetAlg8=make_StandardJetGetter('Kt', 0.3, 'Track', outputCollectionName='Kt3TrackJets', allTools=[MyJetTrackZClusterTool8,JetSorterTool()] ).jetAlgorithmHandle()

    from JetRec.JetMomentGetter import  add_jetMoments_to_JetAlg
    from JetMomentTools.JetMomentToolsConf import JetWidthTool
    widthtool = JetWidthTool('JS_JetWidthTool')
    add_jetMoments_to_JetAlg(TrackJetAlg1, [widthtool])
    #add_jetMoments_to_JetAlg(TrackJetAlg2, [widthtool])
    #add_jetMoments_to_JetAlg(TrackJetAlg3, [widthtool])
    #add_jetMoments_to_JetAlg(TrackJetAlg4, [widthtool])
    add_jetMoments_to_JetAlg(TrackJetAlg5, [widthtool])
    add_jetMoments_to_JetAlg(TrackJetAlg6, [widthtool])
    #add_jetMoments_to_JetAlg(TrackJetAlg7, [widthtool])
    add_jetMoments_to_JetAlg(TrackJetAlg8, [widthtool])
    

    #==============================================================
    #
    # End of job options file
    #
    ###############################################################
