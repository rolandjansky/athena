#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def L1CaloLegacyEDMMonitoringConfig(inputFlags):
    '''Function to configure LVL1 L1CaloLegacyEDM algorithm in the monitoring system.'''

    #import math 
    #import numpy as np
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # any things that need setting up for job e.g.

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'L1CaloLegacyEDMMonitoringCfg')

    # get any algorithms
    L1CaloLegacyEDMMonAlg = helper.addAlgorithm(CompFactory.L1CaloLegacyEDMMonitorAlgorithm,'L1CaloLegacyEDMMonAlg')

    # add any steering
    groupName = 'L1CaloLegacyEDMMonitor' # the monitoring group name is also used for the package name
    L1CaloLegacyEDMMonAlg.PackageName = groupName

    mainDir = 'L1Calo'
    trigPath = 'L1CaloLegacyEDM/'
    inputPath = trigPath + '/Input/'
    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(L1CaloLegacyEDMMonAlg, groupName , mainDir)

    # add run number plot
    myGroup.defineHistogram('run',title='Run Number;run;Events', path=trigPath,xbins=1,xmin=-0.5, xmax=1.5, opt='kAddBinsDynamically', merge='merge')
    #The LVL1EmTau Hists
    myGroup.defineHistogram('LVL1EmTauRoIs_eta',title='EM Tau RoIs eta;EM Tau RoI #eta;Number of EM Tau RoIs', path=inputPath,xbins=100,xmin=-3.5,xmax=3.5)

    myGroup.defineHistogram('LVL1EmTauRoIs_phi',title='EM Tau RoIs eta;EM Tau RoI #phi;Number of EM Tau RoIs', path=inputPath,xbins=100,xmin=-3.5,xmax=3.5)

    myGroup.defineHistogram('LVL1EmTauRoIs_core',title='EM Tau Core Energy;EM Tau Core Energy [GeV];Number of EM Tau RoIs', path=inputPath,xbins=20,xmin=0,xmax=100)
    
    myGroup.defineHistogram('LVL1EmTauRoIs_emClus',title='EM Cluster Energy;EM Cluster Energy [GeV];Number of EM Tau RoIs', path=inputPath,xbins=20,xmin=0,xmax=100)

    myGroup.defineHistogram('LVL1EmTauRoIs_tauClus',title='Hadronic Isolation Sum;Hadronic Isolation Sum [GeV];Number of EM Tau RoIs',path=inputPath,xbins=20,xmin=0,xmax=10)

    myGroup.defineHistogram('LVL1EmTauRoIs_hadIsol',title='EM Isolation Sum;EM Isolation Sum [GeV];Number of EM Tau RoIs', path=inputPath,xbins=20,xmin=0,xmax=10)

    myGroup.defineHistogram('LVL1EmTauRoIs_emIsol',title='Hadronic Core ;EM Isolation Sum;EM Isolation Sum [GeV];Number of EM Tau RoIs',path=inputPath,xbins=20,xmin=0,xmax=20)

    myGroup.defineHistogram('LVL1EmTauRoIs_hadCore',title='Hadronic Core ;Hadronic Core [GeV];Number of EM Tau RoIs', path=inputPath,xbins=40,xmin=0,xmax=20)

    myGroup.defineHistogram('LVL1EmTauRoIs_thrPattern',title='LVL1EmTauRoIs_thrPattern', path=inputPath,xbins=20,xmin=0,xmax=100000)
    
    #The LVL1Jet Hists
    
    etabins_1d = [-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,
                  -1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,
                  0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,
                  2.0,2.2,2.4,2.7,2.9,3.2,4.9]
    eta_bins = {
        'xbins': etabins_1d
    }

    myGroup.defineHistogram('LVL1JetRoIs_eta',title='Jet RoIs eta;Jet RoI #eta;Number of Jet RoIs', path=inputPath,type='TH1F',**eta_bins) 

    myGroup.defineHistogram('LVL1JetRoIs_phi',title='Jet RoIs phi;Jet RoI #phi;Number of Jet RoIs', path=inputPath,xbins=100,xmin=-3.5,xmax=3.5)    
    
    myGroup.defineHistogram('LVL1JetRoIs_et4x4',title='Jet RoIs ET 4x4;Jet ET 4x4 [GeV]; Number of Jet RoIs', path=inputPath,xbins=100,xmin=0,xmax=500)

    myGroup.defineHistogram('LVL1JetRoIs_et8x8',title='Jet RoIs ET 8x8;Jet ET 8x8 [GeV]; Number of Jet RoIs', path=inputPath,xbins=100,xmin=0,xmax=500)

    #The EnergySum Hists
    myGroup.defineHistogram('LVL1EnergySumRoI_energyT',title='Energy Sum RoI energyT;Energy Sum RoI energyT [GeV];Number of Energy Sum RoIs', path=inputPath,xbins=100,xmin=0,xmax=1000)

    myGroup.defineHistogram('LVL1EnergySumRoI_energyX',title='Energy Sum RoI energyX;Energy Sum RoI energyX [GeV];Number of Energy Sum RoIs', path=inputPath,xbins=100,xmin=-500,xmax=500)

    myGroup.defineHistogram('LVL1EnergySumRoI_energyY',title='Energy Sum RoI energyY;Energy Sum RoI energyY [GeV];Number of Energy Sum RoIs', path=inputPath,xbins=100,xmin=-500,xmax=500)

    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
    # set input file and config options
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob

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

    L1CaloLegacyEDMMonitorCfg = L1CaloLegacyEDMMonitoringConfig(ConfigFlags)
    cfg.merge(L1CaloLegacyEDMMonitorCfg)

    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)
