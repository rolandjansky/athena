#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigCaloMonitorAlgorithm.py
@author D. Maximov
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-06-28
@brief Calorimeter trigger python configuration for the Run III AthenaMonitoring package, based on the example by E. Bergeaas Kuutmann
'''
#from AthenaCommon.Logging import logging
#log_trigeg = logging.getLogger( 'TrigCaloMonitorAlgorithm' )

#from TrigCaloMonitoring.TrigCaloMonitoringConfigRun3 import HLTCaloMonitoringTool

def TrigCaloMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigCaloAthMonitorCfg')

    from TrigCaloMonitoring.TrigCaloMonitoringConf import TrigCaloMonitorAlgorithm
    trigCaloMonAlg = helper.addAlgorithm(TrigCaloMonitorAlgorithm,'TrigCaloMonAlg')

#    histogramsRoot = '/HLT/HLTCalo'

#    montoolCfg = HLTCaloMonitoringTool()

#    montoolCfg.bookHistograms(histogramsRoot, helper)

    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)

    CaloMonGroup = helper.addGroup(trigCaloMonAlg,'TrigCaloMonitor','HLT/HLTCalo')

    CaloMonGroup.defineHistogram('test',title='Test;testx;Events',path='Test',xbins=100,xmin=0.0,xmax=100.)
    CaloMonGroup.defineHistogram('lumiPerBCID',title='Luminosity;L/BCID;Events',
                            path='Test',xbins=10,xmin=0.0,xmax=10.0)
    CaloMonGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
                            path='Test',xbins=1000,xmin=-0.5,xmax=999.5)

    ################################
    #     HLT_L2CaloEMClusters     #
    ################################

    # Add monitor algorithm
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCalo_L2CaloEMClustersMonitor
    L2CaloEMClustersMonAlg = helper.addAlgorithm(HLTCalo_L2CaloEMClustersMonitor, 'HLT_L2CaloEMClustersMonAlg')

    # Set properties
    L2CaloEMClustersMonAlg.HLTContainer = 'HLT_L2CaloEMClusters'
    L2CaloEMClustersMonAlg.OFFContainer = 'egammaClusters'
    L2CaloEMClustersMonAlg.MonGroupName = 'TrigCaloMonitor'
    L2CaloEMClustersMonAlg.OFFTypes = []
    L2CaloEMClustersMonAlg.HLTMinET = -1.0
    L2CaloEMClustersMonAlg.OFFMinET = -1.0
    L2CaloEMClustersMonAlg.MaxDeltaR = 0.04

    # Add group
    L2CaloEMClustersMonGroup = helper.addGroup(L2CaloEMClustersMonAlg,'TrigCaloMonitor','HLT/HLTCalo')

    # Declare HLT histograms
    L2CaloEMClustersMonGroup.defineHistogram('HLT_num',title='Number of HLT Clusters; Num Clusters; Entries',
				path='HLT_L2CaloEMClusters',xbins=51,xmin=-0.5,xmax=50.5)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_phi',title='Number of HLT Clusters; #eta; #phi; ', type='TH2F',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-3.14153,ymax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et',title='HLT Clusters E_{T}; E_{T} [GeV]; Entries',
				path='HLT_L2CaloEMClusters',xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta',title='HLT Clusters #eta; #eta; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_phi',title='HLT Cluster #phi; #phi; Entries',
				path='HLT_L2CaloEMClusters',xbins=64,xmin=-3.14153,xmax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_size',title='HLT Cluster Size; Number of Cells; Entries',
				path='HLT_L2CaloEMClusters',xbins=91,xmin=-10.0,xmax=1810.0)

    # Declare OFF histograms
    L2CaloEMClustersMonGroup.defineHistogram('OFF_num',title='Number of OFF Clusters; Num Clusters; Entries',
				path='HLT_L2CaloEMClusters',xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta,OFF_phi',title='Number of OFF Clusters; #eta; #phi; ', type='TH2F',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-3.14153,ymax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_et',title='OFF Clusters E_{T}; E_{T} [GeV]; Entries',
				path='HLT_L2CaloEMClusters',xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta',title='OFF Clusters #eta; #eta; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_phi',title='OFF Cluster #phi; #phi; Entries',
				path='HLT_L2CaloEMClusters',xbins=64,xmin=-3.14153,xmax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_type',title='OFF Cluster Type; Size Enum; Entries',
				path='HLT_L2CaloEMClusters',xbins=16,xmin=0.5,xmax=15.5)

    # Declare OFF histograms without HLT matches
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_num',title='Number of OFF Clusters (No HLT Matches); Num Clusters; Entries',
				path='HLT_L2CaloEMClusters',xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_eta,OFF_no_HLT_match_phi',title='Number of OFF Clusters (No HLT Matches); #eta; #phi; ', type='TH2F',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-3.14153,ymax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_et',title='OFF Clusters E_{T} (No HLT Matches); E_{T} [GeV]; Entries',
				path='HLT_L2CaloEMClusters',xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_eta',title='OFF Clusters #eta (No HLT Matches); #eta; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_phi',title='OFF Cluster #phi (No HLT Matches); #phi; Entries',
				path='HLT_L2CaloEMClusters',xbins=64,xmin=-3.14153,xmax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_type',title='OFF Cluster Type (No HLT Matches); Size Enum; Entries',
				path='HLT_L2CaloEMClusters',xbins=16,xmin=0.5,xmax=15.5)

    # Declare OFF histograms with HLT matches
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_num',title='Number of OFF Clusters (With HLT Matches); Num Clusters; Entries',
				path='HLT_L2CaloEMClusters',xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_eta,OFF_with_HLT_match_phi',title='Number of OFF Clusters (With HLT Matches); #eta; #phi; ', type='TH2F',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-3.14153,ymax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_et',title='OFF Clusters E_{T} (With HLT Matches); E_{T} [GeV]; Entries',
				path='HLT_L2CaloEMClusters',xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_eta',title='OFF Clusters #eta (With HLT Matches); #eta; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_phi',title='OFF Cluster #phi (With HLT Matches); #phi; Entries',
				path='HLT_L2CaloEMClusters',xbins=64,xmin=-3.14153,xmax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_type',title='OFF Cluster Type (With HLT Matches); Size Enum; Entries',
				path='HLT_L2CaloEMClusters',xbins=16,xmin=0.5,xmax=15.5)

    # Declare HLT clusters vs. OFF clusters histograms
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_minimum_delta_r',title='HLT vs OFF Cluster #DeltaR; #DeltaR; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=0.0,xmax=0.1)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_minimum_delta_eta',title='HLT vs OFF Cluster #Delta#eta; #eta_{_{OFF}} -  #eta_{_{HLT}}; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-0.2,xmax=0.2)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_minimum_delta_phi',title='HLT vs OFF Cluster #Delta#phi; #phi_{_{OFF}} -  #phi_{_{HLT}}; Entries',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=0.0,xmax=0.02)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_et,HLT_match_et',title='HLT vs OFF Cluster E_{T}; #E_{T_{OFF}} [GeV]; E_{T_{HLT}} [GeV]', type='TH2F',
				path='HLT_L2CaloEMClusters',xbins=100,xmin=0.0,xmax=100.0, ybins=100,ymin=0.0,ymax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_resolution',title='HLT vs OFF Cluster #DeltaE_{T} / E_{T}; E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}}; Entries',
				path='HLT_L2CaloEMClusters',xbins=100,xmin=-20.0,xmax=20.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_et,HLT_vs_OFF_resolution',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
				path='HLT_L2CaloEMClusters',xbins=20,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_eta,HLT_vs_OFF_resolution',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
				path='HLT_L2CaloEMClusters',xbins=20,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_phi,HLT_vs_OFF_resolution',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #phi_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
				path='HLT_L2CaloEMClusters',xbins=16,xmin=-3.14153,xmax=3.14153)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_eta,OFF_with_HLT_match_phi,HLT_vs_OFF_resolution',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta; #phi', type='TProfile2D',
				path='HLT_L2CaloEMClusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-3.14153,ymax=3.14153)

    return helper.result()
    
if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Input
    file = '/afs/cern.ch/work/j/joheinri/public/TestAOD/AOD.pool.root'
    ConfigFlags.Input.Files = [file]
    ConfigFlags.Input.isMC = True

    # Output
    ConfigFlags.Output.HISTFileName = 'TrigCaloMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigCaloMonitorAcc = TrigCaloMonConfig(ConfigFlags)
    cfg.merge(trigCaloMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigCaloMonitorAcc.getEventAlgo('TrigCaloMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
