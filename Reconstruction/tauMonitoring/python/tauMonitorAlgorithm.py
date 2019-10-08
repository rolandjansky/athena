#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file tauMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief tau python configuration for the Run III AthenaMonitoring package
'''

def tauMonitoringConfig(inputFlags):
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
    cfgHelper = AthMonitorCfgHelper(inputFlags, monName='tauMonitorAlgFamily')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from tauMonitoring.tauMonitoringConf import tauMonitorAlgorithm
    tauMonAlgBA = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgBA')
    tauMonAlgCR = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgCR')
    tauMonAlgEC = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgEC')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    #anotherTauMonAlg = cfgHelper.addAlgorithm(tauMonitorAlgorithm,'AnotherTauMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # cfgHelper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #exampleMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    tauMonAlgBA.etaMin = -1.
    tauMonAlgBA.etaMax = 1.3
    tauMonAlgCR.etaMin = 1.3
    tauMonAlgCR.etaMax = 1.7
    tauMonAlgEC.etaMin = 1.7
    tauMonAlgEC.etaMax = 3.5

    tauMonAlgBA.kinGroupName = 'tauMonKinGroupBA'
    tauMonAlgCR.kinGroupName = 'tauMonKinGroupCR'
    tauMonAlgEC.kinGroupName = 'tauMonKinGroupEC'

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myKinGroupBA = cfgHelper.addGroup(alg=tauMonAlgBA, name='tauMonKinGroupBA', topPath='run/BA/' )
    myKinGroupCR = cfgHelper.addGroup(alg=tauMonAlgCR, name='tauMonKinGroupCR', topPath='run/CR/' )
    myKinGroupEC = cfgHelper.addGroup(alg=tauMonAlgEC, name='tauMonKinGroupEC', topPath='run/EC/' )

    # Add a GMT for the other example monitor algorithm
    # anotherGroup = cfgHelper.addGroup(anotherTauMonAlg,'tauMonitor')


    ### STEP 5 ###
    # Configure histograms
    # myKinGroupBA.defineHistogram('lumiPerBCID',title='Luminosity,WithCommaInTitle;L/BCID;Events',
    #                         path='ToRuleThemAll',xbins=40,xmin=0.0,xmax=80.0)
    # myKinGroupBA.defineHistogram('lb', title='Luminosity Block;lb;Events',
    #                         path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5,weight='testweight')
    # myKinGroupBA.defineHistogram('random', title='LB;x;Events',
    #                         path='ToBringThemAll',xbins=30,xmin=0,xmax=1,opt='kLBNHistoryDepth=10')
    # myKinGroupBA.defineHistogram('random', title='title;x;y',path='ToBringThemAll',
    #                         xbins=[0,.1,.2,.4,.8,1.6])
    #myKinGroupBA.defineHistogram('random,pT', type='TH2F', title='title;x;y',path='ToBringThemAll',
    #                             xbins=[0,.1,.2,.4,.8,1.6],ybins=[0,10,30,40,60,70,90])
    
    for itup in [(myKinGroupBA,'BA'),(myKinGroupCR,'CR'),(myKinGroupEC,'EC')]:
        (igroup, postfix) = itup
        igroup.defineHistogram('Eflow', title='Eflow;Eflow;Events', path='Kinematics', 
                               xbins=100, xmin=0., xmax=1000.)
        igroup.defineHistogram('eta,phi', type='TH2F', title='PhiVsEtaTitle;#eta;#phi', path='Kinematics', 
                               #xbins=[0,.1,.2,.4,.8,1.6],ybins=[0,10,30,40,60,70,90])
                               #xbins=[0,4.,8.,10.,15.],ybins=[0.,1.,2.,3.,4.])
                               xbins=30,xmin=-3.0,xmax=3.0,ybins=32,ymin=-3.15,ymax=3.15)


    # myKinGroupBA.defineHistogram('pT_passed,pT',type='TEfficiency',title='Test TEfficiency;x;Eff',
    #                         path='AndInTheDarkness',xbins=100,xmin=0.0,xmax=50.0)

    # anotherGroup.defineHistogram('lbWithFilter',title='Lumi;lb;Events',
    #                             path='top',xbins=1000,xmin=-0.5,xmax=999.5)
    # anotherGroup.defineHistogram('run',title='Run Number;run;Events',
    #                             path='top',xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Example defining an array of histograms. This is useful if one seeks to create a
    # number of histograms in an organized manner. (For instance, one plot for each ASIC
    # in the subdetector, and these components are mapped in eta, phi, and layer.) Thus,
    # one might have an array of TH1's such as quantity[etaIndex][phiIndex][layerIndex].
    #for alg in [exampleMonAlg,anotherTauMonAlg]:
    #    # Using an array of groups
    #    array = cfgHelper.addArray([2],alg,'tauMonitor')
    #    array.defineHistogram('a,b',title='AB',type='TH2F',path='Eta',
    #                          xbins=10,xmin=0.0,xmax=10.0,
    #                          ybins=10,ymin=0.0,ymax=10.0)
    #    array.defineHistogram('c',title='C',path='Eta',
    #                          xbins=10,xmin=0.0,xmax=10.0)
    #    array = cfgHelper.addArray([4,2],alg,'tauMonitor')
    #    array.defineHistogram('a',title='A',path='EtaPhi',
    #                          xbins=10,xmin=0.0,xmax=10.0)
    #    # Using a map of groups
    #    layerList = ['layer1','layer2']
    #    clusterList = ['clusterX','clusterB']
    #    array = cfgHelper.addArray([layerList],alg,'tauMonitor')
    #    array.defineHistogram('c',title='C',path='Layer',
    #                          xbins=10,xmin=0,xmax=10.0)
    #    array = cfgHelper.addArray([layerList,clusterList],alg,'tauMonitor')
    #    array.defineHistogram('c',title='C',path='LayerCluster',
    #                          xbins=10,xmin=0,xmax=10.0)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return cfgHelper.result()
    
    # # Otherwise, merge with result object and return
    # acc = cfgHelper.result()
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
    ConfigFlags.Output.HISTFileName = 'tauRegions.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = tauMonitoringConfig(ConfigFlags)   # calls the main function above
    cfg.merge(exampleMonitorAcc)

    # If you want to turn on more detailed messages ...
    exampleMonitorAcc.getEventAlgo('tauMonAlgBA').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgCR').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgEC').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
