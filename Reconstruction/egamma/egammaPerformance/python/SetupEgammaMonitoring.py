#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file SetupEgammaMonitoring.py
@author B. Laforge
@brief Example python configuration for the Run III AthenaMonitoring package
'''
from AthenaConfiguration.ComponentFactory import CompFactory

def BookHistogramPerRegion(groupe,name,particletype,title,path,xbins,xmin,xmax,nregions=3):
    '''
    Function to configure the egamma histograms per region (BARREL, CRACK, ENDCAP, FORWARD)
    The list is different for electrons, forward electrons and photons
    '''

    nregionsloc = nregions

    if nregionsloc > 4 :
        nregionsloc = 4

    Regions = ['BARREL', 'CRACK', 'ENDCAP', 'FORWARD']
    for n in range(nregionsloc):
        hname = name + particletype + Regions[n]
        htitle = title + particletype + Regions[n]
        groupe.defineHistogram(hname,title=htitle, path=path, xbins=xbins,xmin=xmin,xmax=xmax)


def BookHistograms(groupe,reconame,particletype,withTrigger=""):
    '''
    Function to configure the egamma histograms
    The list is different for electrons, forward electrons and photons  
    usage example : BookHistograms(GroupElectronLHTight,"LHTight","Electron","WithTrigger")
    '''

    if particletype in ["Electron","Photon"] :

        if particletype in ["Electron"] :
            prefix = "electron"
        if particletype in ["Photon"] :
            prefix = "photon"

        groupe.defineHistogram('lumiPerBCID',title='Luminosity,WithCommaInTitle;L/BCID;Events',path='Shifter',xbins=40,xmin=0.0,xmax=80.0)
        groupe.defineHistogram('lb', title='Luminosity Block;lb;Events', path='Shifter',xbins=1000,xmin=-0.5,xmax=999.5)
        groupe.defineHistogram('run',title='Run Number;run;Events', path='Shifter',xbins=1000000,xmin=-0.5,xmax=999999.5)
        
        groupe.defineHistogram('pt',title='%(p)s Transverse momentum Distribution ; %(p)s Transverse momentum [GeV] ; Events' % {'p': particletype}, path='Shifter',xbins=40,xmin=0.0,xmax=80.0)

        # SHIFTER PANEL      
                                                                                                                                          
        hname = prefix + "N" + reconame + withTrigger
        htitle = "Number of " + prefix + "s (" + reconame + ")"
        groupe.defineHistogram(hname,title=htitle, path='Shifter',xbins=20,xmin=0.0,xmax=20.0)

        hname= prefix + "Et" + reconame
        htitle= particletype + " transverse energy [MeV]" + " (" + reconame + ")"
        groupe.defineHistogram(hname,title=htitle, path='Shifter',xbins=100,xmin=-1000.0,xmax=25000.0)
        
        #hname= prefix + "EtaPhiPtgt4GeV" + reconame 
        #htitle= particletype + " #eta,#phi map (candidates with Pt>4GeV)" + " (" + reconame + ")"
        #groupe.defineHistogram(hname,title=htitle,path='Shifter',type='TH2F',xbins=64,xmin=-3.2,xmax=3.2,ybins=64,ymin=-3.2,ymax=3.2)

        hname= prefix + "Eta" + reconame
        htitle= particletype + " #eta" + " (" + reconame + ")"
        groupe.defineHistogram(hname,title=htitle, path='Shifter',xbins=64,xmin=-3.2,xmax=3.2)
        
        hname= prefix + "Phi" + reconame
        htitle= particletype + " #phi" + " (" + reconame + ")"
        groupe.defineHistogram(hname,title=htitle, path='Shifter',xbins=64,xmin=-3.2,xmax=3.2)
        
        hname= prefix+"TopoEtCone40" + reconame
        htitle= particletype + " Topocluster Isolation Energy" + " (" + reconame + ")"
        groupe.defineHistogram(hname,title=htitle, path='Shifter',xbins=64,xmin=-10000.,xmax=40000.)

    return True

def MonitorElectronConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'ElectronAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example
    # algorithm in the AthenaMonitoring package. Just pass the type to the
    # helper. Then, the helper will instantiate an instance and set up the
    # base class configuration following the inputFlags. The returned object
    # is the algorithm.
    MonitorElectronAlgorithm=CompFactory.MonitorElectronAlgorithm
    elLHTightMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elLHTightMonAlg')
    elLHTightTrigMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elLHTightTrigMonAlg')

    elLHLooseMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elLHLooseMonAlg')
    elLHLooseTrigMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elLHLooseTrigMonAlg')

    elCBTightMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elCBTightMonAlg')
    elCBTightTrigMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elCBTightTrigMonAlg')

    elCBLooseMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elCBLooseMonAlg')
    elCBLooseTrigMonAlg = helper.addAlgorithm(MonitorElectronAlgorithm,'elCBLooseTrigMonAlg')

    ### STEP 3 ###
    # Edit properties of algorithms

    elLHTightMonAlg.ParticleContainerName = "Electrons"
    elLHTightMonAlg.RecoName = "LHTight"
    elLHTightMonAlg.ParticlePrefix = "electron"
    elLHTightMonAlg.WithTrigger = "" 
    elLHTightMonAlg.CheckTrigger = False 
    # to enable a trigger filter, for example:
    # elLHTightMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    elLHTightTrigMonAlg.ParticleContainerName = "Electrons"
    elLHTightTrigMonAlg.RecoName = "LHTight"
    elLHTightTrigMonAlg.ParticlePrefix = "electron"
    elLHTightTrigMonAlg.WithTrigger = "WithTrigger" 
    elLHTightTrigMonAlg.CheckTrigger = True 
    # to enable a trigger filter, for example:
    # elLHTightTrigMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    elLHLooseMonAlg.ParticleContainerName = "Electrons"
    elLHLooseMonAlg.RecoName = "LHLoose"
    elLHLooseMonAlg.ParticlePrefix = "electron"
    elLHLooseMonAlg.CheckTrigger = False 
    # to enable a trigger filter, for example:
    # elLHLooseMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    elLHLooseTrigMonAlg.ParticleContainerName = "Electrons"
    elLHLooseTrigMonAlg.RecoName = "LHLoose"
    elLHLooseTrigMonAlg.ParticlePrefix = "electron"
    elLHLooseTrigMonAlg.CheckTrigger = True 
    # to enable a trigger filter, for example:
    # elLHLooseTrigMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    elCBTightMonAlg.ParticleContainerName = "Electrons"
    elCBTightMonAlg.RecoName = "Tight"
    elCBTightMonAlg.ParticlePrefix = "electron"
    elCBTightMonAlg.CheckTrigger = False 
    # to enable a trigger filter, for example:
    # elCBTightMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    elCBTightTrigMonAlg.ParticleContainerName = "Electrons"
    elCBTightTrigMonAlg.RecoName = "Tight"
    elCBTightTrigMonAlg.ParticlePrefix = "electron"
    elCBTightTrigMonAlg.CheckTrigger = True 
    # to enable a trigger filter, for example:
    # elCBTightTrigMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    elCBLooseMonAlg.ParticleContainerName = "Electrons"
    elCBLooseMonAlg.RecoName = "Loose"
    elCBLooseMonAlg.ParticlePrefix = "electron"
    elCBLooseMonAlg.CheckTrigger = False 
    # to enable a trigger filter, for example:
    # elCBLooseMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    elCBLooseTrigMonAlg.ParticleContainerName = "Electrons"
    elCBLooseTrigMonAlg.RecoName = "Loose"
    elCBLooseTrigMonAlg.ParticlePrefix = "electron"
    elCBLooseTrigMonAlg.CheckTrigger = True 
    # to enable a trigger filter, for example:
    # elCBLooseTrigMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # First, add a tool that's set up by a different configuration function.
    # In this case, CaloNoiseToolCfg returns its own component accumulator,
    # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # electronMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned
    # object here is the standard GenericMonitoringTool.

    GroupElectronLHTight = helper.addGroup(elLHTightMonAlg, 'MonitorElectron', 'egamma/LHTightElectrons/')
    GroupElectronLHTightTriggered = helper.addGroup(elLHTightTrigMonAlg, 'MonitorElectron', 'egamma/LHTightElectronsWithTrigger/')
    GroupElectronLHLoose = helper.addGroup(elLHLooseMonAlg, 'MonitorElectron', 'egamma/LHLooseElectrons/')
    GroupElectronLHLooseTriggered = helper.addGroup(elLHLooseTrigMonAlg, 'MonitorElectron', 'egamma/LHLooseElectronsWithTrigger/')
    GroupElectronCBTight = helper.addGroup(elCBTightMonAlg, 'MonitorElectron', 'egamma/CBTightElectrons/')
    GroupElectronCBTightTriggered = helper.addGroup(elCBTightTrigMonAlg, 'MonitorElectron', 'egamma/CBTightTriggeredElectrons/')
    GroupElectronCBLoose = helper.addGroup(elCBLooseMonAlg, 'MonitorElectron', 'egamma/CBLooseElectrons/')
    GroupElectronCBLooseTriggered = helper.addGroup(elCBLooseTrigMonAlg, 'MonitorElectron', 'egamma/CBLooseTriggeredElectrons/')

    ### STEP 5 ###
    # Configure histograms

    # LHTight Electrons
    BookHistograms(GroupElectronLHTight,"LHTight","Electron")
    BookHistograms(GroupElectronLHTightTriggered,"LHTightTrig","Electron","WithTrigger")
    # LHLoose Electrons
    BookHistograms(GroupElectronLHLoose,"LHLoose","Electron")
    BookHistograms(GroupElectronLHLooseTriggered,"LHLooseTrig","Electron","withTrigger")
    # Cut Based Tight Electrons
    BookHistograms(GroupElectronCBTight,"CBTight","Electron")
    BookHistograms(GroupElectronCBTightTriggered,"CBTightTrig","Electron","withTrigger")
    # Cut Based Loose Electrons
    BookHistograms(GroupElectronCBLoose,"CBLoose","Electron")
    BookHistograms(GroupElectronCBLooseTriggered,"CBLooseTrig","Electron","withTrigger")

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can
    # just return directly (and not create "result" above)
    return helper.result()

    # # Otherwise, merge with result object and return
    # acc = helper.result()
    # result.merge(acc)
    # return result


def MonitorPhotonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'PhotonAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example
    # algorithm in the AthenaMonitoring package. Just pass the type to the
    # helper. Then, the helper will instantiate an instance and set up the
    # base class configuration following the inputFlags. The returned object
    # is the algorithm.

    MonitorPhotonAlgorithm=CompFactory.MonitorPhotonAlgorithm
    phCBTightMonAlg = helper.addAlgorithm(MonitorPhotonAlgorithm,'phCBTightMonAlg')
    phCBTightTrigMonAlg = helper.addAlgorithm(MonitorPhotonAlgorithm,'phCBTightTrigMonAlg')

    phCBLooseMonAlg = helper.addAlgorithm(MonitorPhotonAlgorithm,'phCBLooseMonAlg')
    phCBLooseTrigMonAlg = helper.addAlgorithm(MonitorPhotonAlgorithm,'phCBLooseTrigMonAlg')

    ### STEP 3 ###
    # Edit properties of algorithms

    phCBTightMonAlg.ParticleContainerName = "Photons"
    phCBTightMonAlg.RecoName = "Tight"
    phCBTightMonAlg.ParticlePrefix = "photon"
    phCBTightMonAlg.CheckTrigger = False 
    # to enable a trigger filter, for example:
    # phCBTightMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    phCBTightTrigMonAlg.ParticleContainerName = "Photons"
    phCBTightTrigMonAlg.RecoName = "Tight"
    phCBTightTrigMonAlg.ParticlePrefix = "photon"
    phCBTightTrigMonAlg.CheckTrigger = True 
    phCBTightTrigMonAlg.WithTrigger = "WithTrigger" 
    # to enable a trigger filter, for example:
    # phCBTightTrigMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    phCBLooseMonAlg.ParticleContainerName = "Photons"
    phCBLooseMonAlg.RecoName = "Loose"
    phCBLooseMonAlg.ParticlePrefix = "photon"
    phCBLooseMonAlg.CheckTrigger = False 
    # to enable a trigger filter, for example:
    # phCBLooseMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    phCBLooseTrigMonAlg.ParticleContainerName = "Photons"
    phCBLooseTrigMonAlg.RecoName = "Loose"
    phCBLooseTrigMonAlg.ParticlePrefix = "photon"
    phCBLooseTrigMonAlg.CheckTrigger = True 
    phCBLooseTrigMonAlg.WithTrigger = "WithTrigger" 
    # to enable a trigger filter, for example:
    # phCBLooseTrigMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # First, add a tool that's set up by a different configuration function.
    # In this case, CaloNoiseToolCfg returns its own component accumulator,
    # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # photonMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned
    # object here is the standard GenericMonitoringTool.

    GroupPhotonCBTight = helper.addGroup(phCBTightMonAlg, 'MonitorPhoton', 'egamma/CBTightPhotons/')
    GroupPhotonCBTightTriggered = helper.addGroup(phCBTightTrigMonAlg, 'MonitorPhoton', 'egamma/CBTightTriggeredPhotons/')
    GroupPhotonCBLoose = helper.addGroup(phCBLooseMonAlg, 'MonitorPhoton', 'egamma/CBLoosePhotons/')
    GroupPhotonCBLooseTriggered = helper.addGroup(phCBLooseTrigMonAlg, 'MonitorPhoton', 'egamma/CBLooseTriggeredPhotons/')

    ### STEP 5 ###
    # Configure histograms

    # Cut Based Tight Photons
    BookHistograms(GroupPhotonCBTight,"CBTight","Photon")
    BookHistograms(GroupPhotonCBTightTriggered,"CBTightTrig","Photon")
    # Cut Based Loose Photons
    BookHistograms(GroupPhotonCBLoose,"CBLoose","Photon")
    BookHistograms(GroupPhotonCBLooseTriggered,"CBLooseTrig","Photon")

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can
    # just return directly (and not create "result" above)
    return helper.result()

    # # Otherwise, merge with result object and return
    # acc = helper.result()
    # result.merge(acc)
    # return result


if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'MonitorEgammaOutput.root'

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    MonitorElectronAcc = MonitorElectronConfig(ConfigFlags)
    cfg.merge(MonitorElectronAcc)

    MonitorPhotonAcc = MonitorPhotonConfig(ConfigFlags)
    cfg.merge(MonitorPhotonAcc)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run(20) #use cfg.run(20) to only run on first 20 events
