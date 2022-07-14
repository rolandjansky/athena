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

    etabins=32
    etamin=-3.3
    etamax=3.3
    phibins=34
    phimin=-3.3
    phimax=3.3

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(GfexMonAlg, groupName , mainDir)

    # define gfex histograms
    myGroup.defineHistogram('gFexLRJetTransverseEnergy;h_gFexLRJetTransverseEnergy',
                            title='gFex LRJet Transverse Energy; gFexLRJet Et [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=819200)

    myGroup.defineHistogram('gFexLRJetEta;h_gFexLRJetEta',
                            title='gFex LRJet #eta; #eta; counts',
                            type='TH1F', path=trigPath,
                            xbins=etabins,xmin=etamin,xmax=etamax)

    myGroup.defineHistogram('gFexLRJetPhi;h_gFexLRJetPhi',
                            title='gFex LRJet #phi;  #phi; counts',
                            type='TH1F', path=trigPath,
                            xbins=phibins,xmin=phimin,xmax=phimax)

    myGroup.defineHistogram('gFexRhoTransverseEnergy;h_gFexRhoTransverseEnergy',
                            title='gFex Rho Transverse Energy; gFexRho Et [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)


    myGroup.defineHistogram('gFexSRJetTransverseEnergy;h_gFexSRJetTransverseEnergy',    
                            title='gFex SRJet Transverse Energy; gFexSRJet Et [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-100,xmax=819200)

    myGroup.defineHistogram('gFexSRJetEta;h_gFexSRJetEta',
                            title='gFex SRJet #eta; #eta; counts',
                            type='TH1F', path=trigPath,
                            xbins=etabins,xmin=etamin,xmax=etamax)

    myGroup.defineHistogram('gFexSRJetPhi;h_gFexSRJetPhi',
                            title='gFex SRJet #phi; #phi; counts',
                            type='TH1F', path=trigPath,
                            xbins=32,xmin=-math.pi,xmax=math.pi)

    myGroup.defineHistogram('gFexType;h_gFexType',
                            title='gFex Type for all Trigger Objects; gFex Types',
                            xlabels=["gRho", "gBlockLead", "gBlockSub", "gJet"],
                            type='TH1F', path=trigPath,
                            xbins=4,xmin=0,xmax=4)

    myGroup.defineHistogram('gFexLRJetEta,gFexLRJetPhi;h_gFexLRJetEtaPhiMap_weighted',
                            title="gFexLRJet #eta vs #phi;gFexLRJet #eta;gFexLRJet #phi;gFexLRJet TransverseEnergy",
                            type='TH2F',path=trigPath,weight="gFexLRJetTransverseEnergy",
                            xbins=etabins,xmin=etamin,xmax=etamax,ybins=32,ymin=0,ymax=math.pi)

    myGroup.defineHistogram('gFexSRJetEta,gFexSRJetPhi;h_gFexSRJetEtaPhiMap_weighted',
                            title="gFexSRJet #eta vs #phi;gFexSRJet #eta;gFexSRJet #phi;gFexSRJet TransverseEnergy",
                            type='TH2F',path=trigPath,weight="gFexSRJetTransverseEnergy",
                            xbins=etabins,xmin=etamin,xmax=etamax,ybins=32,ymin=0,ymax=math.pi)
    
    myGroup.defineHistogram('gFexLRJetEta,gFexLRJetPhi;h_gFexLRJetEtaPhiMap',
                            title="gFexLRJe #eta vs #phi;gFexLRJet #eta;gFexLRJet #phi",
                            type='TH2F',path=trigPath,
                            xbins=etabins,xmin=etamin,xmax=etamax,ybins=32,ymin=0,ymax=math.pi)

    myGroup.defineHistogram('gFexSRJetEta,gFexSRJetPhi;h_gFexSRJetEtaPhiMap',
                            title="gFexSRJet #eta vs #phi;gFexSRJet #eta;gFexSRJet #phi",
                            type='TH2F',path=trigPath,
                            xbins=etabins,xmin=etamin,xmax=etamax,ybins=32,ymin=0,ymax=math.pi)

    myGroup.defineHistogram('gFexMET;h_gFexMET',
                            title='gFex MET obtained with jets without jets algorithm;MET [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=600000)

    myGroup.defineHistogram('gFexSumET;h_gFexSumET',
                            title='gFex SumET obtained with jets without jets algorithm ; SumET [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=5000000)

    myGroup.defineHistogram('gFexMETx;h_gFexMETx',
                            title='gFex MET x obtained with jets without jets algorithm ; MET x [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMETy;h_gFexMETy',
                            title='gFex MET y obtained with jets without jets algorithm ; MET y [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMHTx;h_gFexMHTx',
                            title='gFex MHT x obtained with jets without jets algorithm; MHT x [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMHTy;h_gFexMHTy',
                            title='gFex MHT y obtained with jets without jets algorithm;MHT y [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-300000,xmax=300000)

    myGroup.defineHistogram('gFexMSTx;h_gFexMSTx',
                            title='gFex MST x obtained with jets without jets algorithm;MST x [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-200000,xmax=200000)

    myGroup.defineHistogram('gFexMSTy;h_gFexMSTy',
                            title='gFex MST y obtained with jets without jets algorithm;MST y [MeV];counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-200000,xmax=200000)

    myGroup.defineHistogram('gFexMETx_NoiseCut;h_gFexMETx_NoiseCut',
                            title='gFex MET x obtained with noise cut algorithm;MET x [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-600000,xmax=600000)

    myGroup.defineHistogram('gFexMETy_NoiseCut;h_gFexMETy_NoiseCut',
                            title='gFex MET y obtained with noise cut algorithm;MET y [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-600000,xmax=600000)

    myGroup.defineHistogram('gFexMETx_ComponentsRms;h_gFexMETx_ComponentsRms',
                            title='gFex MET x obtained with Rho+RMS algorithm;MET x [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-600000,xmax=600000)

    myGroup.defineHistogram('gFexMETy_ComponentsRms;h_gFexMETy_ComponentsRms',
                            title='gFex MET y obtained with Rho+RMS algorithm;MET y [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-600000,xmax=600000)

    myGroup.defineHistogram('gFexMET_NoiseCut;h_gFexMET_NoiseCut',
                            title='gFex MET [MeV] obtained with noise cut algorithm;MET [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-10000,xmax=1000000)

    myGroup.defineHistogram('gFexSumET_NoiseCut;h_gFexSumET_NoiseCut',
                            title='gFex SumET [MeV] obtained with noise cut algorithm;Sum ET [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=2500000)

    myGroup.defineHistogram('gFexMET_Rms;h_gFexMET_Rms',
                            title='gFex MET [MeV] obtained with Rho+RMS algorithm;MET [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=-100000,xmax=1000000)

    myGroup.defineHistogram('gFexSumET_Rms;h_gFexSumET_Rms',
                            title='gFex SumET [MeV] obtained with Rho+RMS algorithm;Sum ET [MeV]; counts',
                            type='TH1F', path=trigPath,
                            xbins=100,xmin=0,xmax=2000000)

    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
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

