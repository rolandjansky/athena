#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaMonitoring import AthMonitorCfgHelper
from ActsMonitoring.ActsBinnings import ITkPixelBinnings, ITkStripBinnings, InDetPixelBinnings, InDetSCTBinnings
from ActsMonitoring.ActsBinnings import defineHistogramWithBinning

def ActsSeedMonitoringAlgorithmCfg(ConfigFlags,
                                   name: str = 'ActsSeedMonitoringAlgorithm',
                                   inputCollection: str = '',
                                   **options) -> ComponentAccumulator:
    assert isinstance(name, str)
    assert isinstance(inputCollection, str)

    helper = AthMonitorCfgHelper(ConfigFlags,'ActsMonCfg')

    options['name'] = f'{name}_{inputCollection}'
    options['InputSeedCollection'] = inputCollection

    acc = ComponentAccumulator()
    ActsTrk__MonitoringAlgo = CompFactory.getComp("ActsTrk::ActsSeedMonitoringAlgorithm")

    # Set Monitoring
    monitoringAlgorithm = helper.addAlgorithm(ActsTrk__MonitoringAlgo, **options)
    monitoringGroup = helper.addGroup(monitoringAlgorithm, 'ActsMonitoring', f'ActsMonitoring/Seeds/{inputCollection}')

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
                               'Nseed', type="TH1I", title='Number of Seeds;N;Entries',
                               binningX=binnings.Nseeds)

    # 1D
    defineHistogramWithBinning(monitoringGroup,
                               'seed_bottom_sp_pos_x', type='TH1F', title='bottom sp - x coordinate;z [mm];Entries',
                               binningX=binnings.Pos_X)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_bottom_sp_pos_y', type='TH1F', title='bottom sp - y coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Y)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_bottom_sp_pos_z', type='TH1F', title='bottom sp - z coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Z)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_bottom_sp_pos_r', type='TH1F', title='bottom sp - r coordinate;r [mm];Entries',
                               binningX=binnings.Pos_R)

    defineHistogramWithBinning(monitoringGroup,
                               'seed_middle_sp_pos_x', type='TH1F', title='middle sp - x coordinate;z [mm];Entries',
                               binningX=binnings.Pos_X)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_middle_sp_pos_y', type='TH1F', title='middle sp - y coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Y)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_middle_sp_pos_z', type='TH1F', title='middle sp - z coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Z)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_middle_sp_pos_r', type='TH1F', title='middle sp - r coordinate;r [mm];Entries',
                               binningX=binnings.Pos_R)

    defineHistogramWithBinning(monitoringGroup,
                               'seed_top_sp_pos_x', type='TH1F', title='top sp - x coordinate;z [mm];Entries',
                               binningX=binnings.Pos_X)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_top_sp_pos_y', type='TH1F', title='top sp - y coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Y)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_top_sp_pos_z', type='TH1F', title='top sp - z coordinate;z [mm];Entries',
                               binningX=binnings.Pos_Z)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_top_sp_pos_r', type='TH1F', title='top sp - r coordinate;r [mm];Entries',
                               binningX=binnings.Pos_R)

    # 2D
    defineHistogramWithBinning(monitoringGroup,
                               'seed_bottom_sp_pos_z,seed_bottom_sp_pos_r', type='TH2F', title='Bottom SP - Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z,
                               binningY=binnings.Pos_R)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_middle_sp_pos_z,seed_middle_sp_pos_r', type='TH2F', title='Middle SP - Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z,
                               binningY=binnings.Pos_R)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_top_sp_pos_z,seed_top_sp_pos_r', type='TH2F', title='Top SP - Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z,
                               binningY=binnings.Pos_R )

    defineHistogramWithBinning(monitoringGroup,
                               'seed_bottom_sp_pos_z,seed_bottom_sp_pos_signed_r', type='TH2F', title='Bottom SP - Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z,
                               binningY=binnings.Pos_Signed_R)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_middle_sp_pos_z,seed_middle_sp_pos_signed_r', type='TH2F', title='Middle SP - Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z,
                               binningY=binnings.Pos_Signed_R)
    defineHistogramWithBinning(monitoringGroup,
                               'seed_top_sp_pos_z,seed_top_sp_pos_signed_r', type='TH2F', title='Top SP - Z coordinate vs R;z [mm];r [mm]',
                               binningX=binnings.Pos_Z,
                               binningY=binnings.Pos_Signed_R)
    
    acc.merge(helper.result())
    return acc

