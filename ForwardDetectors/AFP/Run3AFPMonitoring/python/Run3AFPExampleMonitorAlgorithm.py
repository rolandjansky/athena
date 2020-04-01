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

    AFPSiGroup.defineHistogram('lb', title='Luminosity Block;lb;total number of Hits',  path='Global',xbins=1000,xmin=-0.5,xmax=999.5,weight='nSiHits') 
    
    AFPToFGroup.defineHistogram('lb', title='Luminosity Block;lb;total number of Hits',  path='ToF',xbins=1000,xmin=-0.5,xmax=999.5,weight='nTofHits') 
    AFPToFGroup.defineHistogram('numberOfHit_S0', title='Number of hit per bar station 0;total number of Hits',  path='ToF',xbins=3,xmin=-0.5,xmax=2.5)
    AFPToFGroup.defineHistogram('numberOfHit_S3', title='Number of hit per bar station 3;total number of Hits',  path='ToF',xbins=3,xmin=-0.5,xmax=2.5)

    
    for alg in [afpSiLayerAlgorithm]:

       # Using a map of groups
       layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
       combinedList = ['farAside', 'nearAside', 'nearCside', 'farCside']

       array2D = helper.addArray([combinedList,layerList], alg, 'AFPSiLayerTool', topPath = 'Hits')
       array2D.defineHistogram('pixelColIDChip,pixelRowIDChip', title='hitmap for {0} Layer {1}', type='TH2F', path='AFPSiLayer', xbins=80, xmin=0.5, xmax=80.5, ybins=336, ymin=0.5, ymax=336.5)

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
    nightly = ''#/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00348618/data18_13TeV.00348618.physics_Main.recon.AOD.v220/'
    #file = 'data18_13TeV.00348618.physics_Main.recon.AOD.v220._lb0295._0009.1'
    #file = '/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.merge.AOD.r10258_p3399_tid13243079_00/AOD.13243079._000003.pool.root.1' #ToF
    file = '/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.merge.AOD.r10258_p3399_tid13243079_00/AOD.13243079._000005.pool.root.1' #SiT
    #/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.deriv.DAOD_STDM7.r10258_p3399_p4030/DAOD_STDM7.20036794._000007.pool.root.1'
    #/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.recon.AOD.r10258_p3412_r11501/AOD.18508508._000007.pool.root.1'
    #/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00354309/data18_13TeV.00354309.physics_Main.recon.AOD.f946/data18_13TeV.00354309.physics_Main.recon.AOD.f946._lb0130._0001.1'
    #nightly = '/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00357750/data18_13TeV.00357750.physics_Main.recon.AOD.f1041/'
    #file = 'data18_13TeV.00357750.physics_Main.recon.AOD.f1041._lb0105._SFO-7._0103.1'
    #nightly = '/eos/atlas/atlastier0/tzero/prod/data17_13TeV/physics_Main/00337176/data17_13TeV.00337176.physics_Main.recon.AOD.f871/'
    #file = 'data17_13TeV.00337176.physics_Main.recon.AOD.f871._lb0142._0006.1'
    
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

    cfg.run(100) #use cfg.run(20) to only run on first 20 events

