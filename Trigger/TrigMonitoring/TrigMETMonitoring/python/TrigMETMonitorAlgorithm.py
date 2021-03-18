#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigMETMonitoringAlgorithm.py
@author K. Hamano
@author G. Gallardo
@date 2019-05-13
@brief MET Trigger python configuration for Run 3 AthenaMonitoring package

'''


def TrigMETMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMETAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #The added algorithm must exist as a .h file 

    from AthenaConfiguration.ComponentFactory import CompFactory
    TrigMETMonAlg = helper.addAlgorithm(CompFactory.TrigMETMonitorAlgorithm,'TrigMETMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    TrigMETMonChain1Alg = helper.addAlgorithm(CompFactory.TrigMETMonitorAlgorithm,'TrigMETMonChain1Alg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # expertTrigMETMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    # TrigMETMonAlg.TriggerChain = 'HLT_xe30_cell_L1XE10'
    # without filters, all events are processed.
    TrigMETMonChain1Alg.TriggerChain = 'HLT_xe65_cell_L1XE50'
    
    ### check Run2 or Run3 MT
    mt_chains = True
    if ( inputFlags.Trigger.EDMVersion < 3 ) :
      mt_chains = False

    ### container name selection
    if mt_chains: # these are temporary, needs to be changed
      TrigMETMonAlg.hlt_electron_key = 'HLT_egamma_Electrons'
      TrigMETMonAlg.hlt_muon_key = 'HLT_MuonsCB_RoI'
      TrigMETMonAlg.offline_met_key = 'MET_EMTopo'
      TrigMETMonAlg.hlt_pfsum_key = 'HLT_MET_pfsum'
    else:
      TrigMETMonAlg.hlt_electron_key = 'HLT_xAOD__ElectronContainer_egamma_Electrons'
      TrigMETMonAlg.hlt_muon_key = 'HLT_xAOD__MuonContainer_MuonEFInfo'
      TrigMETMonAlg.offline_met_key = 'MET_Reference_AntiKt4LCTopo'
      TrigMETMonAlg.l1_jnc_key = 'LVL1EnergySumRoI'
      TrigMETMonAlg.l1_jrho_key = 'LVL1EnergySumRoI'
      TrigMETMonAlg.l1_gnc_key = 'LVL1EnergySumRoI'
      TrigMETMonAlg.l1_grho_key = 'LVL1EnergySumRoI'
      TrigMETMonAlg.l1_gjwoj_key = 'LVL1EnergySumRoI'
      TrigMETMonAlg.l1_gpufit_key = 'LVL1EnergySumRoI'
      TrigMETMonAlg.hlt_cell_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET'
      TrigMETMonAlg.hlt_mt_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht'
      TrigMETMonAlg.hlt_tc_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_tc_em_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_tcpufit_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC'
      TrigMETMonAlg.hlt_trkmht_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_pfsum_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_pfsum_vssk_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_pfsum_cssk_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_pfopufit_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_cvfpufit_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_mhtpufit_pf_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
      TrigMETMonAlg.hlt_mhtpufit_em_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'

    ### chain name selection
    ### these are default chains ######
      #TrigMETMonAlg.L1Chain02 = 'L1_XE50'
      #TrigMETMonAlg.L1Chain02 = 'L1_jXENC50'
      #TrigMETMonAlg.L1Chain03 = 'L1_jXERHO50'
      #TrigMETMonAlg.L1Chain04 = 'L1_gXENC50'
      #TrigMETMonAlg.L1Chain05 = 'L1_gXERHO50'
      #TrigMETMonAlg.L1Chain06 = 'L1_gXEJWOJ50'
      #TrigMETMonAlg.L1Chain07 = 'L1_gXEPUFIT50'
      #TrigMETMonAlg.HLTChain01 = 'HLT_xe65_cell_L1XE50'
      #TrigMETMonAlg.HLTChain02 = 'HLT_xe100_mht_L1XE50'
      #TrigMETMonAlg.HLTChain03 = 'HLT_xe100_tcpufit_L1XE50'
      #TrigMETMonAlg.HLTChain04 = 'HLT_xe100_trkmht_L1XE50'
      #TrigMETMonAlg.HLTChain05 = 'HLT_xe100_pfsum_L1XE50'
      #TrigMETMonAlg.HLTChain06 = 'HLT_xe100_pfopufit_L1XE50'
      #TrigMETMonAlg.HLTChain07 = 'HLT_xe100_cvfpufit_L1XE50'
      #TrigMETMonAlg.HLTChain08 = 'HLT_xe100_mhtpufit_em_subjesgscIS_L1XE50'
      #TrigMETMonAlg.HLTChain09 = 'HLT_xe100_mhtpufit_pf_subjesgscIS_L1XE50'
      #TrigMETMonAlg.HLTChain10 = 'HLT_xe100_pfsum_cssk_L1XE50'
      #TrigMETMonAlg.HLTChain11 = 'HLT_xe100_pfsum_vssk_L1XE50'
      #TrigMETMonAlg.HLTChain12 = 'HLT_xe65_cell_xe110_tcpuft_L1XE50'
      #TrigMETMonAlg.HLTChain13 = 'HLT_xe100_trkmht_xe85_tcpufit_xe65_cell_L1XE50'
      #TrigMETMonAlg.HLTChain14 = 'HLT_xe95_trkmht_xe90_tcpufit_xe75_cell_L1XE50'
    if mt_chains:
      TrigMETMonAlg.HLTChain02 = 'HLT_xe110_mht_L1XE50'
      TrigMETMonAlg.HLTChain03 = 'HLT_xe110_tcpufit_L1XE50'
      TrigMETMonAlg.HLTChain05 = 'HLT_xe110_pfsum_L1XE50'
      TrigMETMonAlg.HLTChain10 = 'HLT_xe110_pfsum_cssk_L1XE50'
      TrigMETMonAlg.HLTChain11 = 'HLT_xe110_pfsum_vssk_L1XE50'
    else: 
      TrigMETMonAlg.L1Chain02 = 'L1_XE10'
      TrigMETMonAlg.L1Chain03 = 'L1_XE30'
      TrigMETMonAlg.L1Chain04 = 'L1_XE55'
      TrigMETMonAlg.L1Chain05 = 'L1_XE60'
      TrigMETMonAlg.L1Chain06 = 'L1_XE70'
      TrigMETMonAlg.L1Chain07 = 'L1_XE75'
      TrigMETMonAlg.HLTChain01 = 'HLT_xe70_mht_L1XE50'
      TrigMETMonAlg.HLTChain02 = 'HLT_xe90_mht_L1XE50'
      TrigMETMonAlg.HLTChain03 = 'HLT_xe110_mht_L1XE50'
      TrigMETMonAlg.HLTChain04 = 'HLT_xe90_pufit_L1XE50'
      TrigMETMonAlg.HLTChain05 = 'HLT_xe100_pufit_L1XE50'
      TrigMETMonAlg.HLTChain06 = 'HLT_xe110_pufit_L1XE50'
      TrigMETMonAlg.HLTChain07 = 'HLT_xe110_pufit_xe65_L1XE50'
      TrigMETMonAlg.HLTChain08 = 'HLT_xe110_pufit_xe70_L1XE50'


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # expertTrigMETMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    metGroup = helper.addGroup(TrigMETMonAlg,'TrigMETMonitor','HLT/METMon/')

    # Add a GMT for the other example monitor algorithm
    metChain1Group = helper.addGroup(TrigMETMonChain1Alg,'TrigMETMonitor','HLT/METMon/HLT_xe65_cell_L1XE50/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    #
    # Binning
    et_bins=205 #Et
    et_min=-13.5
    et_max=401.5
    et_bins_log=20 #Et_log
    et_min_log=-1.875
    et_max_log=4.125
    ec_bins=199 #Ex, Ey, Ez
    ec_min=-298.5
    ec_max=298.5
    ec_bins_log=27 #Ex_log, Ey_log, Ez_log
    ec_min_log=-4.125
    ec_max_log=4.125
    sumet_bins=305 #sumEt
    sumet_min=-27.0
    sumet_max=4203.0
    sumet_bins_log=20 #sumEt_log
    sumet_min_log=-1.875
    sumet_max_log=4.125
    ## These bin settings are for future use.
    ## commented to avoid compiler warning.
    #sume_bins=153 #sumE
    #sume_min=-27.0
    #sume_max=24003.0
    #sume_bins_log=40 #sumE_log
    #sume_min_log=-1.875
    #sume_max_log=6.125
    phi_bins=100 # phi
    phi_bins_2d=24 # phi
    phi_min=-3.1416
    phi_max=3.1416
    #eta_bins=100 # eta
    eta_bins_2d=24# eta
    eta_min=-4.8
    eta_max=4.8
    eff_bins=42 # efficiency
    eff_min=-13.5
    eff_max=401.5
    # Histograms
    ## offline MET
    metGroup.defineHistogram('offline_Ex',title='Offline Missing E_{x};E_{x} [GeV];Events',
                             path='Expert/Offline',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('offline_Ey',title='Offline Missing E_{y};E_{y} [GeV];Events',
                             path='Expert/Offline',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('offline_Et',title='Offline Missing E_{T};E_{T} [GeV];Events',                            
                             path='Expert/Offline',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('offline_sumEt',title='Offline sumE_{T};sumE_{T} [GeV];Events',
                             path='Expert/Offline',xbins=sumet_bins,xmin=sumet_min,xmax=sumet_max)
    ## HLT electron and muon
    metGroup.defineHistogram('hlt_el_mult',title='HLT Electron Multiplicity;Number of electrons;Events',
                           path='Expert/HLT_ElMu',xbins=10,xmin=0,xmax=10)
    metGroup.defineHistogram('hlt_el_pt',title='HLT Electron p_{T};p_{T} [GeV];Events',
                           path='Expert/HLT_ElMu',xbins=100,xmin=0,xmax=100)
    metGroup.defineHistogram('hlt_mu_mult',title='HLT Muon Multiplicity;Number of muons;Events',
                           path='Expert/HLT_ElMu',xbins=10,xmin=0,xmax=10)
    metGroup.defineHistogram('hlt_mu_pt',title='HLT Muon p_{T};p_{T} [GeV];Events',
                           path='Expert/HLT_ElMu',xbins=100,xmin=0,xmax=100)
    ## L1
    algsL1 = ["roi", "jnc", "jrho", "gnc", "grho", "gjwoj", "gpufit"]
    for alg in algsL1:
      metGroup.defineHistogram('L1_{}_Ex'.format(alg),title='L1_{} Missing E_{{x}};E_{{x}} [GeV];Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=ec_bins,xmin=ec_min,xmax=ec_max)
      metGroup.defineHistogram('L1_{}_Ex_log'.format(alg),title='L1_{} Missing E_{{x}} log;sgn(E_{{x}}) log(E_{{x}}/GeV);Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=ec_bins_log,xmin=ec_min_log,xmax=ec_max_log)
      metGroup.defineHistogram('L1_{}_Ey'.format(alg),title='L1_{} Missing E_{{y}};E_{{y}} [GeV];Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=ec_bins,xmin=ec_min,xmax=ec_max)
      metGroup.defineHistogram('L1_{}_Ey_log'.format(alg),title='L1_{} Missing E_{{y}} log;sgn(E_{{y}}) log(E_{{y}}/GeV);Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=ec_bins_log,xmin=ec_min_log,xmax=ec_max_log)
      metGroup.defineHistogram('L1_{}_Et'.format(alg),title='L1_{} Missing E_{{T}};E_{{T}} [GeV];Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=et_bins,xmin=et_min,xmax=et_max)
      metGroup.defineHistogram('L1_{}_Et_log'.format(alg),title='L1_{} Missing E_{{T}} log;log(E_{{T}}/GeV);Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=et_bins_log,xmin=et_min_log,xmax=et_max_log)
      metGroup.defineHistogram('L1_{}_sumEt'.format(alg),title='L1_{} sumE_{{T}};sumE_{{T}} [GeV];Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=et_bins,xmin=et_min,xmax=et_max)
      metGroup.defineHistogram('L1_{}_sumEt_log'.format(alg),title='L1_{} sumE_{{T}} log;log(sumE_{{T}}/GeV);Events'.format(alg),
                             path='Shifter/L1_{}'.format(alg),xbins=sumet_bins_log,xmin=sumet_min_log,xmax=sumet_max_log)
    ## HLT
    algsHLT = ["cell", "tcpufit", "trkmht", "mht", "tc_em", "pfsum", "pfsum_cssk", "pfsum_vssk", "pfopufit", "cvfpufit", "mhtpufit_pf", "mhtpufit_em"]
    for alg in algsHLT:
      metGroup.defineHistogram('{}_Ex'.format(alg),title='{} Missing E_{{x}};E_{{x}} [GeV];Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=ec_bins,xmin=ec_min,xmax=ec_max)
      metGroup.defineHistogram('{}_Ex_log'.format(alg),title='{} Missing E_{{x}} log;sgn(E_{{x}}) log(E_{{x}}/GeV);Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=ec_bins_log,xmin=ec_min_log,xmax=ec_max_log)
      metGroup.defineHistogram('{}_Ey'.format(alg),title='{} Missing E_{{y}};E_{{y}} [GeV];Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=ec_bins,xmin=ec_min,xmax=ec_max)
      metGroup.defineHistogram('{}_Ey_log'.format(alg),title='{} Missing E_{{y}} log;sgn(E_{{y}}) log(E_{{y}}/GeV);Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=ec_bins_log,xmin=ec_min_log,xmax=ec_max_log)
      metGroup.defineHistogram('{}_Et'.format(alg),title='{} Missing E_{{T}};E_{{T}} [GeV];Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=et_bins,xmin=et_min,xmax=et_max)
      metGroup.defineHistogram('{}_Et_log'.format(alg),title='{} Missing E_{{T}} log;log(E_{{T}}/GeV);Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=et_bins_log,xmin=et_min_log,xmax=et_max_log)
      metGroup.defineHistogram('{}_sumEt'.format(alg),title='{} sumE_{{T}};sumE_{{T}} [GeV];Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=sumet_bins,xmin=sumet_min,xmax=sumet_max)
      metGroup.defineHistogram('{}_sumEt_log'.format(alg),title='{} sumE_{{T}} log;log(sumE_{{T}}/GeV);Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=sumet_bins_log,xmin=sumet_min_log,xmax=sumet_max_log)
      metGroup.defineHistogram('{}_phi'.format(alg),title='{} #phi;#phi;Events'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=phi_bins,xmin=phi_min,xmax=phi_max)
      metGroup.defineHistogram('{0}_phi;{0}_phi_etweight'.format(alg), title='{} #phi (etweighted);#phi;Et weighted events'.format(alg),
                             weight='{}_Et'.format(alg),
                             path='Shifter/{}'.format(alg),xbins=phi_bins,xmin=phi_min,xmax=phi_max)

    ## HLT 2d eta-phi histos
    algsHLT2d = ["cell", "tcpufit"]
    for alg in algsHLT2d:
      metGroup.defineHistogram('{0}_eta,{0}_phi;{0}_eta_phi'.format(alg), type='TH2F', title='{} #eta - #phi;#eta;#phi'.format(alg),
                             path='Shifter/{}'.format(alg),
                             xbins=eta_bins_2d,xmin=eta_min,xmax=eta_max,ybins=phi_bins_2d,ymin=phi_min,ymax=phi_max)
      metGroup.defineHistogram('{a}_eta,{a}_phi;{a}_eta_phi_etweight'.format(a=alg), type='TH2F', title='{} #eta - #phi (etweighted);#eta;#phi'.format(alg),
                             weight='{}_Et'.format(alg),
                             path='Shifter/{}'.format(alg),
                             xbins=eta_bins_2d,xmin=eta_min,xmax=eta_max,ybins=phi_bins_2d,ymin=phi_min,ymax=phi_max)
    ## L1 efficiency
    effL1 = ["01", "02", "03", "04", "05", "06", "07"]
    for eff in effL1:
      metGroup.defineHistogram('offline_Et,pass_L1{0};L1{0}_eff'.format(eff), type='TProfile',title='L1{} efficiency;offline E_{{T}} [GeV];Efficiency'.format(eff),
                             path='Shifter/eff',xbins=eff_bins,xmin=eff_min,xmax=eff_max)
    ## HLT efficiency
    effHLT = ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14"]
    for eff in effHLT:
      metGroup.defineHistogram('offline_Et,pass_HLT{0};HLT{0}_eff'.format(eff), type='TProfile',title='HLT{} efficiency;offline E_{{T}} [GeV];Efficiency'.format(eff),
                             path='Shifter/eff',xbins=eff_bins,xmin=eff_min,xmax=eff_max)
    ## HLT cell component
    metGroup.defineHistogram('HLT_MET_status',title='HLT MET Status;;',
                             weight='MET_status',
                             path='Shifter/Component',xbins=32,xmin=-0.5,xmax=31.5)
    metGroup.defineHistogram('HLT_MET_component,component_Et;compN_compEt', type='TH2F', title='HLT Missing E_{T} VS component;;Missing E_{T} [GeV]',
                             path='Shifter/Component',
                             xbins=25,xmin=-0.5,xmax=24.5,ybins=et_bins,ymin=et_min,ymax=et_max)
    metGroup.defineHistogram('component,component_status;compN_HLT_MET_status', type='TH2F', title='HLT MET Status VS component;;',
                             weight='component_status_weight',
                             path='Shifter/Component',
                             xbins=25,xmin=-0.5,xmax=24.5,ybins=32,ymin=-0.5,ymax=31.5)
    ## HLT tc (Expert)
    metGroup.defineHistogram('tc_Ex',title='tc Missing E_{x};E_{x} [GeV];Events',
                             path='Expert/tc',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tc_Ey',title='tc Missing E_{y};E_{y} [GeV];Events',
                             path='Expert/tc',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tc_Et', title='tc Missing E_{T};E_{T} [GeV];Events',
                             path='Expert/tc',xbins=et_bins,xmin=et_min,xmax=et_max)
    ## Chain specific
    metChain1Group.defineHistogram('cell_Ex',title='cell Missing E_{x};E_{x} [GeV];Events',
                                  path='cell',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metChain1Group.defineHistogram('cell_Ex_log',title='cell Missing E_{x} log;sgn(E_{x}) log_{10}(E_{x}/GeV);Events',
                             path='cell',xbins=ec_bins_log,xmin=ec_min_log,xmax=ec_max_log)
    metChain1Group.defineHistogram('cell_Ey',title='cell Missing E_{y};E_{y} [GeV];Events',
                                  path='cell',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metChain1Group.defineHistogram('cell_Ey_log',title='cell Missing E_{y} log;sgn(E_{y}) log_{10}(E_{y}/GeV);Events',
                             path='cell',xbins=ec_bins_log,xmin=ec_min_log,xmax=ec_max_log)
    metChain1Group.defineHistogram('cell_Et',title='cell Missing E_{T};E_{T} [GeV];Events',
                                  path='cell',xbins=et_bins,xmin=et_min,xmax=et_max)
    metChain1Group.defineHistogram('cell_Et_log',title='cell Missing E_{T} log;log_{10}(E_{T}/GeV);Events',
                             path='cell',xbins=et_bins_log,xmin=et_min_log,xmax=et_max_log)
    metChain1Group.defineHistogram('cell_sumEt',title='cell sumEt;sumE_{T} [GeV];Events',
                             path='cell',xbins=sumet_bins,xmin=sumet_min,xmax=sumet_max)
    metChain1Group.defineHistogram('cell_sumEt_log',title='cell sumE_{T} log;log_{10}(sumE_{T}/GeV);Events',
                             path='cell',xbins=sumet_bins_log,xmin=sumet_min_log,xmax=sumet_max_log)
    metChain1Group.defineHistogram('cell_phi',title='cell #phi;#phi;Events',
                             path='cell',xbins=phi_bins,xmin=phi_min,xmax=phi_max)
    metChain1Group.defineHistogram('cell_phi;cell_phi_etweight', title='cell #phi (etweighted);#phi;E_{T} weighted events',
                             weight='cell_Et',
                             path='cell',xbins=phi_bins,xmin=phi_min,xmax=phi_max)
    
    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
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
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'TrigMETMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigMETMonitorAcc = TrigMETMonConfig(ConfigFlags)
    cfg.merge(trigMETMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigMETMonitorAcc.getEventAlgo('TrigMETMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

