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
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    #from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPSiLayerAlgorithm
    afpSiLayerAlgorithmFac = CompFactory.AFPSiLayerAlgorithm
    afpSiLayerAlgorithm = helper.addAlgorithm(afpSiLayerAlgorithmFac,'AFPSiLayerAlg')

    #from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPToFAlgorithm
    afpToFAlgorithmFac = CompFactory.AFPToFAlgorithm
    afpToFAlgorithm = helper.addAlgorithm(afpToFAlgorithmFac,'AFPToFAlg')

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    AFPSiGroup = helper.addGroup(afpSiLayerAlgorithm, 'AFPSiLayerTool', 'AFP/') 
    AFPToFGroup = helper.addGroup(afpToFAlgorithm, 'AFPToFTool', 'AFP/')

    AFPSiGroup.defineHistogram('lb,nSiHits', title='Luminosity Block;lb;total number of Hits', type='TProfile', path='SiT/',xbins=1000,xmin=-0.5,xmax=999.5 ) 
    AFPToFGroup.defineHistogram('lb,nTofHits', title='Luminosity Block;lb;total number of Hits', type='TProfile',  path='ToF/',xbins=1000,xmin=-0.5,xmax=999.5) 

    AFPToFGroup.defineHistogram('numberOfHit_S0', title='Number of hit per bar station 0;total number of Hits', path='ToF/',xbins=4,xmin=-0.5,xmax=3.5)
    AFPToFGroup.defineHistogram('numberOfHit_S3', title='Number of hit per bar station 3;total number of Hits', path='ToF/',xbins=4,xmin=-0.5,xmax=3.5)


    # Using a map of groups
    layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
    combinedList = ['farAside', 'nearAside', 'nearCside', 'farCside']

    array = helper.addArray([combinedList,layerList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath = 'AFP/SiT/')
    array.defineHistogram('pixelColIDChip', title='1D hitmap for {0} Layer {1};pixelColIDChip', path='PixelColIDChip', xbins=80, xmin=0.5, xmax=80.5)
    array.defineHistogram('pixelRowIDChip', title='1D hitmap for {0} Layer {1};pixelRowIDChip', path='PixelRowIDChip', xbins=336, xmin=0.5, xmax=336.5)
    array.defineHistogram('pixelColIDChip,pixelRowIDChip', title='hitmap for {0} Layer {1};pixelColIDChip;pixelRowIDChip', type='TH2F', path='pixelColRow2D', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)
    array.defineHistogram('timeOverThreshold', type='TH1F', title='1D Time over threshold for {0} Layer {1};timeOverThreshold', path='SiTimeOverThreshold', xbins=60, xmin=0, xmax=20)
    array.defineHistogram('clusterX,clusterY', title='Cluster position in station {0} Layer {1};clusterX;clusterY', type='TH2F', path='Cluster', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)


    array = helper.addArray([combinedList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath='AFP/Track/')
    array.defineHistogram('trackX,trackY', title='Track posistion position in station {0};trackX;trackY', type='TH2F', path='Track', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)



    arrayOneList = helper.addArray([combinedList], afpToFAlgorithm, 'AFPToFTool', topPath='AFP/ToF/')
    arrayOneList.defineHistogram('trainID,barInTrainID', title='ToF hit bar vs train {0};trainID;barInTrainID', type='TH2F', path='HitBarvsTrain/',xbins=4,xmin=-0.5,xmax=3.5,ybins=4,ymin=-0.5,ymax=3.5)

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
    file ='/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.merge.AOD.r10258_p3399_tid13243079_00/AOD.13243079._000003.pool.root.1' 

    
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'AFPOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = Run3AFPExampleMonitoringConfig(ConfigFlags)
    cfg.merge(exampleMonitorAcc)

    cfg.run(10000) #use cfg.run(20) to only run on first 20 events


