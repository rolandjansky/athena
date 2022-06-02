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

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(OverviewMonAlg, groupName , mainDir)

    # flag for online - different duration options required
    isOnline=inputFlags.Trigger.Online.isPartition and inputFlags.Input.Format is Format.BS

    # number of processed events
    global_labels = ["Processed Events"]
    myGroup.defineHistogram('n_processed;l1calo_1d_NumberOfEvents',
                            title='Number of processed events',                            
                            type='TH1I',
                            path='Overview/',
                            xbins=1,xmin=0,xmax=1, xlabels=global_labels)


    # global overview
    trigPath = 'Overview/Errors/'
    NumberOfGlobalErrors=15
    globalStatus_xlabels = [
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

    globalStatus_ylabels = []
    for crate in range(14):
        cr = crate
        if cr >= 12:
            cr -= 12
        if cr >= 8:
            cr -= 8
        type = "PP " if (crate < 8) else "CP " if (crate < 12) else "JEP "
        globalStatus_ylabels.append(type)
    
    myGroup.defineHistogram('globalOverviewX,globalOverviewY;l1calo_2d_GlobalOverview',title='L1Calo Global Error Overview;;',
                            type='TH2I',
                            path=trigPath,
                            xbins=NumberOfGlobalErrors,xmin=0.,xmax=NumberOfGlobalErrors,
                            ybins=14,ymin=0.,ymax=14, xlabels=globalStatus_xlabels, ylabels=globalStatus_ylabels,
                            duration='' if isOnline else 'lb',
                            opt='kAlwaysCreate')

    if isOnline:
        myGroup.defineHistogram('globalOverviewX,globalOverviewY;l1calo_2d_GlobalOverviewRecent',
                                title='L1Calo Global Error Overview Last 10 LumiBlocks;;',
                                type='TH2I',
                                path=trigPath,
                                xbins=NumberOfGlobalErrors,xmin=0.,xmax=NumberOfGlobalErrors,
                                ybins=14,ymin=0.,ymax=14, xlabels=globalStatus_xlabels, ylabels=globalStatus_ylabels,
                                opt='kLBNHistoryDepth=10,kAlwaysCreate')


    myGroup.defineHistogram('lb_errors;l1calo_1d_ErrorsByLumiblock',
                            title='Events with Errors by Lumiblock;Lumi Block;Number of Events;',                            
                            type='TH1I',
                            path=trigPath,
                            xbins=3000,xmin=0,xmax=3000,
                            weight='n_lb_errors',
                            opt='kAlwaysCreate')

    
    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
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
