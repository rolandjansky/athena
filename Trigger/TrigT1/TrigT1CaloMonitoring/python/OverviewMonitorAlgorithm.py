#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def OverviewMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Overview algorithm in the monitoring system.'''

    from AthenaConfiguration.Enums import Format

    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'OverviewMonitoringCfg')

    # get any algorithms
    OverviewMonAlg = helper.addAlgorithm(CompFactory.OverviewMonitorAlgorithm,'OverviewMonAlg')

    # add any steering
    groupName = 'OverviewMonitor' # the monitoring group name is also used for the package name
    OverviewMonAlg.PackageName = groupName

    mainDir = 'L1Calo'
    trigPath = 'Overview/'

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(OverviewMonAlg, groupName , mainDir)

    # flag for online - different duration options required
    isOnline=inputFlags.Trigger.Online.isPartition or inputFlags.Input.Format is Format.BS

    # global overview
    NumberOfGlobalErrors=15
    globalStatus_labels = [
        "PPMDataStatus",
        "PPMDataError",
        "SubStatus",
        "Parity",
        "LinkDown",
        "Transmission",
        "Simulation",
        "CMXSubStatus",
        "CMXParity",
        "CMXTransmission",
        "CMXSimulation",
        "RODStatus",
        "RODMissing",
        "ROBStatus",
        "Unpacking"]
    
    myGroup.defineHistogram('globalOverviewX,globalOverviewY;l1calo_2d_GlobalOverview',title='L1Calo Global Error Overview;;',
                            type='TH2F',
                            path=trigPath,
                            xbins=NumberOfGlobalErrors,xmin=0.,xmax=NumberOfGlobalErrors,
                            ybins=14,ymin=0.,ymax=14, xlabels=globalStatus_labels,
                            duration='' if isOnline else 'lb',
                            opt='kLBNHistoryDepth=10' if isOnline else '')


    myGroup.defineHistogram('lb_errors;l1calo_1d_ErrorsByLumiblock',
                            title='Events with Errors by Lumiblock;Lumi Block;Number of Events;',
                            path=trigPath,
                            duration='' if isOnline else 'lb',
                            opt='kLive' if isOnline else '')

    
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

    #inputs = glob.glob('/eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/00357750/data18_13TeV.00357750.physics_Main.recon.ESD.f1072/data18_13TeV.00357750.physics_Main.recon.ESD.f1072._lb0117._SFO-1._0201.1')
    inputs = glob.glob('/eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/00354311/data18_13TeV.00354311.physics_Main.recon.ESD.f1129/data18_13TeV.00354311.physics_Main.recon.ESD.f1129._lb0013._SFO-8._0001.1')


    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_LVL1.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    OverviewMonitorCfg = OverviewMonitoringConfig(ConfigFlags)
    cfg.merge(OverviewMonitorCfg)

    # message level for algorithm
    OverviewMonitorCfg.getEventAlgo('OverviewMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)
