#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file InDetPhysValMonitoringConfig.py
@author T. Strebler
@date 2021-08-30
@brief Main CA-based python configuration for InDetPhysValMonitoring
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def HistogramDefinitionSvcCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    if flags.Detector.GeometryID:
        kwargs.setdefault("DefinitionSource", "InDetPVMPlotDefRun2.xml")
    elif flags.Detector.GeometryITk:
        kwargs.setdefault("DefinitionSource", "InDetPVMPlotDefITK.xml")
    kwargs.setdefault("DefinitionFormat", "text/xml")
    histoSvc = CompFactory.HistogramDefinitionSvc(**kwargs)
    acc.addService(histoSvc)
    return acc

def InDetRttTruthSelectionToolCfg(flags, name="InDetRttTruthSelectionTool", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("requireStatus1", True)
    kwargs.setdefault("requireCharged", True)
    kwargs.setdefault("maxBarcode", (200*1000 if kwargs.pop("OnlyDressPrimaryTracks", True) else 2**31-1))
    kwargs.setdefault("maxProdVertRadius", 300.)
    if flags.Detector.GeometryITk:
        kwargs.setdefault("maxEta", 4.)
    else:
        kwargs.setdefault("maxEta", 2.5)
    kwargs.setdefault("minPt", flags.PhysVal.IDPVM.truthMinPt)

    if "radiusCylinder" in kwargs or "zDisc" in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?
        kwargs.setdefault("Extrapolator", extrapolator)
    else:
        kwargs.setdefault("Extrapolator", None)

    tool = CompFactory.AthTruthSelectionTool(name = name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def HardScatterSelectionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    tool = CompFactory.InDet.InDetHardScatterSelectionTool(name = "InDetHardScatterSelectionTool", **kwargs)
    acc.setPrivateTools(tool)
    return acc

def GoodRunsListSelectionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    cvmfs = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/'
    grldict = {
        '2015': cvmfs + 'data15_13TeV/20170619/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml',
        '2016': cvmfs + 'data16_13TeV/20180129/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml',
        '2017': cvmfs + 'data17_13TeV/20180619/data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
        '2018': cvmfs + 'data18_13TeV/20190318/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
    }

    tool = CompFactory.GoodRunsListSelectionTool(
        name="GoodRunsListSelectionTool",
        GoodRunsListVec=[grldict[p] for p in flags.IDPVM.GRL],
        **kwargs
    )
    acc.setPrivateTools(tool)
    return acc

def InDetVertexTruthMatchToolCfg(flags, **kwargs):
    acc  = ComponentAccumulator()
    tool = CompFactory.InDetVertexTruthMatchTool(**kwargs)
    acc.setPrivateTools(tool)
    return acc

def LRTTrackParticleMergerCfg(flags, name="MergeLRTAndStandard", **kwargs):
    kwargs.setdefault("TrackParticleLocation", ["InDetTrackParticles","InDetLargeD0TrackParticles"])
    kwargs.setdefault("OutputTrackParticleLocation", "InDetWithLRTTrackParticles")
    kwargs.setdefault("CreateViewColllection", True)
    from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleMergerCfg
    return TrackParticleMergerCfg(flags, name, **kwargs)

def LRTMergerCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    tool = acc.getPrimaryAndMerge(LRTTrackParticleMergerCfg(flags))
    LRTMergeAug = CompFactory.DerivationFramework.CommonAugmentation("InDetLRTMerge", AugmentationTools = tool)
    acc.addEventAlgo(LRTMergeAug)
    return acc

def InDetPhysValMonitoringToolCfg(flags, **kwargs):
    from InDetPhysValMonitoring.InDetPhysValDecorationConfig import AddDecoratorIfNeededCfg
    acc = AddDecoratorIfNeededCfg(flags)
    kwargs.setdefault("useTrackSelection", False)
    kwargs.setdefault("EnableLumi", False)

    acc.merge(HistogramDefinitionSvcCfg(flags))

    jets_name_for_HardScatter = 'AntiKt4EMTopoJets'
    # if we are running with sumpT(w) hard scatter selection, we need to schedule jet finding
    if flags.PhysVal.IDPVM.hardScatterStrategy == 2:
        
        from InDetPhysValMonitoring.addRecoJetsConfig import AddRecoJetsIfNotExistingCfg
        acc.merge(AddRecoJetsIfNotExistingCfg(flags, jets_name_for_HardScatter))

    if flags.PhysVal.IDPVM.GRL:
        grlTool = acc.popToolsAndMerge(GoodRunsListSelectionToolCfg(flags))
        kwargs.setdefault("useGRL", True)
        kwargs.setdefault('GoodRunsListSelectionTool', grlTool)

    if flags.Input.isMC:
        kwargs.setdefault("TruthParticleContainerName", "TruthParticles")
        if 'TruthSelectionTool' not in kwargs:
            TruthSelectionTool = acc.popToolsAndMerge(InDetRttTruthSelectionToolCfg(flags))
            kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)

        if 'hardScatterSelectionTool' not in kwargs:
            hardScatterSelectionTool = acc.popToolsAndMerge(HardScatterSelectionToolCfg(flags))
            hardScatterSelectionTool.RedoHardScatter=True
            # for sumpt(w), make sure the HS selection tool picks up the correct jets
            if flags.PhysVal.IDPVM.hardScatterStrategy == 2:
                hardScatterSelectionTool.JetContainer = jets_name_for_HardScatter
            hardScatterSelectionTool.SelectionMode = flags.PhysVal.IDPVM.hardScatterStrategy
            kwargs.setdefault("hardScatterSelectionTool", hardScatterSelectionTool)

        if flags.PhysVal.IDPVM.doValidateTracksInJets:
            jets_name = 'AntiKt4TruthJets'
            kwargs.setdefault("JetContainerName", jets_name)
            kwargs.setdefault("FillTrackInJetPlots", True)            

            from InDetPhysValMonitoring.addTruthJetsConfig import AddTruthJetsIfNotExistingCfg
            acc.merge(AddTruthJetsIfNotExistingCfg(flags))

            if flags.PhysVal.IDPVM.doValidateTracksInBJets:
                kwargs.setdefault("FillTrackInBJetPlots", True)

        else:
            kwargs.setdefault("JetContainerName",'')
            kwargs.setdefault("FillTrackInJetPlots", False)

        kwargs.setdefault("FillTruthToRecoNtuple", flags.PhysVal.IDPVM.doValidateTruthToRecoNtuple)
        kwargs.setdefault("doTruthOriginPlots",    flags.PhysVal.IDPVM.doTruthOriginPlots)
        kwargs.setdefault("doPerAuthorPlots",      flags.PhysVal.IDPVM.doPerAuthorPlots)
        kwargs.setdefault("doHitLevelPlots",       flags.PhysVal.IDPVM.doHitLevelPlots)

        # adding the VertexTruthMatchingTool
        VertexTruthMatchTool = acc.popToolsAndMerge(InDetVertexTruthMatchToolCfg(flags))
        kwargs.setdefault("useVertexTruthMatchTool", True)
        kwargs.setdefault("VertexTruthMatchTool", VertexTruthMatchTool)

        # Options for Truth Strategy : Requires full pile-up truth containers for some
        if flags.PhysVal.IDPVM.setTruthStrategy == 'All' or flags.PhysVal.IDPVM.setTruthStrategy == 'PileUp':
            if "xAOD::TruthPileupEventContainer#TruthPileupEvents" in flags.Input.TypedCollections:
                kwargs.setdefault("PileupSwitch", flags.PhysVal.IDPVM.setTruthStrategy)
            else:
                print('WARNING Truth Strategy for InDetPhysValMonitoring set to %s but TruthPileupEvents are missing in the input; resetting to HardScatter only' % (
                    flags.PhysVal.IDPVM.setTruthStrategy))
        elif flags.PhysVal.IDPVM.setTruthStrategy != 'HardScatter':
            print('WARNING Truth Strategy for for InDetPhysValMonitoring set to invalid option %s; valid flags are ["HardScatter", "All", "PileUp"]' % (
                flags.PhysVal.IDPVM.setTruthStrategy))

    else:
        # disable truth monitoring for data
        kwargs.setdefault("TruthParticleContainerName", '')
        kwargs.setdefault("TruthVertexContainerName", '')
        kwargs.setdefault("TruthEvents", '')
        kwargs.setdefault("TruthPileupEvents", '')
        kwargs.setdefault("TruthSelectionTool", None)        
        # the jet container is actually meant to be a truth jet container
        kwargs.setdefault("JetContainerName", '')
        kwargs.setdefault("FillTrackInJetPlots", False)
        kwargs.setdefault("FillTrackInBJetPlots", False)
        kwargs.setdefault("FillTruthToRecoNtuple", False)

    if flags.Detector.GeometryITk:
        #Disable vertex container for now
        kwargs.setdefault("doTRTExtensionPlots", False)

    # Control the number of output histograms
    if flags.PhysVal.IDPVM.doPhysValOutput:
        kwargs.setdefault("DetailLevel", 100)

    elif flags.PhysVal.IDPVM.doExpertOutput:
        kwargs.setdefault("DetailLevel", 200)

    tool = CompFactory.InDetPhysValMonitoringTool(**kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetPhysValMonitoringToolLooseCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if 'TrackSelectionTool' not in kwargs:
        from InDetConfig.InDetTrackSelectionToolConfig import InDetTrackSelectionTool_Loose_Cfg
        TrackSelectionTool = acc.popToolsAndMerge(InDetTrackSelectionTool_Loose_Cfg(flags))
        kwargs.setdefault("TrackSelectionTool", TrackSelectionTool)

    kwargs.setdefault("SubFolder", 'Loose/')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name="InDetPhysValMonitoringToolLoose", **kwargs)

def InDetPhysValMonitoringToolTightPrimaryCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if 'TrackSelectionTool' not in kwargs:
        from InDetConfig.InDetTrackSelectionToolConfig import InDetTrackSelectionTool_TightPrimary_Cfg
        TrackSelectionTool = acc.popToolsAndMerge(InDetTrackSelectionTool_TightPrimary_Cfg(flags))
        kwargs.setdefault("TrackSelectionTool", TrackSelectionTool)

    kwargs.setdefault("SubFolder", 'TightPrimary/')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name="InDetPhysValMonitoringToolTightPrimary", **kwargs)

def InDetPhysValMonitoringToolGSFCfg(flags, **kwargs):
    kwargs.setdefault("SubFolder", 'GSF/')
    kwargs.setdefault("TrackParticleContainerName", 'GSFTrackParticles')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name="InDetPhysValMonitoringToolGSF", **kwargs)

def InDetPhysValMonitoringToolElectronsCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    TruthSelectionTool = acc.popToolsAndMerge(InDetRttTruthSelectionToolCfg(flags, name="AthTruthSelectionToolForIDPVM_Electrons", **kwargs))
    TruthSelectionTool.pdgId = 11
    TruthSelectionTool.minPt = 5000.

    kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)
    kwargs.setdefault("onlyFillTruthMatched", True)
    kwargs.setdefault("SubFolder", 'Electrons/')

    return InDetPhysValMonitoringToolCfg(flags, name='InDetPhysValMonitoringToolElectrons', **kwargs)

def InDetPhysValMonitoringToolMuonsCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    TruthSelectionTool = acc.popToolsAndMerge(InDetRttTruthSelectionToolCfg(flags, name = "AthTruthSelectionToolForIDPVM_Muons", **kwargs))
    TruthSelectionTool.pdgId = 13
    TruthSelectionTool.minPt = 5000.

    kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)
    kwargs.setdefault("onlyFillTruthMatched", True)
    kwargs.setdefault("SubFolder", 'Muons/')

    return InDetPhysValMonitoringToolCfg(flags, name='InDetPhysValMonitoringToolMuons', **kwargs)

def InDetPhysValMonitoringToolDBMCfg(flags, **kwargs):
    
    kwargs.setdefault("SubFolder", 'DBM/')
    kwargs.setdefault("TrackParticleContainerName", 'ResolvedDBMTracks')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name='InDetPhysValMonitoringToolDBM', **kwargs)

def InDetLargeD0PhysValMonitoringToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    TruthSelectionTool = acc.popToolsAndMerge(InDetRttTruthSelectionToolCfg(flags, name = "AthTruthSelectionToolForIDPVM_LargeD0", **kwargs))
    TruthSelectionTool.maxProdVertRadius = 400.
    TruthSelectionTool.minPt = 1200.
    TruthSelectionTool.ancestorList = flags.PhysVal.IDPVM.ancestorIDs

    kwargs.setdefault("SubFolder", 'LRT/')
    kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)
    if flags.Detector.GeometryID:
        kwargs.setdefault("TrackParticleContainerName", 'InDetLargeD0TrackParticles' if flags.InDet.Tracking.storeSeparateLargeD0Container else 'InDetTrackParticles')
    elif flags.Detector.GeometryITk:
        kwargs.setdefault("TrackParticleContainerName", 'InDetLargeD0TrackParticles' if flags.ITk.Tracking.storeSeparateLargeD0Container else 'InDetTrackParticles')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name='InDetPhysValMonitoringToolLargeD0', **kwargs)

def InDetMergedLargeD0PhysValMonitoringToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    TruthSelectionTool = acc.popToolsAndMerge(InDetRttTruthSelectionToolCfg(flags, name = "AthTruthSelectionToolForIDPVM_MergedLargeD0", **kwargs))
    TruthSelectionTool.maxProdVertRadius = 400.
    TruthSelectionTool.minPt = 1200.
    TruthSelectionTool.ancestorList = flags.PhysVal.IDPVM.ancestorIDs

    kwargs.setdefault("SubFolder", 'LRTMerged/')
    kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)
    if flags.Detector.GeometryID:
        kwargs.setdefault("TrackParticleContainerName", 'InDetWithLRTTrackParticles' if flags.InDet.Tracking.storeSeparateLargeD0Container else 'InDetTrackParticles')
    elif flags.Detector.GeometryITk:
        kwargs.setdefault("TrackParticleContainerName", 'InDetWithLRTTrackParticles' if flags.ITk.Tracking.storeSeparateLargeD0Container else 'InDetTrackParticles')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name='InDetPhysValMonitoringToolMergedLargeD0', **kwargs)

def InDetPhysValMonitoringCfg(flags):
    acc = ComponentAccumulator()

    #from InDetPhysValMonitoring.InDetPhysValDecorationConfig import AddDecoratorIfNeededCfg
    #acc.merge(AddDecoratorIfNeededCfg(flags))

    if flags.PhysVal.IDPVM.doValidateMergedLargeD0Tracks:
        acc.merge(LRTMergerCfg(flags))

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1

    mons = [ (True                                       ,  InDetPhysValMonitoringToolCfg ),
             (flags.PhysVal.IDPVM.doValidateMuonMatchedTracks    ,  InDetPhysValMonitoringToolMuonsCfg ),
             (flags.PhysVal.IDPVM.doValidateElectronMatchedTracks,  InDetPhysValMonitoringToolElectronsCfg ),
             (flags.PhysVal.IDPVM.doValidateLargeD0Tracks        ,  InDetLargeD0PhysValMonitoringToolCfg ),
             (flags.PhysVal.IDPVM.doValidateMergedLargeD0Tracks  ,  InDetMergedLargeD0PhysValMonitoringToolCfg),
             (flags.PhysVal.IDPVM.doValidateLooseTracks          ,  InDetPhysValMonitoringToolLooseCfg ),
             (flags.PhysVal.IDPVM.doValidateTightPrimaryTracks   ,  InDetPhysValMonitoringToolTightPrimaryCfg ),
             (flags.PhysVal.IDPVM.doValidateDBMTracks            ,  InDetPhysValMonitoringToolDBMCfg ),
             (flags.PhysVal.IDPVM.doValidateGSFTracks            ,  InDetPhysValMonitoringToolGSFCfg )
    ]

    for enabled, creator in mons :
        if enabled :
            monMan.AthenaMonTools += [ acc.popToolsAndMerge(creator(flags)) ]

    from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
    for col in flags.PhysVal.IDPVM.validateExtraTrackCollections :
        prefix=extractCollectionPrefix(col)
        tool = acc.popToolsAndMerge(InDetPhysValMonitoringToolCfg(flags, name = 'InDetPhysValMonitoringTool'+prefix))
        tool.SubFolder = prefix+'Tracks/'
        tool.TrackParticleContainerName = prefix+'TrackParticles'

        monMan.AthenaMonTools += [ tool ]
        
    acc.addEventAlgo(monMan, primary = True)
    return acc
             
    
