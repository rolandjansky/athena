#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigCaloMonitorAlgorithm.py
@author D. Maximov
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-06-28
@brief Calorimeter trigger python configuration for the Run III AthenaMonitoring package, based on the example by E. Bergeaas Kuutmann
'''

def TrigCaloMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    import math

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigCaloAthMonitorCfg')

    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)

    # Get BunchCrossingCondAlg
    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    result=BunchCrossingCondAlgCfg(inputFlags)


    ################################
    #     HLT_FastCaloEMClusters     #
    ################################



    # Add monitor algorithm
    from AthenaConfiguration.ComponentFactory import CompFactory
    L2CaloEMClustersMonAlg = helper.addAlgorithm(CompFactory.HLTCalo_L2CaloEMClustersMonitor, 'HLT_FastCaloEMClustersMonAlg')

    # Set properties
    L2CaloEMClustersMonAlg.HLTContainer = 'HLT_FastCaloEMClusters'
    L2CaloEMClustersMonAlg.OFFContainer = 'egammaClusters'
    L2CaloEMClustersMonAlg.MonGroupName = 'TrigCaloMonitor'
    L2CaloEMClustersMonAlg.OFFTypes = []
    L2CaloEMClustersMonAlg.HLThighET= 10000.0
    L2CaloEMClustersMonAlg.HLTMinET = -1.0
    L2CaloEMClustersMonAlg.OFFMinET = -1.0
    L2CaloEMClustersMonAlg.MaxDeltaR = 0.04

    # Add group
    L2CaloEMClustersMonGroup = helper.addGroup(L2CaloEMClustersMonAlg,'TrigCaloMonitor','HLT/HLTCalo')

    ########################
    #     HLT_Clusters     #
    ########################

    # Declare HLT histograms
    hist_path='HLT_FastCaloEMClusters/HLT_Clusters'
    L2CaloEMClustersMonGroup.defineHistogram('HLT_num',title='Number of HLT Clusters; Num Clusters; Entries',
                                path=hist_path,xbins=51,xmin=-0.5,xmax=50.5)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_phi',title='Number of HLT Clusters; #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et',title='HLT Clusters E_{T}; E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta',title='HLT Clusters #eta; #eta; Entries',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_phi',title='HLT Cluster #phi; #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_size',title='HLT Cluster Size; Number of Cells; Entries',
                                path=hist_path,xbins=91,xmin=-10.0,xmax=1810.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,HLT_et;HLT_et_vs_BC',title='HLT Clusters E_{T} vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)

    # Declare high-ET HLT histograms
    L2CaloEMClustersMonGroup.defineHistogram('HLT_barrel_high_et_num',title='Number of high-E_{T} HLT Clusters; Num Clusters; Entries',
                                path=hist_path,xbins=101,xmin=-0.5,xmax=100.5)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_phi;HLT_barrel_high_et_phi_vs_HLT_barrel_high_et_eta',cutmask="HLT_barrel_high_et",title='Number of high-E_{T} HLT Clusters; #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-2.6,xmax=2.6,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et;HLT_barrel_high_et_et',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Clusters E_{T}; E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta;HLT_barrel_high_et_eta',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Clusters #eta; #eta; Entries',
                                path=hist_path,xbins=50,xmin=-2.6,xmax=2.6)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_phi;HLT_barrel_high_et_phi',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Cluster #phi; #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_size;HLT_barrel_high_et_size',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Cluster Size; Number of Cells; Entries',
                                path=hist_path,xbins=91,xmin=-10.0,xmax=1810.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,HLT_et;HLT_barrel_high_et_vs_BC',cutmask="HLT_barrel_high_et",title='hight-E_{T} HLT Clusters E_{T} vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)

    ########################
    #     OFF_Clusters     #
    ########################

    # Declare OFF histograms
    hist_path='HLT_FastCaloEMClusters/OFF_Clusters'
    L2CaloEMClustersMonGroup.defineHistogram('OFF_num',title='Number of OFF Clusters; Num Clusters; Entries',
                                path=hist_path,xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta,OFF_phi',title='Number of OFF Clusters; #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_et',title='OFF Clusters E_{T}; E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta',title='OFF Clusters #eta; #eta; Entries',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_phi',title='OFF Cluster #phi; #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_type',title='OFF Cluster Type; Size Enum; Entries',
                                path=hist_path,xbins=16,xmin=0.5,xmax=16.5)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,OFF_et;OFF_et_vs_BC',title='OFF Clusters E_{T} vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)

    ########################
    #  HLT matched to OFF  #
    ########################

    # Declare HLT matched HLT vs. OFF cluster histograms
    hist_path='HLT_FastCaloEMClusters/HLT_Matched_to_OFF'
    L2CaloEMClustersMonGroup.defineHistogram('HLT_matched_fraction',title='Fraction of HLT clusters matched to HLT clusters; Matched fraction; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=1.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_minimum_delta_r',title='HLT vs OFF Cluster #DeltaR; #DeltaR; Entries',
                                path=hist_path,xbins=50,xmin=0.0,xmax=0.1)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_minimum_delta_eta',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster #Delta#eta; #eta_{_{OFF}} -  #eta_{_{HLT}}; Entries',
                                path=hist_path,xbins=50,xmin=-0.2,xmax=0.2)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_minimum_delta_phi',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster #Delta#phi; #phi_{_{OFF}} -  #phi_{_{HLT}}; Entries',
                                path=hist_path,xbins=50,xmin=-0.01,xmax=0.09)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et,OFF_match_et;HLT_with_OFF_match_et_vs_OFF_match_et',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster E_{T}; E_{T_{OFF}} [GeV]; E_{T_{HLT}} [GeV]', type='TH2F',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0, ybins=100,ymin=0.0,ymax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_vs_OFF_resolution',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster #DeltaE_{T} / E_{T}; E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} [%]; Entries',
                                path=hist_path,xbins=100,xmin=-40.0,xmax=40.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_et',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; E_{T,OFF} [GeV]; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=hist_path,xbins=20,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_phi,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_phi',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #phi_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=hist_path,xbins=16,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_phi,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_phi_vs_HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta; #phi', type='TProfile2D',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)

    # Declare OFF histograms with HLT matches
    L2CaloEMClustersMonGroup.defineHistogram('HLT_with_OFF_match_num',title='Number of HLT Clusters (With OFF Matches); Num Clusters; Entries',
                                path=hist_path,xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_phi;HLT_with_OFF_match_eta_vs_HLT_with_OFF_match_phi',cutmask='HLT_with_OFF_match',title='Number of HLT Clusters (With OFF Matches); #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et;HLT_with_OFF_match_et',cutmask='HLT_with_OFF_match',title='HLT Clusters E_{T} (With OFF Matches); E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta;HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='HLT Clusters #eta (With OFF Matches); #eta; Entries',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_phi;HLT_with_OFF_match_phi',cutmask='HLT_with_OFF_match',title='HLT Cluster #phi (With OFF Matches); #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,HLT_et;HLT_with_OFF_match_et_vs_BC',cutmask='HLT_with_OFF_match',title='HLT Clusters E_{T} (With OFF Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)

    # Declare OFF histograms without HLT matches
    L2CaloEMClustersMonGroup.defineHistogram('HLT_no_OFF_match_num',title='Number of HLT Clusters (No OFF Matches); Num Clusters; Entries',
                                path=hist_path,xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta,HLT_phi;HLT_no_OFF_match_eta_vs_HLT_no_OFF_match_phi',cutmask='HLT_no_OFF_match',title='Number of HLT Clusters (No OFF Matches); #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_et;HLT_no_OFF_match_et',cutmask='HLT_no_OFF_match',title='HLT Clusters E_{T} (No OFF Matches); E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_eta;HLT_no_OFF_match_eta',cutmask='HLT_no_OFF_match',title='HLT Clusters #eta (No OFF Matches); #eta; Entries',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_phi;HLT_no_OFF_match_phi',cutmask='HLT_no_OFF_match',title='HLT Cluster #phi (No OFF Matches); #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,HLT_et;HLT_no_OFF_match_et_vs_BC',cutmask='HLT_no_OFF_match',title='HLT Clusters E_{T} (No OFF Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)

    ########################
    #  OFF matched to HLT  #
    ########################

    # Declare OFF matched HLT vs. OFF cluster histograms
    hist_path='HLT_FastCaloEMClusters/OFF_Matched_to_HLT'
    L2CaloEMClustersMonGroup.defineHistogram('OFF_matched_fraction',title='Fraction of OFF clusters matched to HLT clusters; Matched fraction; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=1.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_vs_HLT_minimum_delta_r',title='HLT vs OFF Cluster #DeltaR; #DeltaR; Entries',
                                path=hist_path,xbins=50,xmin=0.0,xmax=0.1)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_vs_HLT_minimum_delta_eta',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster #Delta#eta; #eta_{_{OFF}} -  #eta_{_{HLT}}; Entries',
                                path=hist_path,xbins=50,xmin=-0.2,xmax=0.2)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_vs_HLT_minimum_delta_phi',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster #Delta#phi; #phi_{_{OFF}} -  #phi_{_{HLT}}; Entries',
                                path=hist_path,xbins=50,xmin=-0.01,xmax=0.09)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_et,HLT_match_et;HLT_match_et_vs_OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster E_{T}; E_{T_{OFF}} [GeV]; E_{T_{HLT}} [GeV]', type='TH2F',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0, ybins=100,ymin=0.0,ymax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_vs_HLT_resolution',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster #DeltaE_{T} / E_{T}; E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} [%]; Entries',
                                path=hist_path,xbins=100,xmin=-40.0,xmax=40.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_et,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; E_{T,OFF} [GeV]; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=hist_path,xbins=20,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_phi,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_phi',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #phi_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=hist_path,xbins=16,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta,OFF_phi,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_phi_vs_OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta; #phi', type='TProfile2D',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)

    # Declare OFF histograms with HLT matches
    L2CaloEMClustersMonGroup.defineHistogram('OFF_with_HLT_match_num',title='Number of OFF Clusters (With HLT Matches); Num Clusters; Entries',
                                path=hist_path,xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta,OFF_phi;OFF_with_HLT_match_phi_vs_OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='Number of OFF Clusters (With HLT Matches); #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_et;OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='OFF Clusters E_{T} (With HLT Matches); E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta;OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='OFF Clusters #eta (With HLT Matches); #eta; Entries',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_phi;OFF_with_HLT_match_phi',cutmask='OFF_with_HLT_match',title='OFF Cluster #phi (With HLT Matches); #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_type;OFF_with_HLT_match_type',cutmask='OFF_with_HLT_match',title='OFF Cluster Type (With HLT Matches); Size Enum; Entries',
                                path=hist_path,xbins=16,xmin=0.5,xmax=16.5)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,OFF_et;OFF_with_HLT_match_et_vs_BC',cutmask='OFF_with_HLT_match',title='OFF Clusters E_{T} (With HLT Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)

    # Declare OFF histograms without HLT matches
    L2CaloEMClustersMonGroup.defineHistogram('OFF_no_HLT_match_num',title='Number of OFF Clusters (No HLT Matches); Num Clusters; Entries',
                                path=hist_path,xbins=101,xmin=-1.0,xmax=201.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta,OFF_phi;OFF_no_HLT_match_phi_vs_OFF_no_HLT_match_eta',cutmask='OFF_no_HLT_match',title='Number of OFF Clusters (No HLT Matches); #eta; #phi; ', type='TH2F',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_et;OFF_no_HLT_match_et',cutmask='OFF_no_HLT_match',title='OFF Clusters E_{T} (No HLT Matches); E_{T} [GeV]; Entries',
                                path=hist_path,xbins=100,xmin=0.0,xmax=100.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_eta;OFF_no_HLT_match_eta',cutmask='OFF_no_HLT_match',title='OFF Clusters #eta (No HLT Matches); #eta; Entries',
                                path=hist_path,xbins=50,xmin=-5.0,xmax=5.0)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_phi;OFF_no_HLT_match_phi',cutmask='OFF_no_HLT_match',title='OFF Cluster #phi (No HLT Matches); #phi; Entries',
                                path=hist_path,xbins=64,xmin=-math.pi,xmax=math.pi)
    L2CaloEMClustersMonGroup.defineHistogram('OFF_type;OFF_no_HLT_match_type',cutmask='OFF_no_HLT_match',title='OFF Cluster Type (No HLT Matches); Size Enum; Entries',
                                path=hist_path,xbins=16,xmin=0.5,xmax=16.5)
    L2CaloEMClustersMonGroup.defineHistogram('HLT_bc,OFF_et;OFF_no_HLT_match_et_vs_BC',cutmask='OFF_no_HLT_match',title='OFF Clusters E_{T} (No HLT Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=hist_path,xbins=21,xmin=-0.5,xmax=20.5)



    ################################
    #     HLT_TopoCaloClusters     #
    ################################



    # Add monitor algorithm
    TopoCaloClustersFSMonAlg = helper.addAlgorithm(CompFactory.HLTCalo_TopoCaloClustersMonitor, 'HLT_TopoCaloClustersFSMonAlg')
    TopoCaloClustersRoIMonAlg = helper.addAlgorithm(CompFactory.HLTCalo_TopoCaloClustersMonitor, 'HLT_TopoCaloClustersRoIMonAlg')
    TopoCaloClustersLCMonAlg = helper.addAlgorithm(CompFactory.HLTCalo_TopoCaloClustersMonitor, 'HLT_TopoCaloClustersLCMonAlg')
    TopoCaloClustersFSMonAlg.HLTContainer = 'HLT_TopoCaloClustersFS'
    TopoCaloClustersRoIMonAlg.HLTContainer = 'HLT_TopoCaloClustersRoI'
    TopoCaloClustersLCMonAlg.HLTContainer = 'HLT_TopoCaloClustersLC'

    # Loop over all three monitoring algorithms
    algs = [TopoCaloClustersFSMonAlg, TopoCaloClustersRoIMonAlg, TopoCaloClustersLCMonAlg]
    path_names = ['HLT_TopoCaloClustersFS', 'HLT_TopoCaloClustersRoI', 'HLT_TopoCaloClustersLC']
    TopoCaloClustersMonGroup = []

    for i in range(len(algs)):

        # Set properties
        algs[i].OFFContainer = 'CaloCalTopoClusters'
        algs[i].MonGroupName = 'TrigCaloMonitor'
        algs[i].HLTTypes = []
        algs[i].OFFTypes = []
        algs[i].HLThighET= 5000.0
        algs[i].HLTMinET = 500.0
        algs[i].OFFMinET = 500.0
        algs[i].MatchType = False
        algs[i].MaxDeltaR = 0.04

        # Add group
        TopoCaloClustersMonGroup.append(helper.addGroup(algs[i], 'TrigCaloMonitor','HLT/HLTCalo'))

        ########################
        #     HLT_Clusters     #
        ########################

        # Declare HLT histograms
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_num',title='Number of HLT Clusters; Num Clusters; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=101,xmin=-10.0,xmax=2010.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta,HLT_phi',title='Number of HLT Clusters; #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/HLT_Clusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et',title='HLT Clusters E_{T}; E_{T} [GeV]; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta',title='HLT Clusters #eta; #eta; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=50,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_phi',title='HLT Cluster #phi; #phi; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_type',title='HLT Cluster Type; Size Enum; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_time',title='HLT Cluster time; time; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=60,xmin=-10.0,xmax=10.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et,HLT_time',title='Number of HLT Clusters; E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/HLT_Clusters',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_size',title='HLT Cluster Size; Number of Cells; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=91,xmin=-10.0,xmax=1810.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,HLT_et;HLT_et_vs_BC',title='HLT Clusters E_{T} vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/HLT_Clusters',xbins=21,xmin=-0.5,xmax=20.5)

        # Declare high-ET HLT histograms
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_barrel_high_et_num',title='Number of high-E_{T} HLT Clusters; Num Clusters; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=101,xmin=-0.5,xmax=100.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta,HLT_phi;HLT_barrel_high_et_phi_vs_HLT_barrel_high_et_eta',cutmask="HLT_barrel_high_et",title='Number of high-E_{T} HLT Clusters; #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/HLT_Clusters',xbins=50,xmin=-2.6,xmax=2.6,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et;HLT_barrel_high_et_et',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Clusters E_{T}; E_{T} [GeV]; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta;HLT_barrel_high_et_eta',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Clusters #eta; #eta; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=50,xmin=-2.6,xmax=2.6)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_phi;HLT_barrel_high_et_phi',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Cluster #phi; #phi; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_type;HLT_barrel_high_et_type',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Cluster Type; Size Enum; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_time;HLT_barrel_high_et_time',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Cluster time; time; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=60,xmin=-10.0,xmax=10.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et,HLT_time;HLT_barrel_high_et_time_vs_HLT_barrel_high_et_et',cutmask="HLT_barrel_high_et",title='Number of high-E_{T} HLT Clusters; E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/HLT_Clusters',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_size;HLT_barrel_high_et_size',cutmask="HLT_barrel_high_et",title='high-E_{T} HLT Cluster Size; Number of Cells; Entries',
                                path=path_names[i]+'/HLT_Clusters',xbins=91,xmin=-10.0,xmax=1810.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,HLT_et;HLT_barrel_high_et_vs_BC',cutmask="HLT_barrel_high_et",title='hight-E_{T} HLT Clusters E_{T} vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/HLT_Clusters',xbins=21,xmin=-0.5,xmax=20.5)

        ########################
        #     OFF_Clusters     #
        ########################

        # Declare OFF histograms
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_num',title='Number of OFF Clusters; Num Clusters; Entries',
                                path=path_names[i]+'/OFF_Clusters',xbins=101,xmin=-10.0,xmax=2010.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta,OFF_phi',title='Number of OFF Clusters; #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/OFF_Clusters',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et',title='OFF Clusters E_{T}; E_{T} [GeV]; Entries',
                                path=path_names[i]+'/OFF_Clusters',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta',title='OFF Clusters #eta; #eta; Entries',
                                path=path_names[i]+'/OFF_Clusters',xbins=50,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_phi',title='OFF Cluster #phi; #phi; Entries',
                                path=path_names[i]+'/OFF_Clusters',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_time',title='OFF Cluster time; time; Entries',
                                path=path_names[i]+'/OFF_Clusters',xbins=75,xmin=-25.0,xmax=25.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et,OFF_time',title='Number of OFF Clusters; E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/OFF_Clusters',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_type',title='HLT Cluster Type; Size Enum; Entries',
                                path=path_names[i]+'/OFF_Clusters',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,OFF_et;OFF_et_vs_BC',title='OFF Clusters E_{T} vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/OFF_Clusters',xbins=21,xmin=-0.5,xmax=20.5)

        ########################
        #  HLT matched to OFF  #
        ########################

        # Declare HLT matched HLT vs. OFF cluster histograms
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_matched_fraction',title='Fraction of HLT clusters matched to OFF clusters; Matched fraction; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=100,xmin=0.0,xmax=1.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_vs_OFF_minimum_delta_r',title='HLT vs OFF Cluster #DeltaR; #DeltaR; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=0.0,xmax=0.1)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_vs_OFF_delta_eta',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster #Delta#eta; #eta_{_{OFF}} -  #eta_{_{HLT}}; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=-0.2,xmax=0.2)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_vs_OFF_delta_phi',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster #Delta#phi; #phi_{_{OFF}} -  #phi_{_{HLT}}; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=0.0,xmax=0.02)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_vs_OFF_delta_time',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster time; Time; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=80,xmin=-20.0,xmax=20.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et,OFF_match_et;OFF_match_et_vs_HLT_with_OFF_match_et',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster E_{T}; E_{T_{OFF}} [GeV]; E_{T_{HLT}} [GeV]', type='TH2F',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=100,xmin=0.0,xmax=100.0, ybins=100,ymin=0.0,ymax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_vs_OFF_resolution',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster #DeltaE_{T} / E_{T}; E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} [%]; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=100,xmin=-60.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_et',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; E_{T,HLT} [GeV]; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=20,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_phi,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_phi',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #phi_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=16,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta,HLT_phi,HLT_vs_OFF_resolution;HLT_vs_OFF_resolution_vs_HLT_with_OFF_match_phi_vs_HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta; #phi', type='TProfile2D',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)

        # Declare HLT histograms with OFF matches
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_with_OFF_match_num',title='Number of HLT Clusters (With OFF Matches); Num Clusters; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=101,xmin=-10.0,xmax=2010.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta,HLT_phi;HLT_with_OFF_match_phi_vs_HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='Number of HLT Clusters (With OFF Matches); #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et;HLT_with_OFF_match_et',cutmask='HLT_with_OFF_match',title='HLT Clusters E_{T} (With OFF Matches); E_{T} [GeV]; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta;HLT_with_OFF_match_eta',cutmask='HLT_with_OFF_match',title='HLT Clusters #eta (With OFF Matches); #eta; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_phi;HLT_with_OFF_match_phi',cutmask='HLT_with_OFF_match',title='HLT Cluster #phi (With OFF Matches); #phi; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_time;HLT_with_OFF_match_time',cutmask='HLT_with_OFF_match',title='HLT Cluster time (With OFF Matches); time; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=75,xmin=-25.0,xmax=25.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et,HLT_time;HLT_with_OFF_match_time_vs_HLT_with_OFF_match_et',cutmask='HLT_with_OFF_match',title='Number of HLT Clusters (With OFF Matches); E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_type;HLT_with_OFF_match_type',cutmask='HLT_with_OFF_match',title='HLT Cluster Type (With OFF Matches); Size Enum; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,HLT_et;HLT_with_OFF_match_et_vs_BC',cutmask='HLT_with_OFF_match',title='HLT Clusters E_{T} (With OFF Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=21,xmin=-0.5,xmax=20.5)

        # Declare HLT histograms without OFF matches
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_no_OFF_match_num',title='Number of HLT Clusters (No OFF Matches); Num Clusters; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=101,xmin=-10.0,xmax=2010.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta,HLT_phi;HLT_no_OFF_match_phi_vs_HLT_no_OFF_match_eta',cutmask='HLT_no_OFF_match',title='Number of HLT Clusters (No OFF Matches); #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et;HLT_no_OFF_match_et',cutmask='HLT_no_OFF_match',title='HLT Clusters E_{T} (No OFF Matches); E_{T} [GeV]; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_eta;HLT_no_OFF_match_eta',cutmask='HLT_no_OFF_match',title='HLT Clusters #eta (No OFF Matches); #eta; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_phi;HLT_no_OFF_match_phi',cutmask='HLT_no_OFF_match',title='HLT Cluster #phi (No OFF Matches); #phi; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_time;HLT_no_OFF_match_time',cutmask='HLT_no_OFF_match',title='HLT Cluster time (No OFF Matches); time; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=75,xmin=-25.0,xmax=25.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_et,HLT_time;HLT_no_OFF_match_time_vs_HLT_no_OFF_match_et',cutmask='HLT_no_OFF_match',title='Number of HLT Clusters (No OFF Matches); E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_type;HLT_no_OFF_match_type',cutmask='HLT_no_OFF_match',title='HLT Cluster Type (No OFF Matches); Size Enum; Entries',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,HLT_et;HLT_no_OFF_match_et_vs_BC',cutmask='HLT_no_OFF_match',title='HLT Clusters E_{T} (No OFF Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/HLT_Matched_to_OFF',xbins=21,xmin=-0.5,xmax=20.5)

        ########################
        #  OFF matched to HLT  #
        ########################

        # Declare OFF matched HLT vs. OFF cluster histograms
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_matched_fraction',title='Fraction of OFF clusters matched to HLT clusters; Matched fraction; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=100,xmin=0.0,xmax=1.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_vs_HLT_minimum_delta_r',title='HLT vs OFF Cluster #DeltaR; #DeltaR; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=0.0,xmax=0.1)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_vs_HLT_delta_eta',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster #Delta#eta; #eta_{_{OFF}} -  #eta_{_{HLT}}; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=-0.2,xmax=0.2)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_vs_HLT_delta_phi',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster #Delta#phi; #phi_{_{OFF}} -  #phi_{_{HLT}}; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=0.0,xmax=0.02)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_vs_HLT_delta_time',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster time; Time; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=80,xmin=-20.0,xmax=20.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et,HLT_match_et;HLT_match_et_vs_OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster E_{T}; E_{T_{OFF}} [GeV]; E_{T_{HLT}} [GeV]', type='TH2F',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=100,xmin=0.0,xmax=100.0, ybins=100,ymin=0.0,ymax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_vs_HLT_resolution',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster #DeltaE_{T} / E_{T}; E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} [%]; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=100,xmin=-60.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; E_{T,OFF} [GeV]; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=20,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_phi,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_phi',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #phi_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=16,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta,OFF_phi,OFF_vs_HLT_resolution;OFF_vs_HLT_resolution_vs_OFF_with_HLT_match_phi_vs_OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta; #phi', type='TProfile2D',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)

        # Declare OFF histograms with HLT matches
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_with_HLT_match_num',title='Number of OFF Clusters (With HLT Matches); Num Clusters; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=101,xmin=-10.0,xmax=2010.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta,OFF_phi;OFF_with_HLT_match_phi_vs_OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='Number of OFF Clusters (With HLT Matches); #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et;OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='OFF Clusters E_{T} (With HLT Matches); E_{T} [GeV]; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta;OFF_with_HLT_match_eta',cutmask='OFF_with_HLT_match',title='OFF Clusters #eta (With HLT Matches); #eta; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_phi;OFF_with_HLT_match_phi',cutmask='OFF_with_HLT_match',title='OFF Cluster #phi (With HLT Matches); #phi; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_time;OFF_with_HLT_match_time',cutmask='OFF_with_HLT_match',title='OFF Cluster time (With HLT Matches); time; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=75,xmin=-25.0,xmax=25.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et,OFF_time;OFF_with_HLT_match_time_vs_OFF_with_HLT_match_et',cutmask='OFF_with_HLT_match',title='Number of OFF Clusters (With HLT Matches); E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_type;OFF_with_HLT_match_type',cutmask='OFF_with_HLT_match',title='OFF Cluster Type (With HLT Matches); Size Enum; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,OFF_et;OFF_with_HLT_match_et_vs_BC',cutmask='OFF_with_HLT_match',title='OFF Clusters E_{T} (With HLT Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=21,xmin=-0.5,xmax=20.5)

        # Declare OFF histograms without HLT matches
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_no_HLT_match_num',title='Number of OFF Clusters (No HLT Matches); Num Clusters; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=101,xmin=-10.0,xmax=2010.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta,OFF_phi;OFF_no_HLT_match_phi_vs_OFF_no_HLT_match_eta',cutmask='OFF_no_HLT_match',title='Number of OFF Clusters (No HLT Matches); #eta; #phi; ', type='TH2F',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=-5.0,xmax=5.0,ybins=64,ymin=-math.pi,ymax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et;OFF_no_HLT_match_et',cutmask='OFF_no_HLT_match',title='OFF Clusters E_{T} (No HLT Matches); E_{T} [GeV]; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=100,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_eta;OFF_no_HLT_match_eta',cutmask='OFF_no_HLT_match',title='OFF Clusters #eta (No HLT Matches); #eta; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=-5.0,xmax=5.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_phi;OFF_no_HLT_match_phi',cutmask='OFF_no_HLT_match',title='OFF Cluster #phi (No HLT Matches); #phi; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=64,xmin=-math.pi,xmax=math.pi)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_time;OFF_no_HLT_match_time',cutmask='OFF_no_HLT_match',title='OFF Cluster time (No HLT Matches); time; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=75,xmin=-25.0,xmax=25.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_et,OFF_time;OFF_no_HLT_match_time_vs_OFF_no_HLT_match_et',cutmask='OFF_no_HLT_match',title='Number of OFF Clusters (No HLT Matches); E_{T} [GeV]; Time', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=50,xmin=0.0,xmax=100.0)
        TopoCaloClustersMonGroup[i].defineHistogram('OFF_type;OFF_no_HLT_match_type',cutmask='OFF_no_HLT_match',title='OFF Cluster Type (No HLT Matches); Size Enum; Entries',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=16,xmin=0.5,xmax=16.5)
        TopoCaloClustersMonGroup[i].defineHistogram('HLT_bc,OFF_et;OFF_no_HLT_match_et_vs_BC',cutmask='OFF_no_HLT_match',title='OFF Clusters E_{T} (No HLT Matches) vs BC; BCs from front of bunch train; <E_{T}> [GeV]', type='TProfile',
                                path=path_names[i]+'/OFF_Matched_to_HLT',xbins=21,xmin=-0.5,xmax=20.5)

    result.merge(helper.result())
    return result
    
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
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigCaloMonitorAcc = TrigCaloMonConfig(ConfigFlags)
    cfg.merge(trigCaloMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigCaloMonitorAcc.getEventAlgo('TrigCaloMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
