#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def GfexMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Gfex algorithm in the monitoring system.'''

    import math

    # get the component factory - used for merging the algorithm results
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    
    # uncomment if you want to see all the flags
    #inputFlags.dump() # print all the configs

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'GfexMonitoringCfg')

    # get any algorithms
    GfexMonAlg = helper.addAlgorithm(CompFactory.GfexMonitorAlgorithm,'GfexMonAlg')

    # add any steering
    groupName = 'GfexMonitor' # the monitoring group name is also used for the package name
    GfexMonAlg.PackageName = groupName

    mainDir = 'L1Calo'
    trigPath = 'Gfex/'

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(GfexMonAlg, groupName , mainDir)

    # define gfex histograms
    myGroup.defineHistogram('gFexLRJetTransverseEnergy;h_gFexLRJetTransverseEnergy',
                            title='gFex LRJet Transverse Energy [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=1000)

    myGroup.defineHistogram('gFexLRJetEta;h_gFexLRJetEta',
                            title='gFex LRJet Eta',
                            type='TH1F', path=trigPath,
                            xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('gFexLRJetPhi;h_gFexLRJetPhi',
                            title='gFex LRJet Phi',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-math.pi,xmax=math.pi)

    myGroup.defineHistogram('gFexRhoTransverseEnergy;h_gFexRhoTransverseEnergy',
                            title='gFex Rho Transverse Energy [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=1000)

    myGroup.defineHistogram('gFexSRJetTransverseEnergy;h_gFexSRJetTransverseEnergy',    
                            title='gFex SRJet Transverse Energy [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-100,xmax=1000)

    myGroup.defineHistogram('gFexSRJetEta;h_gFexSRJetEta',
                            title='gFex SRJet Eta',
                            type='TH1F', path=trigPath,
                            xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('gFexSRJetPhi;h_gFexSRJetPhi',
                            title='gFex SRJet Phi',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-math.pi,xmax=math.pi)

    myGroup.defineHistogram('gFexType;h_gFexType',
                            title='gFex Type for all Trigger Objects',
                            type='TH1F', path=trigPath,
                            xbins=4,xmin=0,xmax=4)

    myGroup.defineHistogram('gFexLRJetEta,gFexLRJetPhi;h_gFexLRJetEtaPhiMap_weighted',
                            title="gFexLRJet Eta vs Phi;gFexLRJet Eta;gFexLRJet Phi;gFexLRJet TransverseEnergy",
                            type='TH2F',path=trigPath,weight="gFexLRJetTransverseEnergy",
                            xbins=60,xmin=-3.0,xmax=3.0,ybins=64,ymin=0,ymax=math.pi)

    myGroup.defineHistogram('gFexSRJetEta,gFexSRJetPhi;h_gFexSRJetEtaPhiMap_weighted',
                            title="gFexSRJet Eta vs Phi;gFexSRJet Eta;gFexSRJet Phi;gFexSRJet TransverseEnergy",
                            type='TH2F',path=trigPath,weight="gFexSRJetTransverseEnergy",
                            xbins=60,xmin=-3.0,xmax=3.0,ybins=64,ymin=0,ymax=math.pi)
    
    myGroup.defineHistogram('gFexLRJetEta,gFexLRJetPhi;h_gFexLRJetEtaPhiMap',
                            title="gFexLRJe Eta vs Phi;gFexLRJet Eta;gFexLRJet Phi",
                            type='TH2F',path=trigPath,
                            xbins=60,xmin=-3.0,xmax=3.0,ybins=64,ymin=0,ymax=math.pi)

    myGroup.defineHistogram('gFexSRJetEta,gFexSRJetPhi;h_gFexSRJetEtaPhiMap',
                            title="gFexSRJe Eta vs Phi;gFexSRJet Eta;gFexSRJet Phi",
                            type='TH2F',path=trigPath,
                            xbins=60,xmin=-3.0,xmax=3.0,ybins=64,ymin=0,ymax=math.pi)

    myGroup.defineHistogram('gFexMET;h_gFexMET',
                            title='gFex MET [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=600000)

    myGroup.defineHistogram('gFexSumET;h_gFexSumET',
                            title='gFex SumET [MeV]',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=5000000)

    myGroup.defineHistogram('gFexMETx;h_gFexMETx',
                            title='gFex MET x',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMETy;h_gFexMETy',
                            title='gFex MET y',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMHTx;h_gFexMHTx',
                            title='gFex MHT x',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMHTy;h_gFexMHTy',
                            title='gFex MHT y',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMSTx;h_gFexMSTx',
                            title='gFex MST x',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-200000,xmax=200000)

    myGroup.defineHistogram('gFexMSTy;h_gFexMSTy',
                            title='gFex MST y',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-200000,xmax=200000)

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

    # MCs with SCells
    #inputs = glob.glob('/eos/user/t/thompson/ATLAS/LVL1_mon/valid1.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3126_d1623_r12488/ESD.24607652._000025.pool.root.1')
    #inputs = glob.glob('/eos/atlas/atlascerngroupdisk/det-l1calo/OfflineSoftware/mc16_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.ESD.e3601_s3126_r12406/ESD.24368002._000045.pool.root.1')
    #
    # Above MCs processed adding L1_eEMRoI
    inputs = glob.glob('/afs/cern.ch/user/t/thompson/work/public/LVL1_monbatch/run_sim/l1calo.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.ESD.eFex_2021-05-16T2101.root')
    

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg  
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    GfexMonitorCfg = GfexMonitoringConfig(ConfigFlags)
    cfg.merge(GfexMonitorCfg)

    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=100
    cfg.run(nevents)

