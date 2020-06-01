#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigBphysMonitorAlgorithm.py
@author Semen Turchikhin
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-08-23
@brief Example trigger python configuration for the Run III AthenaMonitoring package, based on the example by C Burton and P Onyisi
'''

def TrigBphysMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    #from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    #result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigBphysAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # The added algorithm must exist as a .h file 

    from AthenaConfiguration.ComponentFactory import CompFactory
    trigBphysMonAlg = helper.addAlgorithm(CompFactory.TrigBphysMonitorAlgorithm,'TrigBphysMonAlg')
    #from AthenaCommon.Constants import DEBUG,INFO
    #trigBphysMonAlg.OutputLevel = INFO

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    #shifterTrigBphysMonAlg = helper.addAlgorithm(TrigBphysMonitorAlgorithm,'ShifterTrigBphysMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # trigBphysMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #trigBphysMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #trigBphysMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'
    monitored_chains = ['HLT_2mu10_bJpsimumu_L12MU10', 'HLT_2mu10_bUpsimumu_L12MU10']
    monitored_containers = ['HLT_DimuEF']
    trigBphysMonAlg.MonitoredChains = monitored_chains
    trigBphysMonAlg.MonitoredContainers = monitored_containers

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decision tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # trigBphysMonAlg.CaloNoiseTool = caloNoiseTool
    

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # trigBphysMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    expertGroup = helper.addGroup(
        trigBphysMonAlg,
        'TrigBphysMonitor',
        'HLT/BphysMon/Expert/'
    )
      

    # Add a GMT for the other example monitor algorithm
    #shifterGroup = helper.addGroup(shifterTrigBphysMonAlg,'TrigBphysMonitor','HLT/BphysMon/Shifter/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    for monitored_chain in monitored_chains : 
      # number of events histograms will be shown in root Containers/ folder
      expertGroup.defineHistogram('ncandidates_Chain_'+monitored_chain,title='Number of Bphys candidates;number of candidates;Entries',
                                  path='Chains',xbins=10,xmin=-0.5,xmax=9.5)
      # the others in per-container folders
      expertGroup.defineHistogram('dimu_mass_Chain_'+monitored_chain,title='Dimuon mass;m(#mu^{+}#mu^{-}) [GeV];Events / (0.1 GeV)',
                                  path='Chains/'+monitored_chain,xbins=150,xmin=0.0,xmax=15.0)
      expertGroup.defineHistogram('dimu_pt_Chain_'+monitored_chain,title='Dimuon transverse momentum;p_{T}(#mu^{+}#mu^{-}) [GeV];Events / (1 GeV)',
                                  path='Chains/'+monitored_chain,xbins=40,xmin=0.0,xmax=40.0)
      expertGroup.defineHistogram('dimu_chi2_Chain_'+monitored_chain,title='Dimuon #chi^{2};#chi^{2}(#mu^{+}#mu^{-});Events / (0.5)',
                                  path='Chains/'+monitored_chain,xbins=80,xmin=0.0,xmax=40.0)
    for monitored_container in monitored_containers : 
      # number of events histograms will be shown in root Containers/ folder
      expertGroup.defineHistogram('ncandidates_Container_'+monitored_container,title='Number of Bphys candidates;number of candidates;Entries',
                                  path='Containers',xbins=10,xmin=-0.5,xmax=9.5)
      # the others in per-container folders
      expertGroup.defineHistogram('dimu_mass_Container_'+monitored_container,title='Dimuon mass;m(#mu^{+}#mu^{-}) [GeV];Events / (0.1 GeV)',
                                  path='Containers/'+monitored_container,xbins=150,xmin=0.0,xmax=15.0)

    #shifterGroup.defineHistogram('run',title='Run Number;run;Events',
                                  #path='SomePath',xbins=1000000,xmin=-0.5,xmax=999999.5)

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
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TrigBphysMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigBphysMonitorAcc = TrigBphysMonConfig(ConfigFlags)
    cfg.merge(trigBphysMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigBphysMonitorAcc.getEventAlgo('TrigBphysMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

