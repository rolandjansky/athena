#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
def L1CaloL1TopoMonitoringConfig(inputFlags):
    '''Function to configure LVL1 L1CaloL1Topo algorithm in the monitoring system.'''

    #import math 
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
    #phibins = 32
    #phimin = 0.
    #phimax = 2.*math.pi

    # eta bin edges taken from TrigT1CaloLWHistogramTool
    #etabins = [-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,
    #           -1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,
    #           0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,
    #           2.0,2.2,2.4,2.7,2.9,3.2,4.9]

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(L1CaloL1TopoMonAlg, groupName , mainDir)

    # run number plot
    myGroup.defineHistogram(
        'run',title='Run Number;run;Events',
        path=trigPath,xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Error summary plot for the entire algorithm
    error_labels = ['Calo conv','No CMX','DAQ conv','No DAQ','ROI conv',
                    'No ROI' 'Fibre CRC','Payload CRC','CMX-Topo match']
    myGroup.defineHistogram(
        'sumErrors;l1topo_1d_Errors',
        path=trigPath, type='TH1F',
        title='Counts of errors;;Entries',cutmask='lbErrors',
        opt='kAlwaysCreate',
        xlabels=error_labels, xbins=len(error_labels),
        xmin=0, xmax=len(error_labels))

    # Errors ordered by lumiblock
    myGroup.defineHistogram(
        'lbErrors;l1topo_1d_ErrorsByLumiblock',
        path=trigPath,type='TH1F',cutmask='lbErrors',
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

    # 2D plots
    #myGroup.defineHistogram(
    #    'eta,phi;Hitmap',
    #    title='Jet TOB eta/phi;Jet TOB #eta; Jet TOB #phi',type='TH2F',
    #    path=trigPath,
    #    xbins=etabins, ybins=phibins, ymin=phimin, ymax=phimax)

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
