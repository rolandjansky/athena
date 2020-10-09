#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def PprMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Ppr algorithm in the monitoring system.'''

    import math 
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # any things that need setting up for job e.g.
    #from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    #result.merge(AtlasGeometryCfg(inputFlags))

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
 
    # Histogram paths
    mainDir = 'L1Calo'
    trigPath = 'PPM/'

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(PprMonAlg, groupName , mainDir)


    # Trigger tower plots: eta-phi granularity
    etabins = [-4.9,-4.475,-4.050,-3.625,-3.2,-3.1,-2.9,
                     -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
                     -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
                     -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
                     -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
                     0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
                     1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
                     3.1,3.2,3.625,4.050,4.475,4.9]
    phibins = 64
    phimin = 0
    phimax_2d = 64
    phimax_1d = 2.*math.pi 
    maxEnergyRange = 256

   
    #######################   
    # PPM inputs (LUT-CP) #
    #######################
    histPath = trigPath+'/LUT-CP/Distributions'
    
    # EM distributions
    myGroup.defineHistogram('etaTT_EM;h_ppm_em_1d_tt_lutcp_Eta', title='EM LUT-CP: Distribution of peak in #eta; #eta', type='TH1F', path=histPath, xbins=etabins, cutmask='mask_EM_cpET_0_noPhi')

    myGroup.defineHistogram('phiTT_1d_EM;h_ppm_em_1d_tt_lutcp_Phi', title='EM LUT-CP: Distribution of peak in #phi; #phi', type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d, cutmask='mask_EM_cpET_0_phiBins')
 
    myGroup.defineHistogram('cpET_EM;h_ppm_em_1d_tt_lutcp_Et', title='EM LUT-CP: Distribution of peak; EM LUT peak [GeV/2]', type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, cutmask='mask_EM_cpET_0_noPhi')

    # HAD distributions
    myGroup.defineHistogram('etaTT_HAD;h_ppm_had_1d_tt_lutcp_Eta', title='HAD LUT-CP: Distribution of peak in #eta; #eta', type='TH1F', path=histPath, xbins=etabins, cutmask='mask_HAD_cpET_0_noPhi')

    myGroup.defineHistogram('phiTT_1d_HAD;h_ppm_had_1d_tt_lutcp_Phi', title='HAD LUT-CP: Distribution of peak in #phi; #phi', type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d, cutmask='mask_HAD_cpET_0_phiBins')
 
    myGroup.defineHistogram('cpET_HAD;h_ppm_had_1d_tt_lutcp_Et', title='HAD LUT-CP: Distribution of peak; HAD LUT peak [GeV/2]', type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, cutmask='mask_HAD_cpET_0_noPhi')

    # Eta-phi maps
    histPath = trigPath+'/LUT-CP/EtaPhiMaps'

    myGroup.defineHistogram('etaTT_EM,phiTT_2d_EM,cpET_EM;h_ppm_em_2d_etaPhi_tt_lutcp_AverageEt', title='EM Average LUT-CP Et for Et > 5 GeV/2', type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_EM_cpET_5_phiBins')
    
    myGroup.defineHistogram('etaTT_HAD,phiTT_2d_HAD,cpET_HAD;h_ppm_had_2d_etaPhi_tt_lutcp_AverageEt', title='HAD Average LUT-CP Et for Et > 5 GeV/2', type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_HAD_cpET_5_phiBins')

    
    ########################   
    # PPM inputs (LUT-JEP) #
    ########################
    histPath = trigPath+'/LUT-JEP/Distributions'
    
    # EM distributions
    myGroup.defineHistogram('etaTT_EM;h_ppm_em_1d_tt_lutjep_Eta', title='EM LUT-JEP: Distribution of peak in #eta', type='TH1F', path=histPath, xbins=etabins, cutmask='mask_EM_jepET_0_noPhi')

    myGroup.defineHistogram('phiTT_1d_EM;h_ppm_em_1d_tt_lutjep_Phi', title='EM LUT-JEP: Distribution of peak in #phi; #phi', type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d, cutmask='mask_EM_jepET_0_phiBins')

    myGroup.defineHistogram('jepET_EM;h_ppm_em_1d_tt_lutjep_Et', title='EM LUT-JEP: Distribution of peak; EM LUT peak [GeV]', type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, cutmask='mask_EM_jepET_0_noPhi')   

    # HAD distributions
    myGroup.defineHistogram('etaTT_HAD;h_ppm_had_1d_tt_lutjep_Eta', title='HAD LUT-JEP: Distribution of peak in #eta', type='TH1F', path=histPath, xbins=etabins, cutmask='mask_HAD_jepET_0_noPhi')

    myGroup.defineHistogram('phiTT_1d_HAD;h_ppm_had_1d_tt_lutjep_Phi', title='HAD LUT-JEP: Distribution of peak in #phi; #phi', type='TH1F', path=histPath, xbins=phibins, xmin=phimin, xmax=phimax_1d, cutmask='mask_HAD_jepET_0_phiBins')

    myGroup.defineHistogram('jepET_HAD;h_ppm_had_1d_tt_lutjep_Et', title='HAD LUT-JEP: Distribution of peak; HAD LUT peak [GeV]', type='TH1F', path=histPath, xbins=maxEnergyRange-1, xmin=1, xmax=maxEnergyRange, cutmask='mask_HAD_jepET_0_noPhi')   

    # Eta-phi maps
    histPath = trigPath+'/LUT-JEP/EtaPhiMaps'

    myGroup.defineHistogram('etaTT_EM,phiTT_2d_EM,jepET_EM;h_ppm_em_2d_etaPhi_tt_lutjep_AverageEt', title='EM Average LUT-JEP Et for Et > 5 GeV', type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_EM_jepET_5_phiBins')

    myGroup.defineHistogram('etaTT_HAD,phiTT_2d_HAD,jepET_HAD;h_ppm_had_2d_etaPhi_tt_lutjep_AverageEt', title='HAD Average LUT-JEP Et for Et > 5 GeV', type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_HAD_jepET_5_phiBins')


    ####################
    # PPM inputs (ADC) #
    ####################
    histPath = trigPath+'/ADC/EtaPhiMaps'
    
    # EM tower maps 
    myGroup.defineHistogram('etaTT_EM,phiTT_2d_EM;h_ppm_em_2d_etaPhi_tt_adc_HitMap', title='#eta - #phi map of EM FADC > ' +str(threshADC)+ ' for triggered timeslice; Tower #eta; Tower #phi', type='TH2F', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_EM_timeslice') 
   
    myGroup.defineHistogram('etaTT_EM,phiTT_2d_EM,emTT_ADC;h_ppm_em_2d_etaPhi_tt_adc_ProfileHitMap', title='#eta - #phi profile map of EM FADC > ' +str(threshADC)+ ' for triggered timeslice; Tower #eta; Tower #phi', type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_EM_timeslice') 

 
    # HAD tower maps 
    myGroup.defineHistogram('etaTT_HAD,phiTT_2d_HAD;h_ppm_had_2d_etaPhi_tt_adc_HitMap', title='#eta - #phi map of HAD FADC > ' +str(threshADC)+ ' for triggered timeslice; Tower #eta; Tower #phi', type='TH2F', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_HAD_timeslice')
 
    myGroup.defineHistogram('etaTT_HAD,phiTT_2d_HAD,hadTT_ADC;h_ppm_had_2d_etaPhi_tt_adc_ProfileHitMap', title='#eta - #phi profile map of HAD FADC > ' +str(threshADC)+ ' for triggered timeslice; Tower #eta; Tower #phi', type='TProfile2D', path=histPath, xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax_2d, cutmask='mask_HAD_timeslice')
 
  
    
    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
    # For direct tests
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # set debug level for whole job
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO #DEBUG
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
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    status = cfg.run(nevents)
    if status.isFailure():
        import sys
        sys.exit(-1)

