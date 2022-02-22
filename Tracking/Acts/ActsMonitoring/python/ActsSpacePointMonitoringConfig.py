#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaMonitoring import AthMonitorCfgHelper
from ActsMonitoring.ActsBinnings import ITkPixelBinnings, ITkStripBinnings, InDetPixelBinnings, InDetSCTBinnings
from ActsMonitoring.ActsBinnings import defineHistogramWithBinning

def ActsSpacePointMonitoringAlgorithmCfg(ConfigFlags,
                                         name: str = 'ActsSpacePointMonitoringAlgorithm',
                                         inputCollection: str = '',
                                         **options) -> ComponentAccumulator:
    assert isinstance(name, str)
    assert isinstance(inputCollection, str)

    helper = AthMonitorCfgHelper(ConfigFlags,'ActsMonCfg')

    options['name'] = f'{name}_{inputCollection}'
    options['InputSpacePointCollection'] = inputCollection

    acc = ComponentAccumulator()
    ActsTrk__MonitoringAlgo = CompFactory.getComp("ActsTrk::ActsSpacePointMonitoringAlgorithm")

    # Set Monitoring
    monitoringAlgorithm = helper.addAlgorithm(ActsTrk__MonitoringAlgo, **options)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsMonitoring', f'ActsMonitoring/SpacePoints/{inputCollection}')

    # Get Binnings
    binnings = None
    if ConfigFlags.Detector.GeometryITk and 'Pixel' in inputCollection:
        binnings = ITkPixelBinnings()
    elif ConfigFlags.Detector.GeometryITk and 'Pixel' not in inputCollection: 
        binnings = ITkStripBinnings()
    elif not ConfigFlags.Detector.GeometryITk and 'Pixel' in inputCollection: 
        binnings = InDetPixelBinnings()
    elif not ConfigFlags.Detector.GeometryITk and 'Pixel' not in inputCollection:
        binnings = InDetSCTBinnings()
    else:
        raise ValueError('Could not determine configuration for Acts Monitoring')

    # Global
    defineHistogramWithBinning(monitoringGroup,
                               'Nsp', type="TH1I", title='Number of Space Points;N;Entries',
                               binningX=binnings.Nsp)

    # 1D
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_x', type='TH1F', title='x coordinate;x [mm];Entries',
                               binningX=binnings.Pos_X)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_y', type='TH1F', title='y coordinate;y [mm];Entries',
                               binningX=binnings.Pos_Y)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_z', type='TH1F', title='z coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Z)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_r', type='TH1F', title='r coordinate;r [mm];Entries',
                               binningX=binnings.Pos_R)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_signed_r', type='TH1F', title='signed r coordinate;r [mm];Entries',
                               binningX=binnings.Pos_Signed_R)

    defineHistogramWithBinning(monitoringGroup,
                               'sp_cov_r', type='TH1F', title='Covariance R;cov;Entries',
                               binningX=binnings.Cov_R)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_cov_z', type='TH1F', title='Covariance Z;cov;Entries',
                               binningX=binnings.Cov_Z)


    # 2D
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_x,sp_pos_y', type='TH2F', title='X coordinate vs Y;x [mm];y [mm]',
                               binningX=binnings.Pos_X, 
                               binningY=binnings.Pos_Y)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_z,sp_pos_y', type='TH2F', title='Z coordinate vs Y;z [mm];y [mm]',
                               binningX=binnings.Pos_Z, 
                               binningY=binnings.Pos_Y)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_z,sp_pos_r', type='TH2F', title='Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z, 
                               binningY=binnings.Pos_R)
    defineHistogramWithBinning(monitoringGroup,
                               'sp_pos_z,sp_pos_signed_r', type='TH2F', title='Z coordinate vs signed R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z, 
                               binningY=binnings.Pos_Signed_R)

    acc.merge(helper.result())
    return acc


