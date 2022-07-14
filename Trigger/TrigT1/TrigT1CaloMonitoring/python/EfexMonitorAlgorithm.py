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

    # histograms of eEM variables
    myGroup.defineHistogram('TOBTransverseEnergy;h_TOBTransverseEnergy', title='eFex TOB EM Transverse Energy [MeV]',
                            type='TH1F', path=trigPath+'eEM/', xbins=100,xmin=0,xmax=50000)

    myGroup.defineHistogram('TOBEta;h_TOBEta', title='eFex TOB EM Eta',
                            type='TH1F', path=trigPath+'eEM/', xbins=50,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('TOBPhi;h_TOBPhi', title='eFex TOB EM Phi',
                            type='TH1F', path=trigPath+'eEM/', xbins=64,xmin=-math.pi,xmax=math.pi)

    myGroup.defineHistogram('TOBEta,TOBPhi;h_TOBEtaPhiMap', title="eFex TOB EM Eta vs Phi;TOB EM Eta;TOB EM Phi",
                            type='TH2F',path=trigPath+'eEM/', xbins=50,xmin=-3.0,xmax=3.0,ybins=64,ymin=-math.pi,ymax=math.pi)

    myGroup.defineHistogram('TOBeFEXNumber;h_TOBeFEXNumber', title='eFex TOB EM Module Number',
                            type='TH1F', path=trigPath+'eEM/', xbins=12,xmin=0,xmax=12)

    myGroup.defineHistogram('TOBshelfNumber;h_TOBshelfNumber', title='eFex TOB EM Shelf Number',
                            type='TH1F', path=trigPath+'eEM/', xbins=2,xmin=0,xmax=2)

    myGroup.defineHistogram('TOBfpga;h_TOBfpga', title='eFex TOB EM FPGA',
                            type='TH1F', path=trigPath+'eEM/', xbins=4,xmin=0,xmax=4)

    myGroup.defineHistogram('TOBReta;h_TOBReta', title='eFex TOB EM Reta',
                            type='TH1F', path=trigPath+'eEM/',xbins=250,xmin=0,xmax=1)

    myGroup.defineHistogram('TOBRhad;h_TOBRhad', title='eFex TOB EM Rhad',
                            type='TH1F', path=trigPath+'eEM/', xbins=250,xmin=0,xmax=1) 

    myGroup.defineHistogram('TOBWstot;h_TOBWstot', title='eFex TOB EM Wstot',
                            type='TH1F', path=trigPath+'eEM/', xbins=250,xmin=0,xmax=1) 

    threshold_labels = ['fail','loose','medium','tight']
    myGroup.defineHistogram('TOBReta_threshold;h_TOBReta_threshold', title='eFex TOB EM Reta threshold',
                            type='TH1F', path=trigPath+'eEM/',xbins=4,xmin=0,xmax=4.0,xlabels=threshold_labels)

    myGroup.defineHistogram('TOBRhad_threshold;h_TOBRhad_threshold', title='eFex TOB EM Rhad threshold',
                            type='TH1F', path=trigPath+'eEM/', xbins=4,xmin=0,xmax=4.0,xlabels=threshold_labels)

    myGroup.defineHistogram('TOBWstot_threshold;h_TOBWstot_threshold', title='eFex TOB EM Wstot threshold',
                            type='TH1F', path=trigPath+'eEM/', xbins=4,xmin=0,xmax=4.0,xlabels=threshold_labels)

    # plotting of eTau variables
    myGroup.defineHistogram('tauTOBTransverseEnergy;h_tauTOBTransverseEnergy', title='eFex TOB Tau Transverse Energy [MeV]',
                            type='TH1F', path=trigPath+'eTau/', xbins=100,xmin=0,xmax=50000)

    myGroup.defineHistogram('tauTOBEta;h_tauTOBEta', title='eFex TOB Tau Eta',
                            type='TH1F', path=trigPath+'eTau/', xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('tauTOBPhi;h_tauTOBPhi', title='eFex TOB Tau Phi',
                            type='TH1F', path=trigPath+'eTau/', xbins=100,xmin=-math.pi,xmax=math.pi)

    myGroup.defineHistogram('tauTOBEta,tauTOBPhi;h_tauTOBEtaPhiMap', title="eFex TOB Tau Eta vs Phi;TOB Tau Eta;TOB Tau Phi",
                            type='TH2F',path=trigPath+'eTau/', xbins=50,xmin=-3.0,xmax=3.0,ybins=64,ymin=-math.pi,ymax=math.pi)

    myGroup.defineHistogram('tauTOBeFEXNumber;h_tauTOBeFEXNumber', title='eFex TOB Tau Module Number',
                            type='TH1F', path=trigPath+'eTau/', xbins=12,xmin=0,xmax=12)

    myGroup.defineHistogram('tauTOBshelfNumber;h_tauTOBshelfNumber', title='eFex TOB Tau Shelf Number',
                            type='TH1F', path=trigPath+'eTau/', xbins=2,xmin=0,xmax=2)

    myGroup.defineHistogram('tauTOBfpga;h_tauTOBfpga', title='eFex TOB Tau FPGA',
                            type='TH1F', path=trigPath+'eTau/', xbins=4,xmin=0,xmax=4)

    myGroup.defineHistogram('tauTOBRcore;h_tauTOBRcore', title='eFex TOB Tau rCore',
                            type='TH1F', path=trigPath+'eTau/', xbins=250,xmin=0,xmax=1) 

    myGroup.defineHistogram('tauTOBRhad;h_tauTOBRhad', title='eFex TOB Tau rHad',
                            type='TH1F', path=trigPath+'eTau/', xbins=250,xmin=0,xmax=1) 

    myGroup.defineHistogram('tauTOBRcore_threshold;h_tauTOBRcore_threshold', title='eFex TOB Tau rCore threshold',
                            type='TH1F', path=trigPath+'eTau/', xbins=4,xmin=0,xmax=4.0, xlabels=threshold_labels)

    myGroup.defineHistogram('tauTOBRhad_threshold;h_tauTOBRhad_threshold', title='eFex TOB Tau rHad threshold',
                            type='TH1F', path=trigPath+'eTau/', xbins=4,xmin=0,xmax=4.0, xlabels=threshold_labels)

    myGroup.defineHistogram('tauTOBthree_threshold;h_tauTOBthree_threshold', title='eFex TOB Tau 3 taus threshold',
                            type='TH1F', path=trigPath+'eTau/', xbins=4,xmin=0,xmax=4.0, xlabels=threshold_labels)

    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
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

