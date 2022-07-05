#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def DQTDataFlowMonAlgConfig(flags):
    from AthenaMonitoring import AthMonitorCfgHelper
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaMonitoring.AtlasReadyFilterConfig import AtlasReadyFilterCfg
    ReadyFilterCfg=AtlasReadyFilterCfg(flags)
    helper = AthMonitorCfgHelper(flags, 'DQTDataFlowMonAlgCfg')
    _DQTDataFlowMonAlgConfigCore(helper, CompFactory.DQTDataFlowMonAlg, flags.Input.isMC, ReadyFilterCfg.popPrivateTools())
    helper.result().merge(ReadyFilterCfg)
    return helper.result()

def DQTDataFlowMonAlgConfigOld(flags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    from .DataQualityToolsConf import DQTDataFlowMonAlg
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool
    helper = AthMonitorCfgHelperOld(flags, 'DQTDataFlowMonAlgCfg')
    _DQTDataFlowMonAlgConfigCore(helper, DQTDataFlowMonAlg,
                                 globalflags.DataSource() == 'geant4',
                                 GetAtlasReadyFilterTool())
    return helper.result()

def _DQTDataFlowMonAlgConfigCore(helper, algConfObj, isMC, readyFilterTool):
    from ROOT import EventInfo

    monAlg = helper.addAlgorithm(algConfObj,'DQTDataFlowMonAlg')

    # arguments are: algorithm, name of group used to access it from the alg,
    # the 'top level path' to put outputs in, and the default duration of
    # associated histograms
    group = helper.addGroup(monAlg, 'default', 'GLOBAL/DQTDataFlow', 'lowStat')

    # only make this plot if MC
    if isMC:
        group.defineHistogram("LB;m_sumweights",
                              weight='mcweight',
                              title="Sum of MC event weights",
                              xbins=50,
                              xmin=0.5,
                              xmax=50.5,
                              opt='kAddBinsDynamically',
                              duration='run',
                              merge='merge',
        )

    group.defineHistogram("detstates_idx,detstates;eventflag_summary_lowStat",
                          title="Event Flag Summary",
                          type='TH2I',
                          xbins=EventInfo.nDets+1,
                          xmin=-0.5,
                          xmax=EventInfo.nDets+0.5,
                          ybins=3,
                          ymin=-0.5,
                          ymax=2.5,
                          xlabels=["Pixel", "SCT", "TRT", "LAr", "Tile",
                                   "Muon", "ForwardDet", "Core",
                                   "Background", "Lumi", "All"],
                          ylabels=["OK", "Warning", "Error"]
    )

    if not isMC:
        monAlg.ReadyFilterTool = readyFilterTool
        group.defineHistogram('atlasready',
                              title='Events with AtlasReady',
                              type='TH1I',
                              xbins=2,
                              xlabels=["Not Ready","Ready"],
                              xmin=-0.5,
                              xmax=1.5,
                          )

    # generate release string
    import os
    if 'AtlasProject' in os.environ and 'AtlasVersion' in os.environ:
        releaseString = '%s-%s' % (os.environ['AtlasProject'],
                                   os.environ['AtlasVersion'])
    else:
        releaseString = 'Unknown'
        
    group.defineHistogram("environment;m_release_stage_lowStat",
                          title="Release - Event Count",
                          type='TH1I',
                          xbins=7,
                          xmin=-0.5,
                          xmax=6.5,
                          xlabels=[_+'-'+releaseString for _ in
                                   ["user", "online", "tier0", "tier0Raw",
                                    "tier0ESD", "AOD", "altprod"]]
    )
    
    group.defineHistogram("environment;events_lb",
                          title='Event Count',
                          type='TH1I',
                          xbins=7,
                          xmin=-0.5,
                          xmax=6.5,
                          xlabels=["user", "online", "tier0", "tier0Raw",
                                   "tier0ESD", "AOD", "altprod"],
                          duration='lb',
    )
