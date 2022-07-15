# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ActsTrkPixelClusterAnalysisCfg(flags, name="ActsTrkPixelClusterAnalysis", **kwargs):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result = ITkPixelReadoutGeometryCfg(flags)
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'ActsTrkClusterAnalysisCfg')

    kwargs.setdefault("ClusterContainerKey", "ITkPixelClusters")
    monitoringAlgorithm = helper.addAlgorithm(CompFactory.ActsTrk.PixelClusterAnalysis, name, **kwargs)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsTrkClusterAnalysis', '/ActsTrkAnalysis/')

    monitoringGroup.defineHistogram('globalZ,perp;h_globalZR', title="h_globalZR; z [mm]; r [mm]", type="TH2F", path="PixelClusters",
                                    xbins=1500, xmin=-3000, xmax=3000,
                                    ybins=400, ymin=0, ymax=400)
    monitoringGroup.defineHistogram('eta;h_etaCluster', title="h_etaCluster; cluster #eta", type="TH1F", path="PixelClusters",
                                    xbins=100, xmin=-5, xmax=5)
    
    monitoringGroup.defineTree('barrelEndcap,layerDisk,phiModule,etaModule,isInnermost,isNextToInnermost,eta,globalX,globalY,globalZ,perp,localX,localY,localCovXX,localCovYY,sizeX,sizeY;PixelClusters',
                               path='ntuples',
                               treedef='barrelEndcap/vector<int>:layerDisk/vector<int>:phiModule/vector<int>:etaModule/vector<int>:isInnermost/vector<int>:isNextToInnermost/vector<int>:eta/vector<double>:globalX/vector<float>:globalY/vector<float>:globalZ/vector<float>:perp/vector<float>:localX/vector<float>:localY/vector<float>:localCovXX/vector<float>:localCovYY/vector<float>:sizeX/vector<int>:sizeY/vector<int>')

    result.merge(helper.result())
    return result


def ActsTrkStripClusterAnalysisCfg(flags, name="ActsTrkStripClusterAnalysis", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result = ITkStripReadoutGeometryCfg(flags)
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'ActsTrkClusterAnalysisCfg')

    kwargs.setdefault("ClusterContainerKey", "ITkStripClusters")
    monitoringAlgorithm = helper.addAlgorithm(CompFactory.ActsTrk.StripClusterAnalysis, name, **kwargs)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsTrkClusterAnalysis', '/ActsTrkAnalysis/')

    monitoringGroup.defineHistogram('globalZ,perp;h_globalZR', title="h_globalZR; z [mm]; r [mm]", type="TH2F", path="StripClusters",
                                    xbins=1500, xmin=-3000, xmax=3000,
                                    ybins=400, ymin=300, ymax=1100)    
    monitoringGroup.defineHistogram('eta;h_etaCluster', title="h_etaCluster; cluster #eta", type="TH1F", path="StripClusters",
                                    xbins=100, xmin=-5, xmax=5)

    monitoringGroup.defineTree('barrelEndcap,layerDisk,phiModule,etaModule,sideModule,eta,globalX,globalY,globalZ,perp,localX,localCovXX,sizeX;StripClusters', 
                               path='ntuples', 
                               treedef='barrelEndcap/vector<int>:layerDisk/vector<int>:phiModule/vector<int>:etaModule/vector<int>:sideModule/vector<int>:eta/vector<double>:globalX/vector<float>:globalY/vector<float>:globalZ/vector<float>:perp/vector<float>:localX/vector<float>:localCovXX/vector<float>:sizeX/vector<int>')

    result.merge(helper.result())
    return result

def ActsTrkBaseSpacePointAnalysisCfg(flags,
                                     name,
                                     histoPath,
                                     ntupleName,
                                     **kwargs):
    isPixel = 'Pixel' in name
    perp_min = 0 if isPixel else 300
    perp_max = 400 if isPixel else 1100

    acc = ComponentAccumulator()
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'ActsTrkSpacePointAnalysisCfg')
    
    monitoringAlgorithm = helper.addAlgorithm(CompFactory.ActsTrk.SpacePointAnalysis, name, **kwargs)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsTrkSpacePointAnalysis', '/ActsTrkAnalysis/')
    

    monitoringGroup.defineHistogram('Nsp;h_Nsp', title="Number of Space Points;N;Entries", type="TH1I", path=f"{histoPath}",
                                    xbins=100, xmin=0, xmax=0)

    monitoringGroup.defineHistogram('globalX,globalY;h_globalXY', title="h_globalXY; x [mm]; y [mm]", type="TH2F", path=f"{histoPath}",
                                    xbins=800, xmin=-perp_max, xmax=perp_max,
                                    ybins=800, ymin=-perp_max, ymax=perp_max)
    monitoringGroup.defineHistogram('globalZ,perp;h_globalZR', title="h_globalZR; z [mm]; r [mm]", type="TH2F", path=f"{histoPath}",
                                    xbins=1500, xmin=-3000, xmax=3000,
                                    ybins=400, ymin=perp_min, ymax=perp_max)
    monitoringGroup.defineHistogram('eta;h_etaSpacePoint', title="h_etaSpacePoint; space point #eta", type="TH1F", path=f"{histoPath}",
                                    xbins=100, xmin=-5, xmax=5)

    monitoringGroup.defineTree(f'barrelEndcap,layerDisk,phiModule,etaModule,sideModule,isInnermost,isNextToInnermost,isOverlap,eta,globalX,globalY,globalZ,perp,globalCovR,globalCovZ;{ntupleName}',
                               path='ntuples',
                               treedef='barrelEndcap/vector<int>:layerDisk/vector<int>:phiModule/vector<int>:etaModule/vector<int>:sideModule/vector<int>:isInnermost/vector<int>:isNextToInnermost/vector<int>:isOverlap/vector<int>:eta/vector<double>:globalX/vector<double>:globalY/vector<double>:globalZ/vector<double>:perp/vector<double>:globalCovR/vector<double>:globalCovZ/vector<double>')

    acc.merge(helper.result())
    return acc

def ActsTrkPixelSpacePointAnalysisCfg(flags, name="ActsTrkPixelSpacePointAnalysis", **kwargs):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result = ITkPixelReadoutGeometryCfg(flags)

    kwargs.setdefault("SpacePointContainerKey", "ITkPixelSpacePoints")
    kwargs.setdefault("PixelClusterContainerKey", "ITkPixelClusters")
    kwargs.setdefault("UsePixel", True)
    kwargs.setdefault("UseOverlap", False)

    result.merge(ActsTrkBaseSpacePointAnalysisCfg(flags, 
                                                  name = name,
                                                  histoPath = "PixelSpacePoints",
                                                  ntupleName = "PixelSpacePoints",
                                                  **kwargs))
    return result


def ActsTrkStripSpacePointAnalysisCfg(flags, name="ActsTrkStripSpacePointAnalysis", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result = ITkStripReadoutGeometryCfg(flags)

    kwargs.setdefault("SpacePointContainerKey", "ITkStripSpacePoints")
    kwargs.setdefault("StripClusterContainerKey", "ITkStripClusters")
    kwargs.setdefault("UsePixel", False)
    kwargs.setdefault("UseOverlap", False)

    result.merge(ActsTrkBaseSpacePointAnalysisCfg(flags,
                                                  name = name,
                                                  histoPath = "StripSpacePoints",
                                                  ntupleName = "StripSpacePoints",
                                                  **kwargs))
    return result


def ActsTrkStripOverlapSpacePointAnalysisCfg(flags, name="ActsTrkStripOverlapSpacePointAnalysis", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result = ITkStripReadoutGeometryCfg(flags)

    kwargs.setdefault("SpacePointContainerKey", "ITkStripOverlapSpacePoints")
    kwargs.setdefault("StripClusterContainerKey", "ITkStripClusters")
    kwargs.setdefault("UsePixel", False)
    kwargs.setdefault("UseOverlap", True)

    result.merge(ActsTrkBaseSpacePointAnalysisCfg(flags,
                                                  name = name,
                                                  histoPath = "StripOverlapSpacePoints",
                                                  ntupleName = "StripOverlapSpacePoints",
                                                  **kwargs))
    return result


def ActsTrkBaseSeedAnalysisCfg(flags, 
                               name,
                               histoPath,
                               ntupleName,
                               **kwargs):
    acc = ComponentAccumulator()

    isPixel = 'Pixel' in name
    perp_min = 0 if isPixel else 300
    perp_max = 400 if isPixel else 1100

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'ActsTrkSeedAnalysisCfg')

    kwargs.setdefault('InputSeedCollection', 'ITkPixelSeeds')

    if flags.ITk.Tracking.doTruth:
        from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
        geoTool = acc.getPrimaryAndMerge(ActsTrackingGeometryToolCfg(flags))
        
        # ATLAS Converter Tool
        from ActsGeometry.ActsGeometryConfig import ActsATLASConverterToolCfg
        converterTool = acc.popToolsAndMerge(ActsATLASConverterToolCfg(flags))
        
        # Track Param Estimation Tool
        from ActsTrkTrackParamsEstimationTool.ActsTrkTrackParamsEstimationToolConfig import TrackParamsEstimationToolCfg
        trackEstimationTool = acc.popToolsAndMerge(TrackParamsEstimationToolCfg(flags))        

        kwargs.setdefault('TrackingGeometryTool', geoTool)
        kwargs.setdefault('ATLASConverterTool', converterTool)
        kwargs.setdefault('TrackParamsEstimationTool', trackEstimationTool)

    monitoringAlgorithm = helper.addAlgorithm(CompFactory.ActsTrk.SeedAnalysis, name, **kwargs)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsTrkSeedAnalysis', 'ActsTrkAnalysis')

    monitoringGroup.defineHistogram('Nseed', title='Number of Seeds;N;Entries', type='TH1I', path=f'{histoPath}',
                                    xbins=100, xmin=0, xmax=0)



    monitoringGroup.defineHistogram('z1,r1;zr1', title='Bottom SP - Z coordinate vs R;z [mm];r [mm]', type='TH2F', path=f'{histoPath}',
                                    xbins=1500, xmin=-3000, xmax=3000,
                                    ybins=400, ymin=perp_min, ymax=perp_max)
    monitoringGroup.defineHistogram('z2,r2;zr2', title='Middle SP - Z coordinate vs R;z [mm];r [mm]', type='TH2F', path=f'{histoPath}',
                                    xbins=1500, xmin=-3000, xmax=3000,
                                    ybins=400, ymin=perp_min, ymax=perp_max)
    monitoringGroup.defineHistogram('z3,r3;zr3', title='Top SP - Z coordinate vs R;z [mm];r [mm]', type='TH2F', path=f'{histoPath}',
                                    xbins=1500, xmin=-3000, xmax=3000,
                                    ybins=400, ymin=perp_min, ymax=perp_max)

    if flags.ITk.Tracking.doTruth:
        monitoringGroup.defineHistogram('passed,estimated_eta;EfficiencyEta', title='Efficiency vs eta;eta;Efficiency', type='TEfficiency', path=f'{histoPath}',
                                        xbins=50, xmin=-5, xmax=5)
        monitoringGroup.defineHistogram('passed,estimated_pt;EfficiencyPt', title='Efficiency vs pT;pT [GeV];Efficiency', type='TEfficiency', path=f'{histoPath}',
                                        xbins=30, xmin=0, xmax=120)

    # Tree
    list_variables = "x1,y1,z1,r1,x2,y2,z2,r2,x3,y3,z3,r3,pt,theta,eta,d0,dzdr_b,dzdr_t,penalty,event_number,actual_mu"
    tree_def = "x1/vector<double>:y1/vector<double>:z1/vector<double>:r1/vector<double>:x2/vector<double>:y2/vector<double>:z2/vector<double>:r2/vector<double>:x3/vector<double>:y3/vector<double>:z3/vector<double>:r3/vector<double>\
:pt/vector<float>:theta/vector<float>:eta/vector<float>:d0/vector<float>:dzdr_b/vector<float>:dzdr_t/vector<float>:penalty/vector<float>:event_number/l:actual_mu/F"
    if flags.ITk.Tracking.doTruth:
        list_variables += ",truth_barcode,truth_prob"
        tree_def += ":truth_barcode/vector<int>:truth_prob/vector<double>"

    monitoringGroup.defineTree(f'{list_variables};{ntupleName}',
                               path='ntuples',
                               treedef=tree_def )

    acc.merge(helper.result())
    return acc



def ActsTrkPixelSeedAnalysisCfg(flags, name = "ActsTrkPixelSeedAnalysis", **kwargs):
    kwargs.setdefault('InputSeedCollection', 'ITkPixelSeeds')

    if flags.ITk.Tracking.doTruth:
        kwargs.setdefault('DetectorElements', 'ITkPixelDetectorElementCollection')
        kwargs.setdefault('ITkClustersTruth', 'PRD_MultiTruthITkPixel')

    return ActsTrkBaseSeedAnalysisCfg(flags, name, histoPath='PixelSeeds', ntupleName='PixelSeeds', **kwargs)


def ActsTrkStripSeedAnalysisCfg(flags, name = "ActsTrkStripSeedAnalysis", **kwargs):
    kwargs.setdefault('InputSeedCollection', 'ITkStripSeeds')

    if flags.ITk.Tracking.doTruth:
        kwargs.setdefault('UsePixel', False)
        kwargs.setdefault('DetectorElements', 'ITkStripDetectorElementCollection')
        kwargs.setdefault('ITkClustersTruth', 'PRD_MultiTruthITkStrip')

    return ActsTrkBaseSeedAnalysisCfg(flags, name, histoPath='StripSeeds', ntupleName='StripSeeds', **kwargs)


def ActsTrkBaseEstimatedTrackParamsAnalysisCfg(flags,
                                               name,
                                               histoPath,
                                               ntupleName,
                                               **kwargs):
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'ActsTrkEstimatedTrackParamsAnalysisCfg')

    monitoringAlgorithm = helper.addAlgorithm(CompFactory.ActsTrk.EstimatedTrackParamsAnalysis, name, **kwargs)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsTrkEstimatedTrackParamsAnalysis', 'ActsTrkAnalysis')

    monitoringGroup.defineHistogram('Nparams', title='Number of Estimated Parameters from Seeds;N;Entries', type='TH1I', path=f'{histoPath}',
                                    xbins=100, xmin=0, xmax=0)

    monitoringGroup.defineTree(f"track_param_pt,track_param_eta,track_param_phi,track_param_loc0,track_param_loc1,track_param_theta,track_param_qoverp,track_param_time,track_param_charge;{ntupleName}",
                               path="ntuples",
                               treedef="track_param_pt/vector<double>:track_param_eta/vector<double>:track_param_phi/vector<double>:track_param_loc0/vector<double>:track_param_loc1/vector<double>:track_param_theta/vector<double>:track_param_qoverp/vector<double>:track_param_time/vector<double>:track_param_charge/vector<int>")

    return helper.result()


def ActsTrkPixelEstimatedTrackParamsAnalysisCfg(flags, name = 'ActsTrkPixelEstimatedTrackParamsAnalysis', **kwargs):
    kwargs.setdefault('InputTrackParamsCollection', 'ITkPixelEstimatedTrackParams')
    return ActsTrkBaseEstimatedTrackParamsAnalysisCfg(flags, name, histoPath = 'PixelEstimatedTrackParams', ntupleName = 'PixelEstimatedTrackParams', **kwargs)


def ActsTrkStripEstimatedTrackParamsAnalysisCfg(flags, name = 'ActsTrkStripEstimatedTrackParamsAnalysis', **kwargs):
    kwargs.setdefault('InputTrackParamsCollection', 'ITkStripEstimatedTrackParams')
    return ActsTrkBaseEstimatedTrackParamsAnalysisCfg(flags, name, histoPath = 'StripEstimatedTrackParams', ntupleName = 'StripEstimatedTrackParams', **kwargs)


def ActsTrkClusterAnalysisCfg(flags):
    acc = ComponentAccumulator()
    if flags.Detector.EnableITkPixel:
        acc.merge(ActsTrkPixelClusterAnalysisCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ActsTrkStripClusterAnalysisCfg(flags))
    return acc


def ActsTrkSpacePointAnalysisCfg(flags):
    acc = ComponentAccumulator()
    if flags.Detector.EnableITkPixel:
        acc.merge(ActsTrkPixelSpacePointAnalysisCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ActsTrkStripSpacePointAnalysisCfg(flags))
        acc.merge(ActsTrkStripOverlapSpacePointAnalysisCfg(flags))
    return acc


def ActsTrkSeedAnalysisCfg(flags):
    acc = ComponentAccumulator()
    if flags.Detector.EnableITkPixel:
        acc.merge(ActsTrkPixelSeedAnalysisCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ActsTrkStripSeedAnalysisCfg(flags))
    return acc


def ActsTrkEstimatedTrackParamsAnalysisCfg(flags):
    acc = ComponentAccumulator()
    if flags.Detector.EnableITkPixel:
        acc.merge(ActsTrkPixelEstimatedTrackParamsAnalysisCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ActsTrkStripEstimatedTrackParamsAnalysisCfg(flags))
    return acc
