#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from .CscMonUtils import getCSCLabelx


def CscMonitoringRAW_AlgOldConfig(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    helper = AthMonitorCfgHelperOld(inputFlags,'CscAthMonitorCfg')
    return helper.result()

def CscMonitoringRAW_AlgConfig(inputFlags):
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

    from AthenaConfiguration.ComponentFactory import CompFactory
    cscRdoMonAlg = helper.addAlgorithm(CompFactory.CscRdoValMonAlg,'CscRdoMonAlg')

    cscRdoMonAlg.cscNoiseCut = 100

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.
    # # Then, add a tool that doesn't have its own configuration function.
    from MuonConfig.MuonCSC_CnvToolsConfig import MuonCscRDODecoderCfg
    decodertool = result.popToolsAndMerge(MuonCscRDODecoderCfg(inputFlags))
    cscRdoMonAlg.CscRDODecoder = decodertool
 
    cscRdoGroup = helper.addGroup(cscRdoMonAlg,'CscRdoMonitor','Muon/MuonRawDataMonitoring/CSC/')
 
    #RDO
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_etacluswidth',cutmask='measuresEta',type='TH2F',title='ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=192,xmin=0.,xmax=192.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_phicluswidth',cutmask='measuresPhi_mon',type='TH2F',title='ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=48,xmin=0.,xmax=48.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('stripId,secLayer;h2csc_rdo_hitmap',type='TH2F',title='Hit Occupancy;channel;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('diff_max_mon;h1csc_rdo_maxdiffamp',type='TH1F',title='Max Amplitude in ROD Cluster;ADC counts;;',
                                 path='RDO/Shift',xbins=500,xmin=0.,xmax=5000)
    cscRdoGroup.defineHistogram('tmp_xVals_signal_mon,tmp_secLayer_sig_mon;h2csc_rdo_hitmap_signal',cutmask='isSignal',type='TH2F',title='Signal Occupancy;channel;[sector] + [0.2 #times layer]',
                                 path='RDO/Shift',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('tmp_xVals_signal_mon,tmp_secLayer_sig_mon;h2csc_rdo_hitmap_signal_EC',cutmask='sideC_signal',type='TH2F',title='EndCap C: Signal Occupancy;channel;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/RDO',xbins=242,xmin=-49.,xmax=193.,ybins=85,ymin=-17.,ymax=0.)
    cscRdoGroup.defineHistogram('tmp_xVals_signal_mon;h1csc_rdo_hitmap_signal_EC_count',cutmask='sideC_signal',type='TH1F',title='EndCap C: Signal Occupancy;channel;entries/channel',
                                 path='Overview/CSCEC/RDO',xbins=242,xmin=-49.,xmax=193.)
    thisLabelx=getCSCLabelx("labels_occupancy_signal_EC")
    cscRdoGroup.defineHistogram('tmp_secLayer_sig_mon;h1csc_rdo_hitmap_signal_EC_occupancy',cutmask='sideC_signal',type='TH1F',title='EndCap C: Signal Occupancy;;entries/layer',
                                 path='Overview/CSCEC/RDO',xbins=85,xmin=-17.,xmax=0.,xlabels=thisLabelx)
    cscRdoGroup.defineHistogram('tmp_xVals_signal_mon,tmp_secLayer_sig_mon;h2csc_rdo_hitmap_signal_EA',cutmask='sideA_signal',type='TH2F',title='EndCap A: Signal Occupancy;channel;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEA/RDO',xbins=242,xmin=-49.,xmax=193.,ybins=90,ymin=0.,ymax=18.)
    cscRdoGroup.defineHistogram('tmp_xVals_signal_mon;h1csc_rdo_hitmap_signal_EA_count',cutmask='sideA_signal',type='TH1F',title='EndCap A: Signal Occupancy;channel;entries/channel',
                                 path='Overview/CSCEA/RDO',xbins=242,xmin=-49.,xmax=193.)
    thisLabelx=getCSCLabelx("labels_occupancy_signal_EA")
    cscRdoGroup.defineHistogram('tmp_secLayer_sig_mon;h1csc_rdo_hitmap_signal_EA_occupancy',cutmask='sideA_signal',type='TH1F',title='EndCap A: Signal Occupancy;;[sector] + [0.2 #times layer];',
                                 path='Overview/CSCEA/RDO',xbins=90,xmin=0.,xmax=18.,xlabels=thisLabelx)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_etacluswidth_signal',cutmask='signal_measuresEta',type='TH2F',title='ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='RDO/Shift',xbins=192,xmin=0.,xmax=192.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_etacluswidth_signal_EC',cutmask='signal_measuresEta_sideC',type='TH2F',title='EndCap C: ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/RDO',xbins=192,xmin=0.,xmax=192.,ybins=85,ymin=-17.,ymax=0.)
    cscRdoGroup.defineHistogram('raw_clus_width;h1csc_rdo_etacluswidth_signal_EC_count',cutmask='signal_measuresEta_sideC',type='TH1F',title='EndCap C: ROD precision-cluster width;width(no.of strips);entries',
                                 path='Overview/CSCEC/RDO',xbins=192,xmin=0.,xmax=192.)
    thisLabelx=getCSCLabelx("labels_occupancy_signal_EC")
    cscRdoGroup.defineHistogram('secLayer;h1csc_rdo_etacluswidth_signal_EC_occupancy',cutmask='signal_measuresEta_sideC',type='TH1F',title='EndCap C: ROD precision-cluster width;;entries/layer',
                                 path='Overview/CSCEC/RDO',xbins=85,xmin=-17.,xmax=0.,xlabels=thisLabelx)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_etacluswidth_signal_EA',cutmask='signal_measuresEta_sideA',type='TH2F',title='EndCap A: ROD precision-cluster width;no.of strips;',
                                 path='Overview/CSCEA/RDO',xbins=192,xmin=0.,xmax=192.,ybins=90,ymin=0.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width;h1csc_rdo_etacluswidth_signal_EA_count',cutmask='signal_measuresEta_sideA',type='TH1F',title='EndCap A: ROD precision-cluster width;width(no.of strips);entries',
                                 path='Overview/CSCEA/RDO',xbins=192,xmin=0.,xmax=192.)
    thisLabelx=getCSCLabelx("labels_occupancy_signal_EA")
    cscRdoGroup.defineHistogram('secLayer;h1csc_rdo_etacluswidth_signal_EA_occupancy',cutmask='signal_measuresEta_sideA',type='TH1F',title='EndCap A: ROD precision-cluster width;;entries/layer',
                                 path='Overview/CSCEA/RDO',xbins=90,xmin=0.,xmax=18.,xlabels=thisLabelx)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_phicluswidth_signal',cutmask='signal_measuresPhi',type='TH2F',title='ROD transverse-cluster width;width(no.of strips);',
                                 path='RDO/Shift',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_phicluswidth_signal_EC',cutmask='signal_measuresPhi_sideC',type='TH2F',title='EndCap C: ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/RDO',xbins=48,xmin=0.,xmax=48.,ybins=85,ymin=-17.,ymax=0.)
    cscRdoGroup.defineHistogram('raw_clus_width;h1csc_rdo_phicluswidth_signal_EC_count',cutmask='signal_measuresPhi_sideC',type='TH1F',title='EndCap C: ROD transverse-cluster width;width(no.of strips);entries',
                                 path='Overview/CSCEC/RDO',xbins=48,xmin=0.,xmax=48.)
    thisLabelx=getCSCLabelx("labels_occupancy_signal_EC")
    cscRdoGroup.defineHistogram('secLayer;h1csc_rdo_phicluswidth_signal_EC_occupancy',cutmask='signal_measuresPhi_sideC',type='TH1F',title='EndCap C: ROD transverse-cluster width;;entries/layer',
                                 path='Overview/CSCEC/RDO',xbins=85,xmin=-17.,xmax=0.,xlabels=thisLabelx)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_phicluswidth_signal_EA',cutmask='signal_measuresPhi_sideA',type='TH2F',title='EndCap A: ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEA/RDO',xbins=48,xmin=0.,xmax=48.,ybins=90,ymin=0.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width;h1csc_rdo_phicluswidth_signal_EA_count',cutmask='signal_measuresPhi_sideA',type='TH1F',title='EndCap A: ROD transverse-cluster width;width(no.of strips);entries',
                                 path='Overview/CSCEA/RDO',xbins=48,xmin=0.,xmax=48.)
    thisLabelx=getCSCLabelx("labels_occupancy_signal_EA")
    cscRdoGroup.defineHistogram('secLayer;h1csc_rdo_phicluswidth_signal_EA_occupancy',cutmask='signal_measuresPhi_sideA',type='TH1F',title='EndCap A: ROD transverse-cluster width;;entries/layer',
                                 path='Overview/CSCEA/RDO',xbins=90,xmin=0.,xmax=18.,xlabels=thisLabelx)
    cscRdoGroup.defineHistogram('tmp_xVals_noise_mon,tmp_secLayer_noise_mon;h2csc_rdo_hitmap_noise',cutmask='isNoise',type='TH2F',title='Noise Occupancy;channel;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_etacluswidth_noise',cutmask='noise_measuresEta',type='TH2F',title='ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=192,xmin=0.,xmax=192,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('raw_clus_width,secLayer;h2csc_rdo_phicluswidth_noise',cutmask='noise_measuresPhi',type='TH2F',title='ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=48,xmin=0.,xmax=48,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('nPhiClusWidthCnt_mon,nEtaClusWidthCnt_mon;h2csc_rdo_eta_vs_phi_cluswidth',type='TH2F',title='Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width',
                                 path='RDO/Expert',xbins=100,xmin=0.,xmax=100.,ybins=100,ymin=0.,ymax=100.)
    cscRdoGroup.defineHistogram('count_mon,secLayer_count;h2csc_rdo_phicluscount',cutmask='mphi_mon',type='TH2F',title='ROD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=20,xmin=0.,xmax=20.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('scount_mon,secLayer_count;h2csc_rdo_phicluscount_signal',cutmask='mphi_scount',type='TH2F',title='ROD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                 path='RDO/Shift',xbins=20,xmin=0.,xmax=20.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('tmp_counts_phi_mon,secLayer_count;h2csc_rdo_phicluscount_noise',cutmask='mphi_mon',type='TH2F',title='ROD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=20,xmin=0.,xmax=20.,ybins=175,ymin=-17.,ymax=18.)

    cscRdoGroup.defineHistogram('count_mon,secLayer_count;h2csc_rdo_etacluscount',cutmask='mphi_inv',type='TH2F',title='ROD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=20,xmin=0.,xmax=20.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('scount_mon,secLayer_count;h2csc_rdo_etacluscount_signal',cutmask='mphi_inv_scount',type='TH2F',title='ROD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                 path='RDO/Shift',xbins=20,xmin=0.,xmax=20.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('tmp_counts_eta_mon,secLayer_count;h2csc_rdo_etacluscount_noise',cutmask='mphi_inv',type='TH2F',title='ROD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]',
                                 path='RDO/Expert',xbins=20,xmin=0.,xmax=20.,ybins=175,ymin=-17.,ymax=18.)
    cscRdoGroup.defineHistogram('numphi_mon,numeta_mon;h2csc_rdo_eta_vs_phi_cluscount',type='TH2F',title='Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='RDO/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0.,ymax=100.)
    cscRdoGroup.defineHistogram('numphisignal_mon,numetasignal_mon;h2csc_rdo_eta_vs_phi_cluscount_signal',type='TH2F',title='Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='RDO/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0.,ymax=100.)
    cscRdoGroup.defineHistogram('diffnumphi,diffnumeta;h2csc_rdo_eta_vs_phi_cluscount_noise',type='TH2F',title='Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='RDO/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0.,ymax=100.)

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
    ConfigFlags.Input.Files = defaultTestFiles.RAW


    ConfigFlags.Output.HISTFileName = 'CscRDOMonitorOutput.root'
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
    ConfigFlags.DQ.enableLumiAccess = True
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.Environment = 'tier0Raw'

    ConfigFlags.lock()
    ConfigFlags.dump()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg     
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(ByteStreamReadCfg(ConfigFlags))
    
    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    cfg.merge(CscBytestreamDecodeCfg( ConfigFlags, False))

    cscMonitorAcc = CscMonitoringRAW_AlgConfig(ConfigFlags)
    cfg.merge(cscMonitorAcc)

    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run(20) #use cfg.run(20) to only run on first 20 events
