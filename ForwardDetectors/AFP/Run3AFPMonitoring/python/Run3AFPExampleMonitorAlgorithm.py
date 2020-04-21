# 
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file Run3AFPExampleMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief Example python configuration for the Run III AthenaMonitoring package
'''

def Run3AFPExampleMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'Run3AFPMonitorCfg')

    from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPSiLayerAlgorithm
    afpSiLayerAlgorithm = helper.addAlgorithm(AFPSiLayerAlgorithm,'AFPSiLayerAlg')

    from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPToFAlgorithm
    afpToFAlgorithm = helper.addAlgorithm(AFPToFAlgorithm,'AFPToFAlg')

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    AFPSiGroup = helper.addGroup(afpSiLayerAlgorithm, 'AFPSiLayerTool', 'AFP/') 
    AFPToFGroup = helper.addGroup(afpToFAlgorithm, 'AFPToFTool', 'AFP/')

    AFPSiGroup.defineHistogram('lb,nSiHits', title='Luminosity Block;lb;total number of Hits', type='TProfile', path='SiT/',xbins=1000,xmin=-0.5,xmax=999.5 ) 
    AFPToFGroup.defineHistogram('lb,nTofHits', title='Luminosity Block;lb;total number of Hits', type='TProfile',  path='ToF/',xbins=1000,xmin=-0.5,xmax=999.5) 

    AFPToFGroup.defineHistogram('numberOfHit_S0', title='Number of hit per bar station 0;total number of Hits',  path='ToF/',xbins=3,xmin=-0.5,xmax=2.5)
    AFPToFGroup.defineHistogram('numberOfHit_S3', title='Number of hit per bar station 3;total number of Hits',  path='ToF/',xbins=3,xmin=-0.5,xmax=2.5)


    # Using a map of groups
    layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
    combinedList = ['farAside', 'nearAside', 'nearCside', 'farCside']

    array = helper.addArray([combinedList,layerList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath = 'AFP/SiT/')
    array.defineHistogram('pixelColIDChip', title='1D hitmap for {0} Layer {1}', path='PixelColIDChip', xbins=80, xmin=0.5, xmax=80.5)
    array.defineHistogram('pixelRowIDChip', title='1D hitmap for {0} Layer {1}', path='PixelRowIDChip', xbins=336, xmin=0.5, xmax=336.5)
    array.defineHistogram('pixelColIDChip,pixelRowIDChip', title='hitmap for {0} Layer {1}', type='TH2F', path='pixelColRow2D', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)
    array.defineHistogram('timeOverThreshold', title='1D Time over threshold for {0} Layer {1}', path='SiTimeOverThreshold', xbins=60, xmin=0, xmax=20)

    #array.defineHistogram('pixelColIDChip,pixelColIDChip', title='Correlation Row {0} Row {1}', type='TH2F', path='Correlation', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)
    #array.defineHistogram('pixelRowIDChip,pixelRowIDChip', title='Correlation Col {0} Col {1}', type='TH2F', path='Correlation', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)

    # Finalize. The return value should be a tuple of the ComponentAccumulator
    return helper.result()
    

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = ''
    file = '/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00354309/data18_13TeV.00354309.physics_Main.recon.AOD.f946/data18_13TeV.00354309.physics_Main.recon.AOD.f946._lb0130._0001.1'
    
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'AFPOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = Run3AFPExampleMonitoringConfig(ConfigFlags)
    cfg.merge(exampleMonitorAcc)

    cfg.run(1000) #use cfg.run(20) to only run on first 20 events


