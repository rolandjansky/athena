#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigBjetMonitorAlgorithm.py
@author E. Nagy
@author T. Bold
@date 2019-04-16
@brief Example trigger python configuration for the Run III AthenaMonitoring package, based on the example by C Burton and P Onyisi
'''

def TrigBjetMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigBjetAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #The added algorithm must exist as a .h file 

    from TrigBjetMonitoring.TrigBjetMonitoringConf import TrigBjetMonitorAlgorithm
    trigBjetMonAlg = helper.addAlgorithm(TrigBjetMonitorAlgorithm,'TrigBjetMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    shifterTrigBjetMonAlg = helper.addAlgorithm(TrigBjetMonitorAlgorithm,'ShifterTrigBjetMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # trigBjetMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #trigBjetMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #trigBjetMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'
    trigBjetMonAlg.TriggerChain = ''

    ### STEP 4 ###
    # Read in the Bjet trigger chain names

    # Directly from TrigBjetMonitCategory
    #from TrigBjetMonitoring import TrigBjetMonitCategory
    #bjet_triglist = TrigBjetMonitCategory.monitoring_bjet

    # From the hltmonList where TrigHLTMonitoring filtered the run type
    from TrigHLTMonitoring.HLTMonTriggerList import hltmonList
    bjet_triglist = hltmonList.monitoring_bjet

    print " ==> bjet_triglist: ", bjet_triglist
    expert = []
    shifter = []
    for chain in bjet_triglist :
        if chain[0:1] == "E" :
            expert.append(chain[2:])
        if chain[0:1] == "S" :
            shifter.append(chain[2:])
    print " ==> expert folder: ", expert
    print " ==> shifter folder: ", shifter
    trigBjetMonAlg.expert = expert
    trigBjetMonAlg.shifter = shifter

    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # trigBjetMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # trigBjetMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myGroup = helper.addGroup(
        trigBjetMonAlg,
        'TrigBjetMonitor',
        'HLT/BjetMon/Expert/'
    )

    # Add a GMT for the other example monitor algorithm
    shifterGroup = helper.addGroup(shifterTrigBjetMonAlg,'TrigBjetMonitor','HLT/BjetMon/Shifter/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    print " ==> expert[0]: ", expert[0]

    for i in range ( len(expert) ) :
        EHist = 'E' + str(i+1) + 'd0;E' + str(i+1) + 'd0'
        print " ==> EHist: ", EHist
        myGroup.defineHistogram(EHist, title='Distribution of d0;d0;Events',
                                path=expert[i],xbins=10,xmin=-1.0,xmax=1.0)

    #myGroup.defineHistogram('E1d0;E1d0', title='Distribution of d0;d0;Events',
    #                        path=expert[0],xbins=10,xmin=-1.0,xmax=1.0)
    #myGroup.defineHistogram('E2d0;E2d0', title='Distribution of d0;d0;Events',
    #                        path=expert[1],xbins=10,xmin=-1.0,xmax=1.0)

    #myGroup.defineHistogram('lumiPerBCID',title='Luminosity;L/BCID;Events',
    #                        path='ToRuleThemAll',xbins=10,xmin=0.0,xmax=10.0)
    #myGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
    #                        path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5)
    #myGroup.defineHistogram('random', title='LB;x;Events',
    #                        path='ToBringThemAll',xbins=30,xmin=0,xmax=1,opt='kLBNHistoryDepth=10')
    #myGroup.defineHistogram('pT_passed,pT',type='TEfficiency',title='Test TEfficiency;x;Eff',
    #                        path='AndInTheDarkness',xbins=100,xmin=0.0,xmax=50.0)


    print " ==> shifter[0]: ", shifter[0]

    for i in range ( len(shifter) ) :
        SHist = 'S' + str(i+1) + 'd0;S' + str(i+1) + 'd0'
        print " ==> SHist: ", SHist
        shifterGroup.defineHistogram(SHist, title='Distribution of d0;d0;Events',
                                     path=shifter[i],xbins=10,xmin=-1.0,xmax=1.0)

    #shifterGroup.defineHistogram('S1d0;S1d0', title='Distribution of d0;d0;Events',
    #                             path=shifter[0],xbins=10,xmin=-1.0,xmax=1.0)
    #shifterGroup.defineHistogram('S2d0;S2d0', title='Distribution of d0;d0;Events',
    #                             path=shifter[1],xbins=10,xmin=-1.0,xmax=1.0)

    shifterGroup.defineHistogram('S2IP3D_pu;S2IP3D_pu', title='IP3D_pu probability distribution;IP3D_pu;Events',
                                 path=shifter[1],xbins=50,xmin=0.0,xmax=1.0)

    #shifterGroup.defineHistogram('run',title='Run Number;run;Events',
    #                              path='SomePath',xbins=1000000,xmin=-0.5,xmax=999999.5)

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
    from AthenaCommon.Constants import DEBUG,INFO
    # log.setLevel(DEBUG)
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TrigBjetMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigBjetMonitorAcc = TrigBjetMonConfig(ConfigFlags)
    cfg.merge(trigBjetMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigBjetMonitorAcc.getEventAlgo('TrigBjetMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    # cfg.run() #use cfg.run(20) to only run on first 20 events
    Nevents = 10
    cfg.run(Nevents)

