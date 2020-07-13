#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file TileCalCellMonAlg.py
@brief Python configuration of TileCalCellMonAlg algorithm for the Run III
'''

from __future__ import print_function

def TileCalCellMonAlgConfig(inputFlags, **kwargs):
    ''' Function to configure TileCalCellMonAlg algorithm in the monitoring system.'''

    kwargs.setdefault('MonGroupName', 'TileEventFiter')
    kwargs.setdefault('useBeamBackgroundRemoval', False)
    kwargs.setdefault('useLArNoisyAlg', False)
    kwargs.setdefault('useLArCollisionFilterTool', False)

    if not (inputFlags.Common.isOnline == 'online' or inputFlags.Input.isMC):
        kwargs.setdefault('useReadyFilterTool', True)
        kwargs.setdefault('useBadLBTool', False) # FIXME: when new LArBadLBFilterTool config is ready
    else:
        kwargs.setdefault('useReadyFilterTool', False)
        kwargs.setdefault('useBadLBTool', False)

    from AthenaCommon.SystemOfUnits import MeV
    kwargs.setdefault('EnergyThreshold', 300.0 * MeV)
        
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TileCalMonCfg')

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg = LArGMCfg(inputFlags)

    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(TileGMCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(inputFlags))

    if kwargs['useLArCollisionFilterTool']:
        from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
        cfg.merge(LArCollisionTimeCfg(inputFlags))

    if kwargs['useReadyFilterTool'] and 'ReadyFilterTool' not in kwargs:
        from AthenaMonitoring.AtlasReadyFilterConfig import AtlasReadyFilterCfg
        readyFilterTool = cfg.popToolsAndMerge(AtlasReadyFilterCfg(inputFlags))
        kwargs['ReadyFilterTool'] = readyFilterTool
        
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileCalCellMonAlg = helper.addAlgorithm(CompFactory.TileCalCellMonAlg, 'TileCalCellMonAlg')
    
    for k, v in kwargs.items():
        setattr(tileCalCellMonAlg, k, v)

    binLabels = ["TotalEvents", "ATLAS Ready", "with Good LAr LB", "with No LAr Collision",
                 "with No Beam Background", "with No Trigger Filter","with No LArError"] 
        
    if not tileCalCellMonAlg.useReadyFilterTool:
       binLabels[1] = "ATLAS Ready-OFF"
    if not tileCalCellMonAlg.useBadLBTool:
       binLabels[2] = "Good LAr LB-OFF"
    if not tileCalCellMonAlg.useLArCollisionFilterTool:
       binLabels[3] = "LAr collision-OFF"
    if not tileCalCellMonAlg.useBeamBackgroundRemoval:
       binLabels[4] = "Beam backgr.-OFF"
    if not tileCalCellMonAlg.useLArNoisyAlg:
       binLabels[5] = "LAr Error Veto-OFF"   

    topPath = '/CaloMonitoring/TileCellMon_NoTrigSel/General/'
    tileFilterGroup = helper.addGroup(tileCalCellMonAlg, tileCalCellMonAlg.MonGroupName, topPath)

    from CaloMonitoring.CaloMonAlgBase import CaloBaseHistConfig
    CaloBaseHistConfig(tileFilterGroup, 'Summary/', binLabels)  

    # 1) Configure histogram with TileCalCellMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileCalCellMonAlg, 'TileCalCellMonExecuteTime', topPath)
    executeTimeGroup.defineHistogram('TIME_execute', path = 'Summary', type='TH1F',
                                     title = 'Time for execute TileCalCellMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 100000)

    # 2) Configure histograms with occupancy maps over threshold (4 noise sigma) per Tile sample
    samplesWithoutE = ['A', 'BC', 'D', '']
    noiseEtaPhiArray = helper.addArray([len(samplesWithoutE)], tileCalCellMonAlg, 'CellsNoiseXEtaVSPhi', topPath = topPath)
    for postfix, tool in noiseEtaPhiArray.Tools.items():
        sample = samplesWithoutE[int( postfix.split('_')[1] )]
        title = ('Number of Tile Cells %s' % sample) + ' with E > 4 sigma (DB);#eta;#phi'
        name = 'eta,phi;CellsNoiseXEtaVSPhi' + (sample + 'cells' if sample else '')
        tool.defineHistogram(name, title = title, type = 'TH2F',
                             xbins = 17, xmin = -1.7, xmax = 1.7,
                             ybins = 64, ymin = -3.14, ymax = 3.14)

    # 3) Configure histogram with number of 4 sigma seeds per Tile hash ID
    noiseHashGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXNoiseXHash', topPath)
    noiseHashGroup.defineHistogram('hash;CellsXNoiseXHash', path = '', type='TH1F',
                                   title = 'Number of 4 sigma seeds per hash;Tile Cell Hash ID;Events',
                                   xbins = 5184, xmin = -0.5, xmax = 5183.5)

    # 4) Configure histogram with Tile cell energy/noise (DB) ratio
    noiseHashGroup = helper.addGroup(tileCalCellMonAlg, 'CellsNoiseTile', topPath)
    noiseHashGroup.defineHistogram('noise;CellsNoiseTile', path = '', type='TH1F',
                                   title = 'Energy/Noise (DB) of TileCal;Cell Energy / sigma (DB);Events',
                                   xbins = 200, xmin = -10.0, xmax = 10.0)

    # 5) Configure histogram with mean Tile cell noise (DB) vs eta
    noiseEtaGroup = helper.addGroup(tileCalCellMonAlg, 'CellsNoiseXEta', topPath)
    noiseEtaGroup.defineHistogram('eta,noise;CellsNoiseXEta', path = '', type='TProfile',
                                   title = 'Tile Cell noise #sigma (DB) vs #eta;#eta;Mean Cell noise (DB) [MeV]',
                                   xbins = 17, xmin = -1.7, xmax = 1.7)

    # 6) Configure histogram with mean Tile cell noise (DB) vs phi
    noisePhiGroup = helper.addGroup(tileCalCellMonAlg, 'CellsNoiseXPhi', topPath)
    noisePhiGroup.defineHistogram('phi,noise;CellsNoiseXPhi', path = '', type='TProfile',
                                   title = 'Tile Cell noise #sigma (DB) vs #phi;#phi;Mean Cell noise (DB) [MeV]',
                                   xbins = 64, xmin = -3.14, xmax = 3.14)

    # 7) Configure histogram with number of Tile cell over threshold
    nCellsGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXN', topPath)
    nCellsGroup.defineHistogram('nCells;CellsXN', path = '', type='TH1F',
                                title = 'Number of Tile Cells over threshold;Number of Tile Cells; Events',
                                xbins = 250, xmin = 0, xmax = 500)

    # 8) Configure histogram with Tile cell energy in GeV
    energyGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXE', topPath)
    energyGroup.defineHistogram('energy;CellsXE', path = '', type='TH1F',
                                   title = 'Energy of Tile Cells;Tile Cell Energy [GeV]; Events',
                                   xbins = 50, xmin = 0, xmax = 20)

    # 9) Configure histogram with mean Tile cell energy in GeV vs eta
    energyEtaGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXEta', topPath)
    energyEtaGroup.defineHistogram('eta,energy;CellsXEta', path = '', type='TProfile',
                                   title = 'Tile Cell Energy vs #eta;#eta;Mean Cell Energy [GeV]',
                                   xbins = 17, xmin = -1.7, xmax = 1.7)

    # 10) Configure histogram with mean Tile cell energy in GeV vs phi
    energyPhiGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXPhi', topPath)
    energyPhiGroup.defineHistogram('phi,energy;CellsXPhi', path = '', type='TProfile',
                                   title = 'Tile Cell Energy vs #phi;#phi;Mean Cell Energy [GeV]',
                                   xbins = 64, xmin = -3.14, xmax = 3.14)

    # 11) Configure histogram with mean Tile cell energy in GeV vs tower
    energyTowerGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXTower', topPath)
    energyTowerGroup.defineHistogram('tower,energy;CellsXTower', path = '', type='TProfile',
                                     title = 'Tile Cell Energy vs tower;Tower;Mean Cell Energy [GeV]',
                                     xbins = 18, xmin = 0, xmax = 18)

    # 12) Configure histogram with occupancy map over threshold vs eta and phi
    occupEtaPhiGroup = helper.addGroup(tileCalCellMonAlg, 'CellsXEtaVSPhi', topPath)
    occupEtaPhiTitle = ('Number of Tile Cell above threshold %s MeV;#eta;#phi' %  kwargs['EnergyThreshold'])
    occupEtaPhiGroup.defineHistogram('eta,phi;CellsXEtaVSPhi', path = '', type='TH2F', title = occupEtaPhiTitle,
                                     xbins = 17, xmin = -1.7, xmax = 1.7, ybins = 64, ymin = -3.14, ymax = 3.14)

    # 13) Configure histograms with mean Tile cell energy vs module per sample
    samples = ['A', 'BC', 'D', 'E']
    energyModuleArray = helper.addArray([len(samples)], tileCalCellMonAlg, 'CellsXModule', topPath = topPath)
    for postfix, tool in energyModuleArray.Tools.items():
        sampleIdx = int( postfix.split('_')[1] )
        sample = samples[sampleIdx]
        title = ('Tile Sampling %s' % sample) + ';Module;Mean Cell Energy [GeV]'
        name = 'module,energy;CellsXModuleS' + str(sampleIdx + 1)
        tool.defineHistogram(name, title = title, type = 'TProfile',
                             xbins = 64, xmin = 1, xmax = 65)

    
    accumalator = helper.result()
    cfg.merge(accumalator)
    return cfg
    

if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import INFO
    from AthenaCommon.Logging import log
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.Output.HISTFileName = 'TileCalCellMonOutput.root'
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    cfg.merge( TileCalCellMonAlgConfig(ConfigFlags) ) 

    ConfigFlags.dump()
    cfg.printConfig(withDetails = True, summariseProps = True)
    
    cfg.store( open('TileCalCellMonAlg.pkl','wb') )
    
    sc = cfg.run(3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
