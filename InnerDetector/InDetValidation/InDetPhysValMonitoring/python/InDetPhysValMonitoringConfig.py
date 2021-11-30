#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    kwargs.setdefault("minPt", 500.)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", extrapolator)

    tool = CompFactory.AthTruthSelectionTool(name = name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def HardScatterSelectionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    tool = CompFactory.InDet.InDetHardScatterSelectionTool(name = "InDetHardScatterSelectionTool", **kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetVertexTruthMatchToolCfg(flags, **kwargs):
    acc  = ComponentAccumulator()
    tool = CompFactory.InDetVertexTruthMatchTool(**kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetPhysValMonitoringToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("useTrackSelection", False)
    kwargs.setdefault("EnableLumi", False)

    acc.merge(HistogramDefinitionSvcCfg(flags))

    if flags.Input.isMC:
        kwargs.setdefault("TruthParticleContainerName", "TruthParticles")
        if 'TruthSelectionTool' not in kwargs:
            TruthSelectionTool = acc.popToolsAndMerge(InDetRttTruthSelectionToolCfg(flags))
            kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)

        if 'hardScatterSelectionTool' not in kwargs:
            hardScatterSelectionTool = acc.popToolsAndMerge(HardScatterSelectionToolCfg(flags))
            hardScatterSelectionTool.RedoHardScatter=True
            hardScatterSelectionTool.SelectionMode = flags.IDPVM.hardScatterStrategy
            kwargs.setdefault("hardScatterSelectionTool", hardScatterSelectionTool)

        if flags.IDPVM.doValidateTracksInJets:
            jets_name = 'AntiKt4TruthJets'
            kwargs.setdefault("JetContainerName", jets_name)
            kwargs.setdefault("FillTrackInJetPlots", True)            

            from InDetPhysValMonitoring.addTruthJetsConfig import AddTruthJetsIfNotExistingCfg
            acc.merge(AddTruthJetsIfNotExistingCfg(flags))

            if flags.IDPVM.doValidateTracksInBJets:
                kwargs.setdefault("FillTrackInBJetPlots", True)

        else:
            kwargs.setdefault("JetContainerName",'')
            kwargs.setdefault("FillTrackInJetPlots", False)

        kwargs.setdefault("FillTruthToRecoNtuple", flags.IDPVM.doValidateTruthToRecoNtuple)
        kwargs.setdefault("doTruthOriginPlots",    flags.IDPVM.doTruthOriginPlots)
        kwargs.setdefault("doPerAuthorPlots",      flags.IDPVM.doPerAuthorPlots)
        kwargs.setdefault("doHitLevelPlots",       flags.IDPVM.doHitLevelPlots)

        # adding the VeretxTruthMatchingTool
        VertexTruthMatchTool = acc.popToolsAndMerge(InDetVertexTruthMatchToolCfg(flags))
        kwargs.setdefault("useVertexTruthMatchTool", True)
        kwargs.setdefault("VertexTruthMatchTool", VertexTruthMatchTool)

        # Options for Truth Strategy : Requires full pile-up truth containers for some
        if flags.IDPVM.setTruthStrategy == 'All' or flags.IDPVM.setTruthStrategy == 'PileUp':
            if "xAOD::TruthPileupEventContainer#TruthPileupEvents" in flags.Input.TypedCollections:
                kwargs.setdefault("PileupSwitch", flags.IDPVM.setTruthStrategy)
            else:
                print('WARNING Truth Strategy for InDetPhysValMonitoring set to %s but TruthPileupEvents are missing in the input; resetting to HardScatter only' % (
                    flags.IDPVM.setTruthStrategy))
        elif flags.IDPVM.setTruthStrategy != 'HardScatter':
            print('WARNING Truth Strategy for for InDetPhysValMonitoring set to invalid option %s; valid flags are ["HardScatter", "All", "PileUp"]' % (
                flags.IDPVM.setTruthStrategy))

    else:
        # disable truth monitoring for data
        kwargs.setdefault("TruthParticleContainerName", '')
        kwargs.setdefault("TruthVertexContainerName", '')
        kwargs.setdefault("TruthEvents", '')
        kwargs.setdefault("TruthPileupEvents" '')
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
    if flags.IDPVM.doPhysValOutput:
        kwargs.setdefault("DetailLevel", 100)

    elif flags.IDPVM.doExpertOutput:
        kwargs.setdefault("DetailLevel", 200)

    tool = CompFactory.InDetPhysValMonitoringTool(**kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetTrackSelectionToolLooseCfg(flags, name="InDetTrackSelectionToolLoose",
                                    **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CutLevel", "Loose")
    tool = CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetTrackSelectionToolTightPrimaryCfg(flags, name="InDetTrackSelectionToolTightPrimary",
                                           **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CutLevel", "TightPrimary")
    tool = CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetPhysValMonitoringToolLooseCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if 'TrackSelectionTool' not in kwargs:
        TrackSelectionTool = acc.popToolsAndMerge(InDetTrackSelectionToolLooseCfg(flags))
        kwargs.setdefault("TrackSelectionTool", TrackSelectionTool)

    kwargs.setdefault("SubFolder", 'Loose/')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name="InDetPhysValMonitoringToolLoose", **kwargs)

def InDetPhysValMonitoringToolTightPrimaryCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if 'TrackSelectionTool' not in kwargs:
        TrackSelectionTool = acc.popToolsAndMerge(InDetTrackSelectionToolTightPrimaryCfg(flags))
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
    TruthSelectionTool.ancestorList = flags.IDPVM.ancestorIDs

    kwargs.setdefault("SubFolder", 'LRT/')
    kwargs.setdefault("TruthSelectionTool", TruthSelectionTool)
    if flags.Detector.GeometryID:
        kwargs.setdefault("TrackParticleContainerName", 'InDetLargeD0TrackParticles' if flags.InDet.storeSeparateLargeD0Container else 'InDetTrackParticles')
    elif flags.Detector.GeometryITk:
        kwargs.setdefault("TrackParticleContainerName", 'InDetLargeD0TrackParticles' if flags.ITk.storeSeparateLargeD0Container else 'InDetTrackParticles')
    kwargs.setdefault("useTrackSelection", True)

    return InDetPhysValMonitoringToolCfg(flags, name='InDetPhysValMonitoringToolLargeD0', **kwargs)

def InDetPhysValMonitoringCfg(flags):
    acc = ComponentAccumulator()

    from InDetPhysValMonitoring.InDetPhysValDecorationConfig import AddDecoratorIfNeededCfg
    acc.merge(AddDecoratorIfNeededCfg(flags))

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1

    mons = [ (True                                       ,  InDetPhysValMonitoringToolCfg ),
             (flags.IDPVM.doValidateMuonMatchedTracks    ,  InDetPhysValMonitoringToolMuonsCfg ),
             (flags.IDPVM.doValidateElectronMatchedTracks,  InDetPhysValMonitoringToolElectronsCfg ),
             (flags.IDPVM.doValidateLargeD0Tracks        ,  InDetLargeD0PhysValMonitoringToolCfg ),
             (flags.IDPVM.doValidateLooseTracks          ,  InDetPhysValMonitoringToolLooseCfg ),
             (flags.IDPVM.doValidateTightPrimaryTracks   ,  InDetPhysValMonitoringToolTightPrimaryCfg ),
             (flags.IDPVM.doValidateDBMTracks            ,  InDetPhysValMonitoringToolDBMCfg ),
             (flags.IDPVM.doValidateGSFTracks            ,  InDetPhysValMonitoringToolGSFCfg )
    ]

    for enabled, creator in mons :
        if enabled :
            monMan.AthenaMonTools += [ acc.popToolsAndMerge(creator(flags)) ]

    from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
    for col in flags.IDPVM.validateExtraTrackCollections :
        prefix=extractCollectionPrefix(col)
        tool = acc.popToolsAndMerge(InDetPhysValMonitoringToolCfg(flags, name = 'InDetPhysValMonitoringTool'+prefix))
        tool.SubFolder = prefix+'Tracks/'
        tool.TrackParticleContainerName = prefix+'TrackParticles'

        monMan.AthenaMonTools += [ tool ]
        
    acc.addEventAlgo(monMan, primary = True)
    return acc
             
    
