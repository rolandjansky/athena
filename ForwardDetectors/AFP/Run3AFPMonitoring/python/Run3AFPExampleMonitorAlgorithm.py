# 
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file Run3AFPExampleMonitorAlgorithm.py
@author N. Dikic
@date 2020-08-12
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

    AFPSiGroup.defineHistogram('lb,nSiHits', title='Total number of hits;lb;total number of Hits', type='TProfile', path='SiT/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('lb,muPerBCID', title='<mu>;lumiBlock;<mu>', type='TProfile', path='SiT/', xbins=1000, xmin=-0.5, xmax=999.5)
    #AFPSiGroup.defineHistogram('layerNumber,layerEfficiency', title='LayerEfficiency;layerNumber', path='SiT/', xbins = 16, xmin=0.5, xmax=16.5, ybins=100, ymin=0, ymax=1)
    #AFPSiGroup.defineHistogram('layerEfficiency', type='TH1F', title='1D layer efficiency;layerEfficiency', path='SiT/', xbins=16, xmin=0.5, xmax=16.5)

    AFPToFGroup.defineHistogram('lb,nTofHits', title='Multiplicity;lb;total number of Hits', type='TProfile', path='ToF/', xbins=1000, xmin=-0.5, xmax=999.5) 
    AFPToFGroup.defineHistogram('numberOfHit_S0', title='Number of hit per bar station 0;bar', path='ToF/', xbins=4, xmin=-0.5, xmax=3.5)
    AFPToFGroup.defineHistogram('numberOfHit_S3', title='Number of hit per bar station 3;bar', path='ToF/', xbins=4, xmin=-0.5, xmax=3.5)


    # Using a map of groups
    layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
    combinedList = ['farAside', 'nearAside', 'nearCside', 'farCside']

    array = helper.addArray([combinedList,layerList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath = 'AFP/SiT/')
    array.defineHistogram('pixelColIDChip', title='Hits per column for {0} layer {1};pixelColIDChip; entries', path='PixelColIDChip', xbins=80, xmin=0.5, xmax=80.5)
    array.defineHistogram('pixelRowIDChip', title='Hits per row for {0} Layer {1};pixelRowIDChip; entries', path='PixelRowIDChip', xbins=336, xmin=0.5, xmax=336.5)
    array.defineHistogram('pixelRowIDChip,pixelColIDChip', title='Hitmap for {0} Layer {1};pixelRowIDChip;pixelColIDChip', type='TH2F', path='pixelColRow2D', xbins=336, xmin=0.5, xmax=336.5, ybins=80, ymin=0.5, ymax=80.5)
    array.defineHistogram('timeOverThreshold', type='TH1F', title='Time over threshold for {0} Layer {1};timeOverThreshold; entries', path='SiTimeOverThreshold', xbins=16, xmin=0.5, xmax=16.5)
    array.defineHistogram('clusterY,clusterX', title='Cluster position in station {0} Layer {1};x [mm];y [mm]', type='TH2F', path='Cluster', xbins=336, xmin=0.0, xmax=17.0, ybins=80, ymin=0.0, ymax=20.0)
    array.defineHistogram('lb,clustersPerPlane', title='Number of clusters per event divided by <mu> in station {0}, layer {1};lb; clusters per event by <mu>', type='TH2F', path='clustersPerPlane', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneFront', title='(Front BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lb; clusters per event by <mu>', type='TH2F', path='clustersPerPlaneFront', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneEnd', title='(End BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lb; clusters per event by <mu>', type='TH2F', path='clustersPerPlaneEnd', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneMiddle', title='(Middle BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lb; clusters per event by<mu>', type='TH2F', path='clustersPerPlaneMiddle', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlane2', title='Number of clusters per event divided by <mu> in station {0}, layer {1};lb; clustersPerEvent by <mu>', type='TProfile', path='clustersPerPlane2', xbins=1000, xmin=-0.5, xmax=999.5)


    array = helper.addArray([combinedList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath='AFP/SiT/')
    array.defineHistogram('trackY,trackX', title='Track in AFP station {0};x [mm];y [mm]', type='TH2F', path='Track', xbins=336, xmin=0.0, xmax=17.0, ybins=80, ymin=0.0, ymax=20.0)
    array.defineHistogram('lb,clustersPerStation', title ='Number of clusters per event divided by <mu> in station {0};lb; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationFront', title ='(Front) Number of clusters per event divided by <mu> in station {0};lb; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationEnd', title ='(End) Number of clusters per event divided by <mu> in station {0};lb; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationMiddle', title ='(Middle) Number of clusters per event divided by <mu> in station {0};lb; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)

    arrayOneList = helper.addArray([combinedList], afpToFAlgorithm, 'AFPToFTool', topPath='AFP/ToF/')
    arrayOneList.defineHistogram('barInTrainID,trainID', title='ToF hit bar vs train {0};barInTrainID;trainID', type='TH2F', path='HitBarvsTrain/',xbins=4,xmin=-0.5,xmax=3.5,ybins=4,ymin=-0.5,ymax=3.5)

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
    #nightly = '/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/user.ladamczy.00361795.calibration_AFP.AODV1_EXT0'
    #file=''
    #file = '/afs/cern.ch/user/l/ladamczy/public/data18_13TeV.00354309.physics_Main.ESD._lb0130._SFO-1._0001.data.r22'
    #file ='/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.merge.AOD.r10258_p3399_tid13243079_00/AOD.13243079._000003.pool.root.1' 
    #file = '/afs/cern.ch/user/l/ladamczy/public/data17_13TeV.00337176.calibration_AFP.AOD.pool.root'
    #file = '/afs/cern.ch/user/l/ladamczy/public/data18_13TeV.00354309.physics_Main.ESD._lb0130._SFO-1._0001.data.r22'

    #ConfigFlags.Input.Files = [nightly+file]
    #ConfigFlags.Input.Files = ['/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_4/aod0.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_4/aod1.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_4/aod2.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_4/aod3.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_4/aod4.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_5/aod0.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_5/aod1.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_5/aod2.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_5/aod3.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_5/aod4.pool.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/new_run_6/aod0.pool.root']
    #ConfigFlags.Input.Files = ['/eos/atlas/atlastier0/tzero/prod/data17_13TeV/physics_Main/00337176/data17_13TeV.00337176.physics_Main.recon.AOD.f871/data17_13TeV.00337176.physics_Main.recon.AOD.f871._lb0142._0006.1']
    ConfigFlags.Input.Files = ['/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/user.ladamczy.00337176.calibration_AFP.AODV1_EXT0/user.ladamczy.21473705.EXT0._000002.xAOD.root','/eos/atlas/atlascerngroupdisk/det-afp/xAODCalibrationStream/2017/user.ladamczy.00337176.calibration_AFP.AODV1_EXT0/user.ladamczy.21473705.EXT0._000003.xAOD.root']
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'AFPOutput46.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    exampleMonitorAcc = Run3AFPExampleMonitoringConfig(ConfigFlags)
    cfg.merge(exampleMonitorAcc)

    cfg.run(100)



