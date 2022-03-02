#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

def PprMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Ppr algorithm in the monitoring system.'''

    import math 
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'PprMonitoringCfg')

    # get any algorithms
    PprMonAlg = helper.addAlgorithm(CompFactory.PprMonitorAlgorithm,'PprMonAlg')

    # add any steering
    groupName = 'PprMonitor' # the monitoring group name is also used for the package name
    PprMonAlg.PackageName = groupName

    # Steering properties 
    threshADC = 50
    PprMonAlg.TT_ADC_HitMap_Thresh = threshADC  # ADC cut for hit maps
 
    sliceNo = 15
    PprMonAlg.SliceNo = sliceNo  # Max number of timeslices in the readout
   
    threshVec = [0, 1, 3, 5, 10, 20, 30, 50] # LUT thresholds    
    PprMonAlg.LUTHitMap_ThreshVec = threshVec

    # Environment
    isOnline = inputFlags.Trigger.Online.isPartition 
 
    # Histogram paths
    mainDir = 'L1Calo'
    trigPath = 'PPM/'

    # add monitoring algorithm to group, with group name and main directory 
    groupLUTCP         = helper.addGroup(PprMonAlg, 'groupLUTCP', mainDir)
    groupLUTCP_EM      = helper.addGroup(PprMonAlg, 'groupLUTCP_EM', mainDir)
    groupLUTCP_HAD     = helper.addGroup(PprMonAlg, 'groupLUTCP_HAD', mainDir)
    groupLUTJEP        = helper.addGroup(PprMonAlg, 'groupLUTJEP', mainDir)
    groupLUTJEP_EM     = helper.addGroup(PprMonAlg, 'groupLUTJEP_EM', mainDir)
    groupLUTJEP_HAD    = helper.addGroup(PprMonAlg, 'groupLUTJEP_HAD', mainDir)
    groupADC_EM        = helper.addGroup(PprMonAlg, 'groupADC_EM', mainDir)
    groupADC_HAD       = helper.addGroup(PprMonAlg, 'groupADC_HAD', mainDir)
    groupTimeslice_EM  = helper.addGroup(PprMonAlg, 'groupTimeslice_EM', mainDir)
    groupTimeslice_HAD = helper.addGroup(PprMonAlg, 'groupTimeslice_HAD', mainDir)
    groupTimeslice     = helper.addGroup(PprMonAlg, 'groupTimeslice', mainDir)
    groupErrors_EM     = helper.addGroup(PprMonAlg, 'groupErrors_EM', mainDir)
    groupErrors_HAD    = helper.addGroup(PprMonAlg, 'groupErrors_HAD', mainDir)
   

    # Trigger tower plots: eta-phi granularity
    etabins = [-4.9,-4.475,-4.050,-3.625,-3.2,-3.1,-2.9,
                     -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
                     -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
                     -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
                     -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
                     0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
                     1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
                     3.1,3.2,3.625,4.050,4.475,4.9]

    etabins_HAD_1D = [-4.9,-4.050,-3.2,-3.1,-2.9,
                   -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
                   -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
                   -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
                   -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
                   0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
                   1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
                   3.1,3.2,4.050,4.9]

    phibins = 64
    phimin = 0
    phimax_2d = 64
    phimax_1d = 2.*math.pi 
    maxEnergyRange = 256
    bcn = 3564 # Max number of bunches

    
    #######################   
    # PPM inputs (LUT-CP) #
    #######################
    histPath = trigPath+'/LUT-CP/Distributions'

    # LUT per BCN (Both EM & HAD together)
    groupLUTCP.defineHistogram('BCID;ppm_1d_tt_lutcp_LutPerBCN', 
                                title='Number of LUT-CP > 5 GeV/2 per BC; Bunch crossing; # of LUT above limit', 
                                type='TH1F', path=histPath, xbins=bcn, xmin=0, xmax=bcn, 
                                cutmask='mask_cpET_5')
    
    # EM distributions
    groupLUTCP_EM.defineHistogram('eta_TT;ppm_em_1d_tt_lutcp_Eta', 
                                   title='EM LUT-CP: Distribution of peak in #eta; #eta', 
                                   type='TH1F', path=histPath, xbins=etabins, 
                                   cutmask='mask_cpET_0')

    groupLUTCP_EM.defineHistogram('phiTT_1D;ppm_em_1d_tt_lutcp_Phi', 
                                   title='EM LUT-CP: Distribution of peak in #phi; #phi', 
                                   type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d)
 
    groupLUTCP_EM.defineHistogram('cpET_TT;ppm_em_1d_tt_lutcp_Et', 
                                   title='EM LUT-CP: Distribution of peak; EM LUT peak [GeV/2]', 
                                   type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, 
                                   cutmask='mask_cpET_0')

    # HAD distributions
    groupLUTCP_HAD.defineHistogram('eta_TT;ppm_had_1d_tt_lutcp_Eta', 
                                    title='HAD LUT-CP: Distribution of peak in #eta; #eta', 
                                    type='TH1F', path=histPath, xbins=etabins_HAD_1D, 
                                    cutmask='mask_cpET_0')

    groupLUTCP_HAD.defineHistogram('phiTT_1D;ppm_had_1d_tt_lutcp_Phi', 
                                    title='HAD LUT-CP: Distribution of peak in #phi; #phi', 
                                    type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d)
 
    groupLUTCP_HAD.defineHistogram('cpET_TT;ppm_had_1d_tt_lutcp_Et', 
                                    title='HAD LUT-CP: Distribution of peak; HAD LUT peak [GeV/2]', 
                                    type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, 
                                    cutmask='mask_cpET_0')

    # Eta-phi maps
    histPath = trigPath+'/LUT-CP/EtaPhiMaps'

    groupLUTCP_EM.defineHistogram('etaTT_2D,phiTT_2D,cpET_TT_2D;ppm_em_2d_etaPhi_tt_lutcp_AverageEt', 
                                   title='EM Average LUT-CP Et for Et > 5 GeV/2', 
                                   type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)
       
    groupLUTCP_HAD.defineHistogram('etaTT_2D,phiTT_2D,cpET_TT_2D;ppm_had_2d_etaPhi_tt_lutcp_AverageEt', 
                                    title='HAD Average LUT-CP Et for Et > 5 GeV/2', 
                                    type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)

    # Eta-phi maps per threshold (low stat). 
    # Offline: fill on a per-LB basis and then merge into low-stat plots
    # Online: refresh every 10 LB

    layers = ['EM', 'HAD']
    iThresh = list(range(0, len(threshVec)))

    for layer in layers:
        for i in iThresh:
            groupLUTCP_LB = helper.addGroup(PprMonAlg, 'groupLUTCP_{0}_{1}_LB'.format(layer, threshVec[i]), mainDir)
            groupLUTCP_LB.defineHistogram('etaTT_2D,phiTT_2D;ppm_{0}_2d_etaPhi_tt_lutcp_Threshold_0{1}'.format(layer.lower(), i), 
                                           title='#eta - #phi map of {0} LUT-CP > {1} GeV/2'.format(layer, threshVec[i]), 
                                           type='TH2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, 
                                           opt='kLBNHistoryDepth=10, kAlwaysCreate' if isOnline else 'kAlwaysCreate', 
                                           duration='' if isOnline else 'lb')
                  

    ########################   
    # PPM inputs (LUT-JEP) #
    ########################
    histPath = trigPath+'/LUT-JEP/Distributions'
   
    # LUT per BCN
    groupLUTJEP.defineHistogram('BCID;ppm_1d_tt_lutjep_LutPerBCN', 
                                 title='Number of LUT-JEP > 5 GeV per BC; Bunch crossing; # of LUT above limit', 
                                 type='TH1F', path=histPath, xbins=bcn, xmin=0, xmax=bcn, 
                                 cutmask='mask_jepET_5')
 
    # EM distributions
    groupLUTJEP_EM.defineHistogram('eta_TT;ppm_em_1d_tt_lutjep_Eta', 
                                    title='EM LUT-JEP: Distribution of peak in #eta', 
                                    type='TH1F', path=histPath, xbins=etabins, 
                                    cutmask='mask_jepET_0')

    groupLUTJEP_EM.defineHistogram('phiTT_1D;ppm_em_1d_tt_lutjep_Phi', 
                                    title='EM LUT-JEP: Distribution of peak in #phi; #phi', 
                                    type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d)

    groupLUTJEP_EM.defineHistogram('jepET_TT;ppm_em_1d_tt_lutjep_Et', 
                                    title='EM LUT-JEP: Distribution of peak; EM LUT peak [GeV]', 
                                    type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, 
                                    cutmask='mask_jepET_0')   

    # HAD distributions
    groupLUTJEP_HAD.defineHistogram('eta_TT;ppm_had_1d_tt_lutjep_Eta', 
                                     title='HAD LUT-JEP: Distribution of peak in #eta', 
                                     type='TH1F', path=histPath, xbins=etabins_HAD_1D, 
                                     cutmask='mask_jepET_0')

    groupLUTJEP_HAD.defineHistogram('phiTT_1D;ppm_had_1d_tt_lutjep_Phi', 
                                     title='HAD LUT-JEP: Distribution of peak in #phi; #phi', 
                                     type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d)

    groupLUTJEP_HAD.defineHistogram('jepET_TT;ppm_had_1d_tt_lutjep_Et', 
                                     title='HAD LUT-JEP: Distribution of peak; HAD LUT peak [GeV]', 
                                     type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, 
                                     cutmask='mask_jepET_0')   

    # Eta-phi maps
    histPath = trigPath+'/LUT-JEP/EtaPhiMaps'

    groupLUTJEP_EM.defineHistogram('etaTT_2D,phiTT_2D,jepET_TT_2D;ppm_em_2d_etaPhi_tt_lutjep_AverageEt', 
                                    title='EM Average LUT-JEP Et for Et > 5 GeV', 
                                    type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)

    groupLUTJEP_HAD.defineHistogram('etaTT_2D,phiTT_2D,jepET_TT_2D;ppm_had_2d_etaPhi_tt_lutjep_AverageEt', 
                                     title='HAD Average LUT-JEP Et for Et > 5 GeV', 
                                     type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)
    
    for layer in layers:
        for i in iThresh:
            groupLUTJEP_LB = helper.addGroup(PprMonAlg, 'groupLUTJEP_{0}_{1}_LB'.format(layer, threshVec[i]), mainDir) 
            groupLUTJEP_LB.defineHistogram('etaTT_2D,phiTT_2D;ppm_{0}_2d_etaPhi_tt_lutjep_Threshold_0{1}'.format(layer.lower(), i), 
                                            title='#eta - #phi map of {0} LUT-JEP > {1} GeV'.format(layer, threshVec[i]), 
                                            type='TH2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, 
                                            opt='kLBNHistoryDepth=10, kAlwaysCreate' if isOnline else 'kAlwaysCreate', 
                                            duration='' if isOnline else 'lb') 


    ####################
    # PPM inputs (ADC) #
    ####################
    histPath = trigPath+'/ADC/EtaPhiMaps'
    
    # EM tower maps 
    groupADC_EM.defineHistogram('etaTT_2D,phiTT_2D;ppm_em_2d_etaPhi_tt_adc_HitMap', 
                                 title='#eta - #phi map of EM FADC > {0} for triggered timeslice; Tower #eta; Tower #phi'.format(threshADC), 
                                 type='TH2F', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d) 
   
    groupADC_EM.defineHistogram('etaTT_2D,phiTT_2D,adcTT;ppm_em_2d_etaPhi_tt_adc_ProfileHitMap', 
                                 title='#eta - #phi profile map of EM FADC > {0} for triggered timeslice; Tower #eta; Tower #phi'.format(threshADC), 
                                 type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)
 
    # HAD tower maps 
    groupADC_HAD.defineHistogram('etaTT_2D,phiTT_2D;ppm_had_2d_etaPhi_tt_adc_HitMap', 
                                  title='#eta - #phi map of HAD FADC > {0} for triggered timeslice; Tower #eta; Tower #phi'.format(threshADC),
                                  type='TH2F', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)
 
    groupADC_HAD.defineHistogram('etaTT_2D,phiTT_2D,adcTT;ppm_had_2d_etaPhi_tt_adc_ProfileHitMap', 
                                  title='#eta - #phi profile map of HAD FADC > {0} for triggered timeslice; Tower #eta; Tower #phi'.format(threshADC), 
                                  type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)
 
    # Triggered time-slice
    histPath = trigPath+'/ADC/Timeslices'

    groupTimeslice_EM.defineHistogram('adcPeak;ppm_em_1d_tt_adc_TriggeredSlice', 
                                       title='Number of the EM triggered slice; # Slice', 
                                       type='TH1F', path=histPath, xbins=sliceNo, xmin=0, xmax=sliceNo) 
 
    groupTimeslice_HAD.defineHistogram('adcPeak;ppm_had_1d_tt_adc_TriggeredSlice', 
                                        title='Number of the HAD triggered slice; # Slice', 
                                        type='TH1F', path=histPath, xbins=sliceNo, xmin=0, xmax=sliceNo)   
 
    groupTimeslice_EM.defineHistogram('maxADC;ppm_em_1d_tt_adc_MaxTimeslice', 
                                       title='EM distribution of maximum timeslice; slice', 
                                       type='TH1D', path=histPath, xbins=sliceNo, xmin=0, xmax=sliceNo) 

    groupTimeslice_HAD.defineHistogram('maxADC;ppm_had_1d_tt_adc_MaxTimeslice', 
                                        title='HAD distribution of maximum timeslice; slice', 
                                        type='TH1D', path=histPath, xbins=sliceNo, xmin=0, xmax=sliceNo) 
    
    groupTimeslice_EM.defineHistogram('etaTT_2D,phiTT_2D,maxADCPlus1;ppm_em_2d_etaPhi_tt_adc_MaxTimeslice', 
                                       title='Average maximum timeslice for EM signal (TS:1-15); Tower #eta; Tower #phi', 
                                       type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d) 

    groupTimeslice_HAD.defineHistogram('etaTT_2D,phiTT_2D,maxADCPlus1;ppm_had_2d_etaPhi_tt_adc_MaxTimeslice', 
                                        title='Average maximum timeslice for HAD signal (TS:1-15); Tower #eta; Tower #phi', 
                                        type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d)

    # Bits of BCID logic word 
    bcidBitsLabels = ['none (40 MHz)', 'satBC only', 'peakF only', 'satBC & peakF', 'sat80BC & peakF', 'sat80BC & sat40BC', 'sat80BC only']
    groupTimeslice.defineHistogram('bcidBits,adcBCID;ppm_2d_tt_adc_BcidBits', 
                                    title='PPM: PeakADC Vs. Bits of BCID Logic Word', 
                                    type='TH2I', path=histPath, xbins=7, xmin=0, xmax=7, xlabels=bcidBitsLabels, ybins=1024, ymin=0, ymax=1024)
   
    # High/low threshold pass cases (Sat80 BCID)
    sat80Labels = ['no saturated ADC', 'none/none', 'none/low', 'none/high', 'low/low', 'low/high', 'high/high', 'else']
    groupTimeslice.defineHistogram('sat80Word;ppm_1d_tt_adc_HLCase', 
                                    title= 'PPM: Sat80 thresholds passed by ADC[n-2.5] / ADC[n-1.5]', 
                                    type='TH1I', path=histPath, xbins=8, xmin=0, xmax=8, xlabels=sat80Labels)

    # Signal shape profiles
    partitionsEM = ['LArFCAL1C', 'LArEMECC', 'LArOverlapC', 'LArEMBC', 'LArEMBA', 'LArOverlapA', 'LArEMECA', 'LArFCAL1A']
    partitionsHAD = [ 'LArFCAL23C', 'LArHECC', 'TileEBC', 'TileLBC', 'TileLBA', 'TileEBA', 'LArHECA', 'LArFCAL23A']

    signalsEM = helper.addArray([partitionsEM], PprMonAlg, 'groupTimeslice_EM', topPath=mainDir)
    signalsEM.defineHistogram('slice,wADC;ppm_em_1d_tt_adc_SignalProfile{0}', 
                               title='Signal Shape Profile for {0}; Timeslice', 
                               type='TProfile', path=histPath, xbins=sliceNo, xmin=0, xmax=sliceNo)

    signalsHAD = helper.addArray([partitionsHAD], PprMonAlg, 'groupTimeslice_HAD', topPath=mainDir)
    signalsHAD.defineHistogram('slice,wADC;ppm_had_1d_tt_adc_SignalProfile{0}', 
                                title='Signal Shape Profile for {0}; Timeslice', 
                                type='TProfile', path=histPath, xbins=sliceNo, xmin=0, xmax=sliceNo)
    
    ####################
    #      Errors      #
    ####################
  
    # Note: use opt='kAlwaysCreate' for error plots so that empty plots will still be published, for sanity checks

    histPath = trigPath+'Errors'

    # Pedestal correction over-/underflows (EM)
    groupErrors_EM.defineHistogram('etaTT;ppm_em_1d_pedOverflow_Eta', 
                                    title='EM : Overflow of pedestal correction;#eta', 
                                    type='TH1F', path=histPath, xbins=etabins, 
                                    cutmask='mask_PedCorrOverflow', 
                                    opt='kAlwaysCreate')
   
    groupErrors_EM.defineHistogram('etaTT;ppm_em_1d_pedUnderflow_Eta', 
                                    title='EM : Underflow of pedestal correction;#eta', 
                                    type='TH1F', path=histPath, xbins=etabins, 
                                    cutmask='mask_PedCorrUnderflow', 
                                    opt='kAlwaysCreate')

    # Pedestal correction over-/underflows (HAD)
    groupErrors_HAD.defineHistogram('etaTT;ppm_had_1d_pedOverflow_Eta', 
                                     title='HAD : Overflow of pedestal correction;#eta', 
                                     type='TH1F', path=histPath, xbins=etabins_HAD_1D, 
                                     cutmask='mask_PedCorrOverflow', 
                                     opt='kAlwaysCreate')

    groupErrors_HAD.defineHistogram('etaTT;ppm_had_1d_pedUnderflow_Eta', 
                                     title='HAD : Underflow of pedestal correction;#eta', 
                                     type='TH1F', path=histPath, xbins=etabins_HAD_1D, 
                                     cutmask='mask_PedCorrUnderflow', 
                                     opt='kAlwaysCreate') 
    
     

    # Finish up

    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
    # For direct tests
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # set debug level for whole job
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO 
    log.setLevel(INFO)

    # set input file and config options
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob

    inputs = glob.glob('/eos/atlas/atlascerngroupdisk/data-art/build-output/master/Athena/x86_64-centos7-gcc8-opt/2020-04-06T2139/TrigP1Test/test_trigP1_v1PhysP1_T0Mon_build/ESD.pool.root')

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_LVL1.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    PprMonitorCfg = PprMonitoringConfig(ConfigFlags)
    cfg.merge(PprMonitorCfg)

    # message level for algorithm
    PprMonitorCfg.getEventAlgo('PprMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=True, summariseProps = True)

    nevents=-1
    status = cfg.run(nevents)
    if status.isFailure():
        import sys
        sys.exit(-1)

