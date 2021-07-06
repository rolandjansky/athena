#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
def JepJemMonitoringConfig(inputFlags):
    '''Function to configure LVL1 JepJem algorithm in the monitoring system.'''

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
    helper = AthMonitorCfgHelper(inputFlags,'JepJemMonitoringCfg')

    # get any algorithms
    JepJemMonAlg = helper.addAlgorithm(CompFactory.JepJemMonitorAlgorithm,'JepJemMonAlg')

    # add any steering
    groupName = 'JepJemMonitor' # the monitoring group name is also used for the package name
    JepJemMonAlg.PackageName = groupName
    maxSlices = 5
    JepJemMonAlg.s_maxSlices = maxSlices

    # histogram paths
    mainDir = 'L1Calo'
    trigPath = 'JepJem/'
    inputPath = trigPath + '/Input/'
    # outputPath = trigPath + '/Output/'
    # errorPath = trigPath + '/Errors/'
    # monRoIPath = outputPath + '/RoI/'
    # monThrePath = outputPath + '/Thresholds/'
    # monESumPath = outputPath + '/EnergySums/'
    # monHardPath = errorPath + '/Hardware/'

    # phi bins (auto)
    phibins = 64
    phibins_2d = 32
    phimin = 0.
    phimin_2d = 0.
    phimax = 2.*math.pi

    # eta bin edges taken from TrigT1CaloLWHistogramTool
    etabins = [-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,
               -1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,
               0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,
               2.0,2.2,2.4,2.7,2.9,3.2,4.9]
    maxEnergyRange = 256

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(JepJemMonAlg, groupName , mainDir)

    # add run number plot
    myGroup.defineHistogram('run',title='Run Number;run;Events',
                            path=trigPath,xbins=1000000,xmin=-0.5,xmax=999999.5)
    
    # for 2D histograms x,y;histogram alias
    myGroup.defineHistogram('etaJE,phiJE;ppm_em_2d_etaPhi_je_Hitmap',title='JEPJEM Jet Element EM eta/phi;Element #eta; Element #phi',type='TH2F',
                            cutmask='mask_em_JE', path=inputPath,
                            xbins=etabins, ybins=phibins_2d, ymin=phimin_2d, ymax=phimax)
    myGroup.defineHistogram('etaJE,phiJE;ppm_had_2d_etaPhi_tt_Hitmap',title='JEPJEM Jet Element HAD eta/phi;Element #eta; Element #phi',type='TH2F',
                            cutmask='mask_had_JE', path=inputPath,
                            xbins=etabins, ybins=phibins_2d, ymin=phimin_2d, ymax=phimax)


    #### HISTOS ####
    # EM 1D
    myGroup.defineHistogram('phiJepJemJE_em;m_h_jem_em_1d_jetEl_Phi', title='EM TowerSum distribution per #phi  --  JEM input;#phi', type='TH1F', 
                            path=inputPath, cutmask='',
                            xbins=phibins, xmin=phimin, xmax=phimax)
    myGroup.defineHistogram('etaJepJemJE_em;m_h_jem_em_1d_jetEl_Eta', title='EM TowerSum distribution per #eta  --  JEM input;#eta', type='TH1F', 
                            path=inputPath,  cutmask='',
                            xbins=etabins)
    myGroup.defineHistogram('etJepJemJE_em;jem_em_1d_jetEl_Energy', title='EM TowerSum energy distribution  --  JEM input;em energy [GeV]', type='TH1F', 
                            path=inputPath, cutmask='',
                            xbins=maxEnergyRange, xmin=0, xmax=maxEnergyRange)

    # HAD 1D
    myGroup.defineHistogram('phiJepJemJE_had;m_h_jem_had_1d_jetEl_Phi', title='HAD TowerSum distribution per #phi  --  JEM input;#phi', type='TH1F', 
                            path=inputPath, cutmask='',
                            xbins=phibins, xmin=phimin, xmax=phimax)
    myGroup.defineHistogram('etaJepJemJE_had;m_h_jem_had_1d_jetEl_Eta', title='HAD TowerSum distribution per #eta  --  JEM input;#eta', type='TH1F',
                            path=inputPath, cutmask='',
                            xbins=etabins)
    myGroup.defineHistogram('etJepJemJE_had;jem_had_1d_jetEl_Energy', title='HAD TowerSum energy distribution  --  JEM input;had energy [GeV]', type='TH1F', 
                            path=inputPath, cutmask='',
                            xbins=maxEnergyRange, xmin=0, xmax=maxEnergyRange)

    # EM 2D
    myGroup.defineHistogram('etaJepJemJE_em,phiJepJemJE_em;jepjem_em_2d_etaPhi_je_EtWeighted',
                            title='JEPJEM Element EM eta/phi weighted;Element #eta; Element #phi',type='TH2F',
                            cutmask='', path=inputPath,
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax, weight="etJepJemJE_em")
    myGroup.defineHistogram('etaJepJepJE_em,phiScaledJepJemJE_em;jepjem_em_2d_etaPhi_je_Hitmap',
                            title='JEPJEM Element EM eta/phi;Element #eta; Element #phi',type='TH2F',
                            cutmask='', path=inputPath,
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    # HAD 2D
    myGroup.defineHistogram('etaJepJemJE_had,phiJepJemJE_had;jepjem_had_2d_etaPhi_je_EtWeighted',
                            title='JEPJEM Element HAD eta/phi weighted;Element #eta; Element #phi',type='TH2F',
                            cutmask='',path=inputPath,
                            xbins=etabins,ybins=phibins,ymin=phimin,ymax=phimax, weight="etJepJemJE_had")
    myGroup.defineHistogram('etaJepJepJE_had,phiScaledJepJemJE_had;jepjem_had_2d_etaPhi_je_Hitmap',
                            title='JEPJEM Element HAD eta/phi;Element #eta; Element #phi',type='TH2F',
                            cutmask='', path=inputPath,
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    # Timeslices
    for i in range(maxSlices):
        # EM
        myGroup.defineHistogram('etaSliceJepJemJE_em_num{0},phiSliceJepJemJE_em_num{0};jepjem_em_2d_je_Slice_{0}'.format(i),
                                title='JEPJEM #eta - #phi map of EM TowerSum for Timeslice {0};Number of Slices;Slice num {0}'.format(i), type='TH2F',
                                cutmask='', path=inputPath,
                                xbins=etabins, ybins=phibins, ymin=0, ymax=phimax)
        # HAD
        myGroup.defineHistogram('etaSliceJepJemJE_had_num{0},phiSliceJepJemJE_had_num{0};jepjem_had_2d_je_Slice_{0}'.format(i),
                                title='JEPJEM #eta - #phi map of HAD TowerSum for Timeslice {0};Number of Slices; Slice num {0}'.format(i), type='TH2F',
                                cutmask='', path=inputPath,
                                xbins=etabins, ybins=phibins, ymin=0, ymax=phimax)

    acc = helper.result()
    result.merge(acc)
    return result


if __name__ == '__main__':
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

    JepJemMonitorCfg = JepJemMonitoringConfig(ConfigFlags)
    cfg.merge(JepJemMonitorCfg)

    # message level for algorithm
    JepJemMonitorCfg.getEventAlgo('JepJemMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)
