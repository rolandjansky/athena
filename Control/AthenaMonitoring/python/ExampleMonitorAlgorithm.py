#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file ExampleMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief Example python configuration for the Run III AthenaMonitoring package
'''

def ExampleMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'ExampleAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from AthenaMonitoring.AthenaMonitoringConf import ExampleMonitorAlgorithm
    exampleMonAlg = helper.addAlgorithm(ExampleMonitorAlgorithm,'ExampleMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    anotherExampleMonAlg = helper.addAlgorithm(ExampleMonitorAlgorithm,'AnotherExampleMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    exampleMonAlg.TriggerChain = ''

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # exampleMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myGroup = helper.addGroup(
        exampleMonAlg,
        'ExampleMonitor',
        'OneRing/'
    )

    # Add a GMT for the other example monitor algorithm
    anotherGroup = helper.addGroup(anotherExampleMonAlg,'ExampleMonitor')

    ### STEP 5 ###
    # Configure histograms
    myGroup.defineHistogram('lumiPerBCID;lumiPerBCID', title='Luminosity;L/BCID;Events',
                            path='ToRuleThemAll',xbins=10,xmin=0.0,xmax=10.0)
    myGroup.defineHistogram('lb;lb', title='Luminosity Block;lb;Events',
                            path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5)
    myGroup.defineHistogram('random;random', title='LB;x;Events',
                            path='ToBringThemAll',xbins=30,xmin=0,xmax=1)


    anotherGroup.defineHistogram('lbWithFilter;lbWithFilter',title='Lumi;lb;Events',
                                 path='top',xbins=1000,xmin=-0.5,xmax=999.5)


    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()
    
    # # Otherwise, merge with result object and return
    # acc, seq = helper.result()
    # result.merge(acc)
    # return result, seq


if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG,INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc,exampleMonitorAlg = ExampleMonitoringConfig(ConfigFlags)
    
    cfg.merge(exampleMonitorAcc)
    cfg.run()
