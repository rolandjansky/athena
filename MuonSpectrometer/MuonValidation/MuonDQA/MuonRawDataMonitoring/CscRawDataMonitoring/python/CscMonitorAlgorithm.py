#
#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory

from .CscMonUtils import getCSCLabelx


def CscMonitoringConfigOld(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    helper = AthMonitorCfgHelperOld(inputFlags,'CscAthMonitorCfg')
    return helper.result()

def CscMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))

    
    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'CscAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # This uses the new Configurables object system.
    cscClusMonAlg = helper.addAlgorithm(CompFactory.CscClusterValMonAlg,'CscClusMonAlg')
    cscPrdMonAlg = helper.addAlgorithm(CompFactory.CscPrdValMonAlg,'CscPrdMonAlg')
    cscSegmMonAlg = helper.addAlgorithm(CompFactory.CSCSegmValMonAlg,'CscSegmMonAlg')


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    cscClusMonAlg.CSCQmaxCutADC = 100
    cscClusMonAlg.CSCDoEventSelection = False
    cscClusMonAlg.CSCEventSelTriggers = [ "" ]
    cscPrdMonAlg.NoiseCutADC = 50 
    cscPrdMonAlg.MapYXandRZ = False
    cscSegmMonAlg.DoEventSelection = False
    cscSegmMonAlg.EventSelTriggers = [ "" ] #[ "L1_MU10", "L1_MU15", "EF_mu20_muCombTag_NoEF", "EF_mu15", "EF_mu15_mu10_EFFS", "EF_2mu10", "EF_2mu10_loose"  ]

    # to enable a trigger filter, for example:
    #exampleMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()
    from MuonConfig.MuonCalibrationConfig import CscCalibToolCfg
    calibtool = result.popToolsAndMerge( CscCalibToolCfg(inputFlags) )
    cscClusMonAlg.CscCalibTool = calibtool

    from MuonConfig.MuonSegmentFindingConfig import CalibCscStripFitterCfg
    stripfitter = result.popToolsAndMerge( CalibCscStripFitterCfg(inputFlags) ) 
    cscClusMonAlg.CSCStripFitter = stripfitter
    cscPrdMonAlg.CSCStripFitter = stripfitter
 
    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    cscClusGroup = helper.addGroup(cscClusMonAlg,'CscClusMonitor','Muon/MuonRawDataMonitoring/CSC/')
    cscPrdGroup = helper.addGroup(cscPrdMonAlg,'CscPrdMonitor','Muon/MuonRawDataMonitoring/CSC/')
    cscSegmGroup = helper.addGroup(cscSegmMonAlg,'CscSegmMonitor','Muon/MuonSegmentMonitoring/')



    ### STEP 5 ###
    # Configure histograms

    qmaxCut = str(cscClusMonAlg.CSCQmaxCutADC) 
    #Cluster
    cscClusGroup.defineHistogram('z,r;h2csc_clus_r_vs_z_hitmap',type='TH2F',title='R vs. Z Cluster hitmap;z(mm);R(mm)',
                                 path='Clusters/Shift',xbins=200,xmin=-10000.,xmax=10000., ybins=40, ymin=0., ymax=4000.)

    cscClusGroup.defineHistogram('y,x;h2csc_clus_y_vs_x_hitmap',type='TH2F',title='X vs. Y Cluster hitmap;y(mm);x(mm)',
                                 path='Clusters/Shift',xbins=100,xmin=-5000.,xmax=5000.,ybins=100,ymin=-5000,ymax=5000)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_phicluswidth',type='TH2F',cutmask='clus_phi',title='Phi-Cluster width;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_etacluswidth',type='TH2F',cutmask='clus_eta',title='Eta-Cluster width;# strips;[sector] + [0.2 #times layer]',
                                  path='Clusters/Expert',xbins=192, xmin=0, xmax=192, ybins=175, ymin=-17, ymax=18)

    cscClusGroup.defineHistogram('stripid,secLayer;h2csc_clus_hitmap',type='TH2F',title='Cluster occupancy;channel;[sector]+[0.2 #times layer]',
                                  path='Clusters/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('fStripIDs_col,secLayer;h2csc_clus_hitmap_signal',cutmask='signal_mon',type='TH2F',title='Cluster occupancy, Qmax > '+qmaxCut+' counts;channel;[sector] + [0.2 #times layer]',
                                  path='Clusters/Shift',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_phicluswidth_signal',type='TH2F',cutmask='clus_phiSig',title='#phi-cluster width, Qmax > '+qmaxCut+' counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_etacluswidth_signal',type='TH2F',cutmask='clus_etaSig',title='#eta-cluster width, Qmax > '+qmaxCut+' counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=192,xmin=0,xmax=192,ybins=175,ymin=-17,ymax=18)

    thisLabelx=getCSCLabelx("labels_clus_occupancy_signal_EA")
    cscClusGroup.defineHistogram('secLayer;h1csc_clus_occupancy_signal_EA',type='TH1F',cutmask='sideA',title='EndCap A: Layer occupancy, Qmax > '+qmaxCut+' counts;;entries/layer',
                                 path='Overview/CSCEA/Cluster',xbins=90,xmin=0,xmax=18, xlabels=thisLabelx)

    thisLabelx=getCSCLabelx("labels_clus_occupancy_signal_EC")
    cscClusGroup.defineHistogram('secLayer;h1csc_clus_occupancy_signal_EC',type='TH1F',cutmask='sideC',title='EndCap C: Layer occupancy, Qmax > '+qmaxCut+' counts;;entries/layer',
                                 path='Overview/CSCEC/Cluster',xbins=85,xmin=-17.,xmax=0., xlabels=thisLabelx) 

    cscClusGroup.defineHistogram('fStripIDs_col,secLayer;h2csc_clus_hitmap_noise',cutmask='noise_mon',type='TH2F',title='Cluster occupancy, Qmax #leq '+qmaxCut+' counts;channel;[sector] + [0.2 #times layer]',
                                  path='Clusters/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_phicluswidth_noise',type='TH2F',cutmask='clus_phiNoise',title='#phi-cluster width, Qmax #leq '+qmaxCut+' counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_etacluswidth_noise',type='TH2F',cutmask='clus_etaNoise',title='#eta-cluster width, Qmax #leq '+qmaxCut+' counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=192,xmin=0,xmax=192,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax',type='TH2F',title='Cluster peak-strip charge, Qmax;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_signal',cutmask='signal_mon',type='TH2F',title='Cluster peak-strip charge, Qmax > '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Shift',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_noise',cutmask='noise_mon',type='TH2F',title='Cluster peak-strip charge, Qmax #leq '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_signal_EA',cutmask='sideA',type='TH2F',title='EndCap A: Cluster peak-strip charge, Qmax > '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000,ybins=90,ymin=0,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC;h1csc_clus_qmax_signal_EA_count',cutmask='sideA',type='TH1F',title='EndCap A: Cluster peak-strip charge, Qmax > '+qmaxCut+' counts;counts;entries/20 counts;',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_signal_EC',cutmask='sideC',type='TH2F',title='EndCap C: Cluster peak-strip charge, Qmax > '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000,ybins=90,ymin=0,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC;h1csc_clus_qmax_signal_EC_count',cutmask='sideC',type='TH1F',title='EndCap C: Cluster peak-strip charge, Qmax > '+qmaxCut+' counts;counts;entries/20 counts;',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum',type='TH2F',title='Cluster charge (Qsum);counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum_signal',cutmask='signal_mon',type='TH2F',title='Cluster charge(Qsum), Qmax > '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Shift',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum_signal_EA',cutmask='sideA',type='TH2F',title='EndCap A: Cluster charge(Qsum), Qmax > '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000,ybins=90,ymin=0,ymax=18)

    cscClusGroup.defineHistogram('QsumADC;h1csc_clus_qsum_signal_EA_count',cutmask='sideA',type='TH1F',title='EndCap A: Cluster charge(Qsum), Qmax > '+qmaxCut+' counts;counts;entries/20 counts;',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC;h2csc_clus_qsum_signal_EC',cutmask='sideC',type='TH1F',title='EndCap C: Cluster charge(Qsum), Qmax > '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC;h1csc_clus_qsum_signal_EC_count',cutmask='sideC',type='TH1F',title='EndCap C: Cluster charge(Qsum), Qmax > '+qmaxCut+' counts;counts;entries/20 counts;',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum_noise',cutmask='noise_mon',type='TH2F',title='Cluster charge(Qsum), Qmax #leq '+qmaxCut+' counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_transverse_time',cutmask='clus_phi',type='TH1F',title='#phi-cluster sampling time;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_transverse_charge',cutmask='clus_phi',type='TH1F',title='#phi-cluster charge;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_transverse_time_signal',cutmask='clus_phiSig',type='TH1F',title='#phi-cluster sampling time, Qmax > '+qmaxCut+' counts;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_transverse_charge_signal',cutmask='clus_phiSig',type='TH1F',title='#phi-cluster charge, Qmax > '+qmaxCut+' counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_transverse_time_noise',cutmask='clus_phiNoise',type='TH1F',title='#phi-cluster sampling time, Qmax #leq '+qmaxCut+' counts;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_transverse_charge_noise',cutmask='clus_phiNoise',type='TH1F',title='#phi-cluster charge, Qmax #leq '+qmaxCut+' counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time',cutmask='clus_eta',type='TH1F',title='#eta-cluster sampling time;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_precision_charge',cutmask='clus_eta',type='TH1F',title='eta-cluster charge;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_signal',cutmask='clus_etaSig',type='TH1F',title='#eta-cluster sampling time, Qmax > '+qmaxCut+' counts;ns;entries/ns',
                                 path='Clusters/Shift',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_precision_charge_signal',cutmask='clus_etaSig',type='TH1F',title='#eta-cluster charge, Qmax > '+qmaxCut+' counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_signal_EA',cutmask='sideA',type='TH1F',title='EndCap A: #eta-cluster sampling time, Qmax > '+qmaxCut+' counts;ns;entries/ns',
                                 path='Overview/CSCEA/Cluster',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_signal_EC',cutmask='sideC',type='TH1F',title='EndCap C: #eta-cluster sampling time, Qmax > '+qmaxCut+' counts;ns;entries/ns',
                                 path='Overview/CSCEC/Cluster',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_noise',cutmask='clus_etaNoise',type='TH1F',title='#eta-cluster sampling time, Qmax #leq '+qmaxCut+' counts;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_precision_charge_noise',cutmask='clus_etaNoise',type='TH1F',title='#eta-cluster charge, Qmax #leq '+qmaxCut+' counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('stripsSum_EA_mon;h1csc_clus_totalWidth_EA',type='TH1F',title='EndCap A: Cluster hits in all EA eta(#eta) & phi(#phi) strips;strips;cluster hits',
                                 path='Overview/CSCEA/Cluster',xbins=15360,xmin=1.,xmax=15361.)

    cscClusGroup.defineHistogram('stripsSum_EC_mon;h1csc_clus_totalWidth_EC',type='TH1F',title='EndCap C: Cluster hits in all EC eta(#eta) & phi(#phi) strips;strips;cluster hits',
                                 path='Overview/CSCEC/Cluster',xbins=15360,xmin=1.,xmax=15361.)

    cscClusGroup.defineHistogram('nPhiClusWidthCnt_mon,nEtaClusWidthCnt_mon;h2csc_clus_eta_vs_phi_cluswidth',type='TH2F',title='Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    cscClusGroup.defineHistogram('count_mon,secLayer;h2csc_clus_phicluscount',cutmask='mphi_true',type='TH2F',title='#phi-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('scount_mon,secLayer;h2csc_clus_phicluscount_signal',cutmask='scount_phi_true',type='TH2F',title='#phi-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('count_diff,secLayer;h2csc_clus_phicluscount_noise',cutmask='scount_phi_false',type='TH2F',title='#phi-cluster count, Qmax #leq '+qmaxCut+' counts;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('count_mon,secLayer;h2csc_clus_etacluscount',cutmask='mphi_false',type='TH2F',title='#eta-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('scount_mon,secLayer;h2csc_clus_etacluscount_signal',cutmask='scount_eta_true',type='TH2F',title='#eta-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('count_diff,secLayer;h2csc_clus_etacluscount_noise',cutmask='scount_eta_false',type='TH2F',title='#eta-cluster count, Qmax #leq '+qmaxCut+' counts;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('segNum_mon,sec_mon;h2csc_clus_segmap_signal',type='TH2F',title='Segment occupancy, Qmax > '+qmaxCut+' counts;segment;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=16,xmin=-0.5,xmax=15.5,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('numphi_numeta_mon;h1csc_clus_count',type='TH1F',title='Clusters per event;no.of clusters;entries',
                                 path='Clusters/Expert',xbins=26,xmin=-1,xmax=25)

    cscClusGroup.defineHistogram('numphi_numeta_sig_mon;h1csc_clus_count_signal',type='TH1F',title='Clusters per event, Qmax > '+qmaxCut+' counts;no.of clusters;entries',
                                 path='Clusters/Expert',xbins=26,xmin=-1,xmax=25)

    cscClusGroup.defineHistogram('num_num_noise_mon;h1csc_clus_count_noise',type='TH1F',title='Clusters per event, Qmax #leq '+qmaxCut+' counts;no.of clusters;entries',
                                 path='Clusters/Expert',xbins=26,xmin=-1,xmax=25)

    cscClusGroup.defineHistogram('numphi_mon,numeta_mon;h2csc_clus_eta_vs_phi_cluscount',type='TH2F',title='Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    cscClusGroup.defineHistogram('numphi_sig_mon,numeta_sig_mon;h2csc_clus_eta_vs_phi_cluscount_signal',type='TH2F',title='Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    cscClusGroup.defineHistogram('numphi_diff_mon,numeta_diff_mon;h2csc_clus_eta_vs_phi_cluscount_noise',type='TH2F',title='Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    #PRD
    cscPrdGroup.defineHistogram('spid, secLayer;h2csc_prd_hitmap', type='TH2F', title='Hit Occupancy; channel; [sector] + [0.2 #times layer]',
                                 path='PRD/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_phicluswidth',type='TH2F',cutmask='measphi',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='PRD/Expert',xbins=48,xmin=0,xmax=48, ybins=175,ymin=-17.,ymax=18.)

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_etacluswidth',type='TH2F',cutmask='measeta',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='PRD/Expert',xbins=192,xmin=0,xmax=192, ybins=175,ymin=-17.,ymax=18.)
   
    cscPrdGroup.defineHistogram('z,r;h2csc_prd_r_vs_z_hitmap',type='TH2F',title='R vs. Z Cluster hitmap;z(mm);R(mm)',
                                path='PRD/Shift',xbins=200,xmin=-10000.,xmax=10000., ybins=40, ymin=0., ymax=4000.) 

    cscPrdGroup.defineHistogram('y,x;h2csc_prd_y_vs_x_hitmap',type='TH2F',title='Y vs. X Cluster hitmap;x(mm);y(mm)',
                                path='PRD/Shift',xbins=100,xmin=-5000.,xmax=5000., ybins=100, ymin=-5000., ymax=5000.) 

    cscPrdGroup.defineHistogram('spid,secLayer;h2csc_prd_hitmap_signal',cutmask='signal_mon',type='TH2F',title='Signal Occupancy;channel;[sector] + [0.2 #times layer]',
                                path='PRD/Shift',xbins=242,xmin=-49.,xmax=193., ybins=175, ymin=-17., ymax=18.) 

    cscPrdGroup.defineHistogram('spid,secLayer;h2csc_prd_hitmap_signal_EC',cutmask='sideC',type='TH2F',title='EndCap C: Signal Occupancy;channel;[sector] + [0.2 #times layer]',
                                path='Overview/CSCEC/PRD',xbins=242,xmin=-49.,xmax=193., ybins=85, ymin=-17., ymax=0.)

    cscPrdGroup.defineHistogram('spid;h1csc_prd_hitmap_signal_EC_count',cutmask='sideC',type='TH1F',title='EndCap C: Signal Occupancy;channel;entries/channel;',
                                path='Overview/CSCEC/PRD',xbins=242,xmin=-49.,xmax=193.)

    thisLabelx=getCSCLabelx("labels_clus_occupancy_signal_EC")
    cscPrdGroup.defineHistogram('secLayer;h1csc_prd_hitmap_signal_EC_occupancy',cutmask='sideC',type='TH1F',title='EndCap C: Signal Occupancy;;entries/layer',
                                path='Overview/CSCEC/PRD',xbins=85,xmin=-17.,xmax=0., xlabels=thisLabelx) #labels

    cscPrdGroup.defineHistogram('lumiblock_mon,secLayer;h2csc_prd_occvslb_EC',cutmask='sideC',type='TH2F',title='EndCap C: Layer Signal Occupancy Per LB;LB;[sector] + [0.2 #times layer]',
                                path='Overview/CSCEC/PRD',xbins=2510,xmin=-10.,xmax=2500., ybins=85, ymin=-17., ymax=0.) 

    cscPrdGroup.defineHistogram('spid,secLayer;h2csc_prd_hitmap_signal_EA',cutmask='sideA',type='TH2F',title='EndCap A: Signal Occupancy;channel;[sector] + [0.2 #times layer]',
                                path='Overview/CSCEA/PRD',xbins=242,xmin=-49.,xmax=193., ybins=90, ymin=0., ymax=18.)

    cscPrdGroup.defineHistogram('spid;h1csc_prd_hitmap_signal_EA_count',cutmask='sideA',type='TH1F',title='EndCap A: Signal Occupancy;channel;entries/channel;',
                                path='Overview/CSCEA/PRD',xbins=242,xmin=-49.,xmax=193.)
    
    thisLabelx=getCSCLabelx("labels_clus_occupancy_signal_EA")
    cscPrdGroup.defineHistogram('secLayer;h1csc_prd_hitmap_signal_EA_occupancy',cutmask='sideA',type='TH1F',title='EndCap A: Signal Occupancy;;entries/layer',
                                path='Overview/CSCEA/PRD',xbins=90,xmin=0.,xmax=18., xlabels=thisLabelx) #labels

    cscPrdGroup.defineHistogram('lumiblock_mon,secLayer;h2csc_prd_occvslb_EA',cutmask='sideA',type='TH2F',title='EndCap A: Layer Signal Occupancy Per LB;LB;[sector] + [0.2 #times layer]',
                                path='Overview/CSCEA/PRD',xbins=2510,xmin=-10.,xmax=2500., ybins=90, ymin=0., ymax=18.) 

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_etacluswidth_signal',cutmask='clus_etaSig',type='TH2F',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=192,xmin=0.,xmax=192., ybins=175, ymin=-17., ymax=18.) 

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_phicluswidth_signal',cutmask='clus_phiSig',type='TH2F',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=48,xmin=0.,xmax=48., ybins=175, ymin=-17., ymax=18.) 

    cscPrdGroup.defineHistogram('spid,secLayer;h2csc_prd_hitmap_noise',cutmask='noise_mon',type='TH2F',title='Noise Occupancy;channel;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=242,xmin=-49.,xmax=193., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_etacluswidth_noise',cutmask='clus_etaNoise',type='TH2F',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=192,xmin=0.,xmax=192., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_phicluswidth_noise',cutmask='clus_phiNoise',type='TH2F',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=48,xmin=0.,xmax=48., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('count_mon,secLayer;h2csc_prd_phicluscount',cutmask='mphi_true',type='TH2F',title='PRD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=20,xmin=0.,xmax=20., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('scount_mon,secLayer;h2csc_prd_phicluscount_signal',cutmask='scount_phi_true',type='TH2F',title='PRD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                path='PRD/Shift',xbins=20,xmin=0.,xmax=20., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('tmp_val_mon,secLayer;h2csc_prd_phicluscount_noise',cutmask='mphi_true',type='TH2F',title='PRD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=20,xmin=0.,xmax=20., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('count_mon,secLayer;h2csc_prd_etacluscount',cutmask='mphi_false',type='TH2F',title='PRD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=20,xmin=0.,xmax=20., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('scount_mon,secLayer;h2csc_prd_etacluscount_signal',cutmask='scount_eta_true',type='TH2F',title='PRD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                path='PRD/Shift',xbins=20,xmin=0.,xmax=20., ybins=175, ymin=-17., ymax=18.)    

    cscPrdGroup.defineHistogram('tmp_val_mon,secLayer;h2csc_prd_etacluscount_noise',cutmask='mphi_false',type='TH2F',title='PRD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                path='PRD/Expert',xbins=20,xmin=0.,xmax=20., ybins=175, ymin=-17., ymax=18.)

    cscPrdGroup.defineHistogram('numphi_mon,numeta_mon;h2csc_prd_eta_vs_phi_cluscount',type='TH2F',title='Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                path='PRD/Expert',xbins=100,xmin=0,xmax=100, ybins=100, ymin=0, ymax=100)

    cscPrdGroup.defineHistogram('numphi_sig_mon,numeta_sig_mon;h2csc_prd_eta_vs_phi_cluscount_signal',type='TH2F',title='Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                path='PRD/Expert',xbins=100,xmin=0,xmax=100, ybins=100, ymin=0, ymax=100)

    cscPrdGroup.defineHistogram('numphi_diff_mon,numeta_diff_mon;h2csc_prd_eta_vs_phi_cluscount_noise',type='TH2F',title='Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                path='PRD/Expert',xbins=100,xmin=0,xmax=100, ybins=100, ymin=0, ymax=100)

    cscClusGroup.defineHistogram('nPhiClusWidthCnt_mon,nEtaClusWidthCnt_mon;h2csc_prd_eta_vs_phi_cluswidth',type='TH2F',title='Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width',
                                 path='PRD/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    #Segments

    cscSegmGroup.defineHistogram('n_clust,segm_sectorNo;Muon_Segm_NumSegments_EA',cutmask='sideA',type='TH2F',title='EndCap A: No. of segments; #segments; [sector] + [0.2 #times layer]',
                                 path='EndCapA/Overview/MuonSegments/CSC',xbins=20,xmin=0,xmax=80,ybins=90,ymin=0,ymax=18.)

    cscSegmGroup.defineHistogram('n_clust,segm_sectorNo;Muon_Segm_NumSegments_EC',cutmask='sideC',type='TH2F',title='EndCap C: No. of segments; #segments; [sector] + [0.2 #times layer]',
                                 path='EndCapC/Overview/MuonSegments/CSC',xbins=20,xmin=0,xmax=80,ybins=85,ymin=-17,ymax=0.)

    thisLabelx=getCSCLabelx("labels_sgm_status")
    status_bins = len(thisLabelx)
    cscSegmGroup.defineHistogram('status_mon;Muon_Segm_EtaClusterStatus_EA',cutmask='sideA_phi0',type='TH1F',title='Endcap A: #eta-cluster status;;entries',
                                 path='EndCapA/Detail/CSC',xbins=status_bins,xmin=-0.5,xmax=status_bins-0.5,xlabels=thisLabelx)

    thisLabelx=getCSCLabelx("labels_sgm_status")
    status_bins = len(thisLabelx)
    cscSegmGroup.defineHistogram('status_mon;Muon_Segm_EtaClusterStatus_EC',cutmask='sideC_phi0',type='TH1F',title='Endcap C: #eta-cluster status;;entries',
                                 path='EndCapC/Detail/CSC',xbins=status_bins,xmin=-0.5,xmax=status_bins-0.5,xlabels=thisLabelx)

    thisLabelx=getCSCLabelx("labels_sgm_status")
    status_bins = len(thisLabelx)
    cscSegmGroup.defineHistogram('status_mon;Muon_Segm_PhiClusterStatus_EA',cutmask='sideA_phi',type='TH1F',title='Endcap A: #phi-cluster status;;entries',
                                 path='EndCapA/Detail/CSC',xbins=status_bins,xmin=-0.5,xmax=status_bins-0.5,xlabels=thisLabelx)

    thisLabelx=getCSCLabelx("labels_sgm_status")
    status_bins = len(thisLabelx)
    cscSegmGroup.defineHistogram('status_mon;Muon_Segm_PhiClusterStatus_EC',cutmask='sideC_phi',type='TH1F',title='Endcap C: #phi-cluster status;;entries',
                                 path='EndCapC/Detail/CSC',xbins=status_bins,xmin=-0.5,xmax=status_bins-0.5,xlabels=thisLabelx)

    
    cscSegmGroup.defineHistogram('clus_qsum_mon,clus_secLayer;Muon_Segm_QSumEtaGoodClusPerLayer_EA',cutmask='checkStatusEtaA',type='TH2F',title='EndCap A: #eta-cluster charge per layer;counts;[sector] + [0.2 #times layer]',
                                 path='EndCapA/Overview/MuonSegments/CSC',xbins=80,xmin=0.,xmax=4000.,ybins=90,ymin=0.,ymax=18.) #PLEASE CHECk

    cscSegmGroup.defineHistogram('clus_time_mon,clus_secLayer;Muon_Segm_TimeEtaGoodClusPerLayer_EA',cutmask='checkTimeEtaA',type='TH2F',title='EndCap A: #eta-cluster time per layer;time [ns];[sector] + [0.2 #times layer]',
                                 path='EndCapA/Overview/MuonSegments/CSC',xbins=40,xmin=-200.,xmax=200.,ybins=90,ymin=0.,ymax=18.) #PLEASE CHECk

    cscSegmGroup.defineHistogram('clus_qsum_mon,clus_secLayer;Muon_Segm_QSumEtaGoodClusPerLayer_EC',cutmask='checkStatusEtaC',type='TH2F',title='EndCap C: #eta-cluster charge per layer;counts;[sector] + [0.2 #times layer]',
                                 path='EndCapC/Overview/MuonSegments/CSC',xbins=80,xmin=0.,xmax=4000.,ybins=85,ymin=-17.,ymax=0.) 

    cscSegmGroup.defineHistogram('clus_time_mon,clus_secLayer;Muon_Segm_TimeEtaGoodClusPerLayer_EC',cutmask='checkTimeEtaC',type='TH2F',title='EndCap C: #eta-cluster time per layer;time [ns];[sector] + [0.2 #times layer]',
                                 path='EndCapC/Overview/MuonSegments/CSC',xbins=40,xmin=-200.,xmax=200.,ybins=85,ymin=-17.,ymax=0.) 

    cscSegmGroup.defineHistogram('clus_qsum_mon,clus_secLayer;Muon_Segm_QSumPhiGoodClusPerLayer_EA',cutmask='checkStatusPhiA',type='TH2F',title='EndCap A: #phi-cluster charge per layer;counts;[sector] + [0.2 #times layer]',
                                 path='EndCapA/Overview/MuonSegments/CSC',xbins=80,xmin=0.,xmax=4000,ybins=90,ymin=0.,ymax=18.) 

    cscSegmGroup.defineHistogram('clus_time_mon,clus_secLayer;Muon_Segm_TimePhiGoodClusPerLayer_EA',cutmask='checkTimePhiA',type='TH2F',title='EndCap A: #phi-cluster time per layer;time [ns];[sector] + [0.2 #times layer]',
                                 path='EndCapA/Overview/MuonSegments/CSC',xbins=40,xmin=-200.,xmax=200.,ybins=90, ymin=0.,ymax=18.) 

    cscSegmGroup.defineHistogram('clus_qsum_mon,clus_secLayer;Muon_Segm_QSumPhiGoodClusPerLayer_EC',cutmask='checkStatusPhiC',type='TH2F',title='EndCap C: #phi-cluster charge per layer;counts;[sector] + [0.2 #times layer]',
                                 path='EndCapC/Overview/MuonSegments/CSC',xbins=80,xmin=0.,xmax=4000,ybins=85,ymin=-17.,ymax=0.) 

    cscSegmGroup.defineHistogram('clus_time_mon,clus_secLayer;Muon_Segm_TimePhiGoodClusPerLayer_EC',cutmask='checkTimePhiC',type='TH2F',title='EndCap A: #phi-cluster time per layer;time [ns];[sector] + [0.2 #times layer]',
                                 path='EndCapC/Overview/MuonSegments/CSC',xbins=40,xmin=-200.,xmax=200.,ybins=85,ymin=-17.,ymax=0.) 

    cscSegmGroup.defineHistogram('tmp_etaClusA;Muon_Segm_NumEtaCluster_EA',cutmask='checkClusEtaA',type='TH1F',title='Endcap A: No. of #eta-clusters on segment;#clusters;entries',
                                 path='EndCapA/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_etaClusGoodA;Muon_Segm_NumGoodEtaCluster_EA',cutmask='checkClusEtaGoodA',type='TH1F',title='Endcap A: No. of good #eta-clusters on segment;#good-clusters;entries',
                                 path='EndCapA/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_phiClusA;Muon_Segm_NumPhiCluster_EA',cutmask='checkClusPhiA',type='TH1F',title='Endcap A: No. of #phi-clusters on segment;#clusters;entries',
                                 path='EndCapA/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_phiClusGoodA;Muon_Segm_NumGoodPhiCluster_EA',cutmask='checkClusPhiGoodA',type='TH1F',title='Endcap A: No. of good #phi-clusters on segment;#good-clusters;entries',
                                 path='EndCapA/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_etaClusC;Muon_Segm_NumEtaCluster_EC',cutmask='checkClusEtaC',type='TH1F',title='Endcap C: No. of #eta-clusters on segment;#clusters;entries',
                                 path='EndCapC/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_etaClusGoodC;Muon_Segm_NumGoodEtaCluster_EC',cutmask='checkClusEtaGoodC',type='TH1F',title='Endcap C: No. of good #eta-clusters on segment;#good-clusters;entries',
                                 path='EndCapC/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_phiClusC;Muon_Segm_NumPhiCluster_EC',cutmask='checkClusPhiC',type='TH1F',title='Endcap C: No. of #phi-clusters on segment;#clusters;entries',
                                 path='EndCapC/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    cscSegmGroup.defineHistogram('tmp_phiClusGoodC;Muon_Segm_NumGoodPhiCluster_EC',cutmask='checkClusPhiGoodC',type='TH1F',title='Endcap C: No. of good #phi-clusters on segment;#good-clusters;entries',
                                 path='EndCapC/Detail/CSC',xbins=11,xmin=-0.5,xmax=10.5)

    thisLabelx=getCSCLabelx("labels_sgm_clusCounts")
    cscSegmGroup.defineHistogram('tmp_layerIndexA_mon,tmp_isectA_mon;Muon_Segm_NumNClusSegments_EA',type='TH2F',title='EndCap A: #eta-number of 3 and 4 cluster segments with each layer;counts;[sector] + [0.2 #times layer]',
                                 path='EndCapA/Detail/CSC',xbins=50,xmin=.5,xmax=5.5,ybins=90,ymin=0,ymax=18.,xlabels=thisLabelx)

    thisLabelx=getCSCLabelx("labels_sgm_clusCounts")
    cscSegmGroup.defineHistogram('tmp_layerIndexC_mon,tmp_isectC_mon;Muon_Segm_NumNClusSegments_EC',type='TH2F',title='EndCap C: #eta-number of 3 and 4 cluster segments with each layer;counts;[sector] + [0.2 #times layer]',
                                 path='EndCapC/Detail/CSC',xbins=50,xmin=.5,xmax=5.5,ybins=85,ymin=-17,ymax=0.,xlabels=thisLabelx)

    cscSegmGroup.defineHistogram('etaClusQSumA;Muon_Segm_QSumEtaCluster_EA',type='TH1F',title='Endcap A: #eta-cluster Qsum;counts;entries',
                                 path='EndCapA/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('phiClusQSumA;Muon_Segm_QSumPhiCluster_EA',type='TH1F',title='Endcap A: #phi-cluster Qsum;counts;entries',
                                 path='EndCapA/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('etaTimeClusA;Muon_Segm_TimeEtaCluster_EA',type='TH1F',title='Endcap A: #eta-cluster Time;time [ns];entries',
                                 path='EndCapA/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('phiTimeClusA;Muon_Segm_TimePhiCluster_EA',type='TH1F',title='Endcap A: #phi-cluster Time;time [ns];entries',
                                 path='EndCapA/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('etaClusQSumC;Muon_Segm_QSumEtaCluster_EC',type='TH1F',title='Endcap C: #eta-cluster Qsum;counts;entries',
                                 path='EndCapC/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('phiClusQSumC;Muon_Segm_QSumPhiCluster_EC',type='TH1F',title='Endcap C: #phi-cluster Qsum;counts;entries',
                                 path='EndCapC/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('etaTimeClusC;Muon_Segm_TimeEtaCluster_EC',type='TH1F',title='Endcap C: #eta-cluster Time;time [ns];entries',
                                 path='EndCapC/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('phiTimeClusC;Muon_Segm_TimePhiCluster_EC',type='TH1F',title='Endcap C: #phi-cluster Time;time [ns];entries',
                                 path='EndCapC/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('etaQSumGoodClusA;Muon_Segm_QSumGoodEtaCluster_EA',type='TH1F',title='Endcap A: Good #eta-cluster Qsum;counts;entries',
                                 path='EndCapA/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('phiQSumGoodClusA;Muon_Segm_QSumGoodPhiCluster_EA',type='TH1F',title='Endcap A: Good #phi-cluster Qsum;counts;entries',
                                 path='EndCapA/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('etaQSumGoodClusC;Muon_Segm_QSumGoodEtaCluster_EC',type='TH1F',title='Endcap C: Good #eta-cluster Qsum;counts;entries',
                                 path='EndCapC/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('phiQSumGoodClusC;Muon_Segm_QSumGoodPhiCluster_EC',type='TH1F',title='Endcap C: Good #phi-cluster Qsum;counts;entries',
                                 path='EndCapC/Detail/CSC',xbins=80,xmin=0.,xmax=4000.)

    cscSegmGroup.defineHistogram('etaTimeGoodClusA;Muon_Segm_TimeGoodEtaCluster_EA',type='TH1F',title='Endcap A: Good #eta-cluster Time;time [ns];entries',
                                 path='EndCapA/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('etaTimeGoodClusC;Muon_Segm_TimeGoodEtaCluster_EC',type='TH1F',title='Endcap C: Good #eta-cluster Time;time [ns];entries',
                                 path='EndCapC/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('phiTimeGoodClusA;Muon_Segm_TimeGoodPhiCluster_EA',type='TH1F',title='Endcap A: Good #phi-cluster Time;time [ns];entries',
                                 path='EndCapA/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('phiTimeGoodClusC;Muon_Segm_TimeGoodPhiCluster_EC',type='TH1F',title='Endcap C: Good #phi-cluster Time;time [ns];entries',
                                 path='EndCapC/Detail/CSC',xbins=40,xmin=-200.,xmax=200.)

    cscSegmGroup.defineHistogram('etaQSumTot0,phiQSumTot0;Muon_Segm_QSumGoodClusCorrelation_EA',type='TH2F',title='Endcap A: #phi-cluster vs. good #eta-cluster;good #eta-cluster counts;good #phi-cluster counts',
                                 path='EndCapA/Detail/CSC',xbins=80,xmin=0.,xmax=4000.,ybins=80,ymin=0.,ymax=4000.)

    cscSegmGroup.defineHistogram('etaQSumTot,phiQSumTot;Muon_Segm_QSumGoodClusCorrelation_EC',type='TH2F',title='Endcap C: #phi-cluster vs. good #eta-cluster;good #eta-cluster counts;good #phi-cluster counts',
                                 path='EndCapC/Detail/CSC',xbins=80,xmin=0.,xmax=4000.,ybins=80,ymin=0.,ymax=4000.)





    #myGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
    #                       path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5,weight='testweight')
    #myGroup.defineHistogram('random', title='LB;x;Events',
    #                        path='ToBringThemAll',xbins=30,xmin=0,xmax=1,opt='kLBNHistoryDepth=10')
    #myGroup.defineHistogram('random', title='title;x;y',path='ToBringThemAll',
    #                        xbins=[0,.1,.2,.4,.8,1.6])
    ##myGroup.defineHistogram('random,pT', type='TH2F', title='title;x;y',path='ToBringThemAll',
     #                       xbins=[0,.1,.2,.4,.8,1.6],ybins=[0,10,30,40,60,70,90])
    # TEfficiencies
    ##myGroup.defineHistogram('pT_passed,pT', type='TEfficiency', title='Test TEfficiency;x;Eff',
    #                        path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0)
    #myGroup.defineHistogram('pT_passed,pT,random', type='TEfficiency', title='Test TEfficiency 2D;x;y;Eff',
    #                        path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0,
    #                        ybins=10, ymin=0.0, ymax=2.0)
    # # use a cutmask to only fill certain events
    #myGroup.defineHistogram('pT;pT_with_cut', title='p_{T};p_{T};Events', path='AndInTheDarkness',
    #                         xbins=50, xmin=0, xmax=50, cutmask='pT_passed')
    # make a TTree
    #myGroup.defineTree('pT,lb,pT_vec,strvec,str;testtree', path='BindThem',
    #                    treedef='pT/F:lb/i:pT_vec/vector<float>:strvec/vector<string>:str/string')

    #anotherGroup.defineHistogram('lbWithFilter',title='Lumi;lb;Events',
    #                             path='top',xbins=1000,xmin=-0.5,xmax=999.5)
    #anotherGroup.defineHistogram('run',title='Run Number;run;Events',
    #                             path='top',xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Example defining an array of histograms. This is useful if one seeks to create a
    # number of histograms in an organized manner. (For instance, one plot for each ASIC
    # in the subdetector, and these components are mapped in eta, phi, and layer.) Thus,
    # one might have an array of TH1's such as quantity[etaIndex][phiIndex][layerIndex].
   # for alg in [exampleMonAlg,anotherExampleMonAlg]:
        # Using an array of groups
    #    topPath = 'OneRing' if alg == exampleMonAlg else ''
    #    array = helper.addArray([2],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('a,b',title='AB',type='TH2F',path='Eta',
    #                          xbins=10,xmin=0.0,xmax=10.0,
    #                          ybins=10,ymin=0.0,ymax=10.0)
    #    array.defineHistogram('c',title='C',path='Eta',
     #                         xbins=10,xmin=0.0,xmax=10.0)
    #    array = helper.addArray([4,2],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('a',title='A',path='EtaPhi',
     #                         xbins=10,xmin=0.0,xmax=10.0)
        # Using a map of groups
    #    layerList = ['layer1','layer2']
    #    clusterList = ['clusterX','clusterB']
    #    array = helper.addArray([layerList],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('c',title='C',path='Layer',
     #                         xbins=10,xmin=0,xmax=10.0)
    #    array = helper.addArray([layerList,clusterList],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('c',title='C',path='LayerCluster',
     #                         xbins=10,xmin=0,xmax=10.0)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #return helper.result()
    
    # # Otherwise, merge with result object and return
    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD


    ConfigFlags.Output.HISTFileName = 'CscMonitorOutput.root'
    ConfigFlags.Muon.doCSCs = True
    ConfigFlags.Muon.doRPCs = False
    ConfigFlags.Muon.doTGCs = False
    ConfigFlags.Muon.doMicromegas = False
    ConfigFlags.Detector.GeometryMuon=False
    ConfigFlags.Detector.GeometryCSC=True
    ConfigFlags.Detector.GeometryRPC=False
    ConfigFlags.Detector.GeometryTGC=False
    ConfigFlags.Detector.GeometryMM=False
    ConfigFlags.Detector.GeometryMDT=False
    #ConfigFlags.Muon.useAlignmentCorrections=False
    ConfigFlags.Muon.Align.UseILines = False
    #ConfigFlags.Muon.Align.UseAsBuilt = True
    ConfigFlags.Muon.Align.UseALines = False
    ConfigFlags.Muon.Align.UseBLines = False


    ConfigFlags.lock()
    ConfigFlags.dump()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cscMonitorAcc = CscMonitoringConfig(ConfigFlags)
    cfg.merge(cscMonitorAcc)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run(20) #use cfg.run(20) to only run on first 20 events
