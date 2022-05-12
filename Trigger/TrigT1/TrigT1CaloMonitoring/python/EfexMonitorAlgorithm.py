#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def EfexMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Efex algorithm in the monitoring system.'''

    import math

    # get the component factory - used for merging the algorithm results
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    
    # uncomment if you want to see all the flags
    #inputFlags.dump() # print all the configs

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'EfexMonitoringCfg')

    # get any algorithms
    EfexMonAlg = helper.addAlgorithm(CompFactory.EfexMonitorAlgorithm,'EfexMonAlg')

    # add any steering
    groupName = 'EfexMonitor' # the monitoring group name is also used for the package name
    EfexMonAlg.PackageName = groupName

    mainDir = 'L1Calo'
    trigPath = 'Efex/'

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(EfexMonAlg, groupName , mainDir)

    # histograms of TOB variables
    myGroup.defineHistogram('TOBTransverseEnergy;h_TOBTransverseEnergy',
                            title='TOB Transverse Energy [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=50000)

    myGroup.defineHistogram('TOBEta;h_TOBEta',
                            title='TOB Eta',
                            type='TH1F', path=trigPath,
                            xbins=50,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('TOBPhi;h_TOBPhi',
                            title='TOB Phi',
                            type='TH1F', path=trigPath,
                            xbins=64,xmin=-math.pi,xmax=math.pi)

    myGroup.defineHistogram('TOBEta,TOBPhi;h_TOBEtaPhiMap',
                            title="TOB Eta vs Phi;TOB Eta;TOB Phi",
                            type='TH2F',path=trigPath,
                            xbins=50,xmin=-3.0,xmax=3.0,ybins=64,ymin=-math.pi,ymax=math.pi)

    myGroup.defineHistogram('TOBeFEXNumber;h_TOBeFEXNumber',
                            title='TOB eFEX Number',
                            type='TH1F', path=trigPath,
                            xbins=30,xmin=0,xmax=30)

    myGroup.defineHistogram('TOBshelfNumber;h_TOBshelfNumber',
                            title='TOB Shelf Number',
                            type='TH1F', path=trigPath,
                            xbins=5,xmin=0,xmax=10)

    myGroup.defineHistogram('TOBReta;h_TOBReta',
                            title='TOB Reta',
                            type='TH1F', path=trigPath,
                            xbins=4,xmin=0,xmax=4.0)

    myGroup.defineHistogram('TOBRhad;h_TOBRhad',
                            title='TOB Rhad',
                            type='TH1F', path=trigPath,
                            xbins=4,xmin=0,xmax=4.0)

    myGroup.defineHistogram('TOBWstot;h_TOBWstot',
                            title='TOB Wstot',
                            type='TH1F', path=trigPath,
                            xbins=4,xmin=0,xmax=4.0)

    # plotting of TOB tau variables
    myGroup.defineHistogram('tauTOBTransverseEnergy;h_tauTOBTransverseEnergy',
                            title='TOB Tau Transverse Energy [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=50000)

    myGroup.defineHistogram('tauTOBEta;h_tauTOBEta',
                            title='TOB Tau Eta',
                            type='TH1F', path=trigPath,
                            xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('tauTOBPhi;h_tauTOBPhi',
                            title='TOB Tau Phi',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-math.pi,xmax=math.pi)

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

    # MCs processed adding L1_eEMRoI
    inputs = glob.glob('/eos/user/t/thompson/ATLAS/LVL1_mon/MC_ESD/l1calo.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.eFex_gFex_2022-01-13T2101.root')
    
    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_LVL1_MC.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg  
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    EfexMonitorCfg = EfexMonitoringConfig(ConfigFlags)
    cfg.merge(EfexMonitorCfg)

    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=10
    cfg.run(nevents)

