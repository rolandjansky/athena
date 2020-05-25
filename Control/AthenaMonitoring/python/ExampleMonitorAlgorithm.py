#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

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
    # This uses the new Configurables object system.
    from AthenaConfiguration.ComponentFactory import CompFactory
    exampleMonAlg = helper.addAlgorithm(CompFactory.ExampleMonitorAlgorithm,'ExampleMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    anotherExampleMonAlg = helper.addAlgorithm(CompFactory.ExampleMonitorAlgorithm,'AnotherExampleMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #exampleMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

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
    myGroup.defineHistogram('lumiPerBCID',title='Luminosity,WithCommaInTitle;L/BCID;Events',
                            path='ToRuleThemAll',xbins=40,xmin=0.0,xmax=80.0)
    myGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
                            path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5,weight='testweight')
    myGroup.defineHistogram('random;random2', title='LB;x;Events',
                            path='ToBringThemAll',xbins=30,xmin=0,xmax=1,opt='kLBNHistoryDepth=10')
    myGroup.defineHistogram('random', title='title;x;y',path='ToBringThemAll',
                            xbins=[0,.1,.2,.4,.8,1.6])
    myGroup.defineHistogram('random,pT', type='TH2F', title='title;x;y',path='ToBringThemAll',
                            xbins=[0,.1,.2,.4,.8,1.6],ybins=[0,10,30,40,60,70,90])
    # specify a merge method
    myGroup.defineHistogram('lumiPerBCID;lumiPerBCID_merge',title='Luminosity,WithCommaInTitle;L/BCID;Events',
                            path='ToRuleThemAll',xbins=40,xmin=0.0,xmax=80.0, merge='weightedAverage')
    # TEfficiencies
    myGroup.defineHistogram('pT_passed,pT', type='TEfficiency', title='Test TEfficiency;x;Eff',
                            path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0)
    myGroup.defineHistogram('pT_passed,pT,random', type='TEfficiency', title='Test TEfficiency 2D;x;y;Eff',
                            path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0,
                            ybins=10, ymin=0.0, ymax=2.0)
    # # use a cutmask to only fill certain events
    myGroup.defineHistogram('pT;pT_with_cut', title='p_{T};p_{T};Events', path='AndInTheDarkness',
                             xbins=50, xmin=0, xmax=50, cutmask='pT_passed')
    # make a TTree
    myGroup.defineTree('pT,lb,pT_vec,strvec,str;testtree', path='BindThem',
                        treedef='pT/F:lb/i:pT_vec/vector<float>:strvec/vector<string>:str/string')

    anotherGroup.defineHistogram('lbWithFilter',title='Lumi;lb;Events',
                                 path='top',xbins=1000,xmin=-0.5,xmax=999.5)
    anotherGroup.defineHistogram('run',title='Run Number;run;Events',
                                 path='top',xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Example defining an array of histograms. This is useful if one seeks to create a
    # number of histograms in an organized manner. (For instance, one plot for each ASIC
    # in the subdetector, and these components are mapped in eta, phi, and layer.) Thus,
    # one might have an array of TH1's such as quantity[etaIndex][phiIndex][layerIndex].
    for alg in [exampleMonAlg, anotherExampleMonAlg]:
        # Using an array of groups
        topPath = 'OneRing' if alg == exampleMonAlg else 'top'
        array = helper.addArray([2],alg,'ExampleMonitor', topPath=topPath)
        array.defineHistogram('a,b',title='AB',type='TH2F',path='Eta',
                              xbins=10,xmin=0.0,xmax=10.0,
                              ybins=10,ymin=0.0,ymax=10.0)
        array.defineHistogram('c',title='C',path='Eta',
                              xbins=10,xmin=0.0,xmax=10.0)
        array = helper.addArray([4,2],alg,'ExampleMonitor', topPath=topPath)
        array.defineHistogram('a',title='A',path='EtaPhi',
                              xbins=10,xmin=0.0,xmax=10.0)

        # Using a map of groups
        layerList = ['layer1', 'layer2']
        clusterList = ['clusterX', 'clusterB']
        array1D = helper.addArray([layerList], alg, 'ExampleMonitor', topPath=topPath)
        array1D.defineHistogram('c', title='C', path='Layer',
                                xbins=10, xmin=0, xmax=10.0)
        array2D = helper.addArray([layerList, clusterList], alg, 'ExampleMonitor', topPath=topPath)
        array2D.defineHistogram('c', title='C', path='LayerCluster',
                                xbins=10, xmin=0, xmax=10.0)

        # Using templates for histogram titles or paths
        array1D.defineHistogram('c', title='Layer {0}', path='Keys', xmax=3.)
        array1D.defineHistogram('c;c_alternate', title='Layer', path='Keys/{0}', xmax=3.)
        array2D.defineHistogram('c', title='Cluster {1}, Layer {0}', path='Keys/{1}', xmax=3.)

        # Making a histogram only for certain elements of the array
        array2D.defineHistogram('c;c_restricted', path='Keys', pattern=[('layer1', 'clusterB'), ('layer2', 'clusterX')])

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
    import sys
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    ConfigFlags.fillFromArgs(sys.argv[1:])
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = ExampleMonitoringConfig(ConfigFlags)
    cfg.merge(exampleMonitorAcc)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
