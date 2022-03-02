#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def L1CaloL1TopoMonitoringConfig(inputFlags):
    '''Function to configure LVL1 L1CaloL1Topo algorithm in the monitoring system.'''

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
    helper = AthMonitorCfgHelper(inputFlags,'L1CaloL1TopoMonitoringCfg')

    # get any algorithms
    L1CaloL1TopoMonAlg = helper.addAlgorithm(CompFactory.L1CaloL1TopoMonitorAlgorithm,'L1CaloL1TopoMonAlg')

    # add any steering
    groupName = 'L1CaloL1TopoMonitor' # the monitoring group name is also used for the package name
    L1CaloL1TopoMonAlg.PackageName = groupName

    mainDir = 'LVL1_Interfaces'
    trigPath = 'L1Topo/'

    MAXTOBS=20
    # phi bins (auto)
    phibins = 32
    phimin = 0.
    phimax = 2.*math.pi

    # eta bin edges taken from TrigT1CaloLWHistogramTool
    etabins = [-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,
               -1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,
               0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,
               2.0,2.2,2.4,2.7,2.9,3.2,4.9]

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(L1CaloL1TopoMonAlg, groupName , mainDir)

    # run number plot
    myGroup.defineHistogram(
        'run',title='Run Number;run;Events',
        path=trigPath,xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Error summary plot for the entire algorithm
    error_labels = ['Calo conv','No CMX','DAQ conv','No DAQ','ROI conv',
                    'Fibre CRC','Payload CRC','CMX-Topo match']
    myGroup.defineHistogram(
        'sumErrors;l1topo_1d_Errors',
        path=trigPath, type='TH1F',
        title='Counts of errors;;Entries',cutmask='',
        opt='kAlwaysCreate',
        xlabels=error_labels, xbins=len(error_labels),
        xmin=0, xmax=len(error_labels))

    # Errors ordered by lumiblock
    myGroup.defineHistogram(
        'lbErrors;l1topo_1d_ErrorsByLumiblock',
        path=trigPath,type='TH1F',cutmask='',
        opt='kAlwaysCreate',
        title='Events with Errors by Lumiblock;Lumi Block;Number of Events',
        xbins=2500,xmin=0,xmax=2500)

    # TOB plots
    myGroup.defineHistogram(
        'jetTobsEnergyL;l1topo_1d_JetTobs_EnergyLg', weight='wFPGA',
        title='L1Topo-Jet TOB Energy Large Window Size;Energy;Events',
        path=trigPath,xbins=256,xmin=0,xmax=1024)

    myGroup.defineHistogram(
        'nJetTobs;l1topo_1d_DAQJetTobs',
        title='Number of L1Topo DAQ Jet TOBs;Tobs;Events',
        path=trigPath,xbins=MAXTOBS,xmin=0,xmax=MAXTOBS)

    myGroup.defineHistogram(
        'nTauTobs;l1topo_1d_DAQTauTobs',
        title='Number of L1Topo DAQ Tau TOBs;Tobs;Events',
        path=trigPath,xbins=MAXTOBS,xmin=0,xmax=MAXTOBS)

    myGroup.defineHistogram(
        'nEMTobs;l1topo_1d_DAQEMTobs',
        title='Number of L1Topo DAQ EM TOBs;Tobs;Events',
        path=trigPath,xbins=MAXTOBS,xmin=0,xmax=MAXTOBS)

    myGroup.defineHistogram(
        'nMuonTobs;l1topo_1d_DAQMuonTobs',
        title='Number of L1Topo DAQ Muon TOBs;Tobs;Events',
        path=trigPath,xbins=MAXTOBS,xmin=0,xmax=MAXTOBS)

    # 2D hitmaps matched eta-phi
    myGroup.defineHistogram('etaJetSTobs_match,phiJetSTobs_match;l1topo_2d_JetSTobs_etaPhi_match',
                            title='CMX-L1Topo matched small jet TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    myGroup.defineHistogram('etaJetLTobs_match,phiJetLTobs_match;l1topo_2d_JetLTobs_etaPhi_match',
                            title='CMX-L1Topo matched large jet TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    myGroup.defineHistogram('etaTauTobs_match,phiTauTobs_match;l1topo_2d_TauTobs_etaPhi_match',
                            title='CMX-L1Topo matched tau TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    myGroup.defineHistogram('etaEMTobs_match,phiEMTobs_match;l1topo_2d_EMTobs_etaPhi_match',
                            title='CMX-L1Topo matched EM TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    # 2D hitmaps mismatched eta-phi
    myGroup.defineHistogram('etaJetSTobs_mismatch,phiJetSTobs_mismatch;l1topo_2d_JetSTobs_etaPhi_mismatch',
                            title='CMX-L1Topo mismatched small jet TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    myGroup.defineHistogram('etaJetLTobs_mismatch,phiJetLTobs_mismatch;l1topo_2d_JetLTobs_etaPhi_mismatch',
                            title='CMX-L1Topo mismatched large jet TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    myGroup.defineHistogram('etaTauTobs_mismatch,phiTauTobs_mismatch;l1topo_2d_TauTobs_etaPhi_mismatch',
                            title='CMX-L1Topo mismatched tau TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    myGroup.defineHistogram('etaEMTobs_mismatch,phiEMTobs_mismatch;l1topo_2d_EMTobs_etaPhi_mismatch',
                            title='CMX-L1Topo mismatched EM TOBs hit map;#eta;#phi', type='TH2F',
                            cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

    # 2D hitmaps matched hardware coordinates
    myGroup.defineHistogram('xJetSTobs_match,yJetSTobs_match;l1topo_2d_JetSTobs_Hitmap_match',
                            title='CMX-L1Topo matched small jet TOBs hit map;Crate*Module;Frame*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=0.,xmax=32,ybins=32,ymin=0,ymax=32.)
    
    myGroup.defineHistogram('xJetLTobs_match,yJetLTobs_match;l1topo_2d_JetLTobs_Hitmap_match',
                            title='CMX-L1Topo matched large jet TOBs hit map;Crate*Module;Frame*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=0.,xmax=32,ybins=32,ymin=0,ymax=32.)
    
    myGroup.defineHistogram('xTauTobs_match,yTauTobs_match;l1topo_2d_TauTobs_Hitmap_match',
                            title='CMX-L1Topo matched tau TOBs hit map;Crate*Module;Chip*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=56,xmin=0.,xmax=56,ybins=64,ymin=0,ymax=64.)
    
    myGroup.defineHistogram('xEMTobs_match,yEMTobs_match;l1topo_2d_EMTobs_Hitmap_match',
                            title='CMX-L1Topo matched EM TOBs hit map;Crate*Module;Chip*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.)

    # 2D hitmaps mismatched hardware coordinates
    myGroup.defineHistogram('xJetSTobs_mismatch,yJetSTobs_mismatch;l1topo_2d_JetSTobs_Hitmap_mismatch',
                            title='CMX-L1Topo mismatched small jet TOBs hit map;Crate*Module;Frame*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=0.,xmax=32,ybins=32,ymin=0,ymax=32.)
    
    myGroup.defineHistogram('xJetLTobs_mismatch,yJetLTobs_mismatch;l1topo_2d_JetLTobs_Hitmap_mismatch',
                            title='CMX-L1Topo mismatched large jet TOBs hit map;Crate*Module;Frame*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=0.,xmax=32,ybins=32,ymin=0,ymax=32.)
    
    myGroup.defineHistogram('xTauTobs_mismatch,yTauTobs_mismatch;l1topo_2d_TauTobs_Hitmap_mismatch',
                            title='CMX-L1Topo mismatched tau TOBs hit map;Crate*Module;Chip*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=56,xmin=0.,xmax=56,ybins=64,ymin=0,ymax=64.)
    
    myGroup.defineHistogram('xEMTobs_mismatch,yEMTobs_mismatch;l1topo_2d_EMTobs_Hitmap_mismatch',
                            title='CMX-L1Topo mismatched EM TOBs hit map;Crate*Module;Chip*Local',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.)

    # Timing Topo items vs BC
    myGroup.defineHistogram('item0,bc0;l1topo_2d_ItemsBC0',
                            title='BC Timing vs Algorithm Number L1Topo_00_U1',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=0*32,xmax=(0+1)*32,ybins=5,ymin=-2.5,ymax=2.5)
    myGroup.defineHistogram('item1,bc1;l1topo_2d_ItemsBC1',
                            title='BC Timing vs Algorithm Number L1Topo_00_U2',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=1*32,xmax=(1+1)*32,ybins=5,ymin=-2.5,ymax=2.5)
    myGroup.defineHistogram('item2,bc2;l1topo_2d_ItemsBC2',
                            title='BC Timing vs Algorithm Number L1Topo_01_U1',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=2*32,xmax=(2+1)*32,ybins=5,ymin=-2.5,ymax=2.5)
    myGroup.defineHistogram('item3,bc3;l1topo_2d_ItemsBC3',
                            title='BC Timing vs Algorithm Number L1Topo_01_U2',
                            type='TH2F',cutmask='',path=trigPath,opt='kAlwaysCreate',
                            xbins=32,xmin=3*32,xmax=(3+1)*32,ybins=5,ymin=-2.5,ymax=2.5)

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

    L1CaloL1TopoMonitorCfg = L1CaloL1TopoMonitoringConfig(ConfigFlags)
    cfg.merge(L1CaloL1TopoMonitorCfg)

    # message level for algorithm
    L1CaloL1TopoMonitorCfg.getEventAlgo('L1CaloL1TopoMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)
