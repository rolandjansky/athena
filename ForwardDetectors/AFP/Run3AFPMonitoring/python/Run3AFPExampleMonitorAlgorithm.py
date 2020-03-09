#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file Run3AFPExampleMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief Example python configuration for the Run III AthenaMonitoring package
'''

def Run3AFPExampleMonitoringConfig(inputFlags):
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
    helper = AthMonitorCfgHelper(inputFlags,'Run3AFPMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPSiLayerAlgorithm
    afpSiLayerAlgorithm = helper.addAlgorithm(AFPSiLayerAlgorithm,'AFPSiLayerAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configuration
    #anotherExampleMonAlg = helper.addAlgorithm(AFPSiLayerAlgorithm,'AnotherExampleMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    #exampleMonAlg.RandomHist = True
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
    AFPGroup = helper.addGroup( afpSiLayerAlgorithm,         'AFPSiLayerTool',         'AFP/'     ) 

    #    station0_1 = helper.addGroup(afpSiLayerAlgorithm, 'AFPSiLayer','Station0/')

    # Add a GMT for the other example monitor algorithm
    #    anotherGroup = helper.addGroup(anotherExampleMonAlg,'AFPSiLayer')
    AFPGroup.defineHistogram('lb', title='Luminosity Block;lb;total number of Hits',  path='Global',xbins=1000,xmin=-0.5,xmax=999.5,weight='nhits') 

    ### STEP 5 ###
    # Configure histogramsIf you want to create histograms with variable bin widths, ROOT provides another constructor suited for this purpose. Instead of passing the data interval and the number of bins, you have to pass an array (single or double precision) of bin edges. When the histogram has n bins, then there are n+1 distinct edges, so the array you pass must be of size n+1.


    # Example defining an array of histograms. This is useful if one seeks to create a
    # number of histograms in an organized manner. (For instance, one plot for each ASIC
    # in the subdetector, and these components are mapped in eta, phi, and layer.) Thus,
    # one might have an array of TH1's such as quantity[etaIndex][phiIndex][layerIndex].
    for alg in [afpSiLayerAlgorithm]:

       # Using a map of groups
       layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
       #stationList = ['FarStation','NearStation'] # TODO XXX
       #sideList = ['Aside' , 'Cside' ]        
       combinedList = [ 'farAside', 'nearAside' , 'nearCside' , 'farCside' ]

       #array = helper.addArray([sideList,stationList,layerList],alg,'AFPSiLayerTool')
       array2D = helper.addArray([combinedList,layerList],alg,'AFPSiLayerTool', topPath = 'Hits')
       array2D.defineHistogram(  'h_hitMap', title='Map of hits', type='TH2F', path='AFPSiLayer', xbins=80,xmin=0.5,xmax=80.5, ybins=336,ymin=0.5,ymax=336.5)
       #array.defineHistogram('h_hitMap', title='hitmap for {0} {1} Layer {2}', path='Keys/{0}', xmax=3. )
       array2D.defineHistogram('h_hitMap', title='hitmap for {0} Layer {1}', path='Keys/{0}', xmax=3. )

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
    nightly = ''#/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00348618/data18_13TeV.00348618.physics_Main.recon.AOD.v220/'
    #file = 'data18_13TeV.00348618.physics_Main.recon.AOD.v220._lb0295._0009.1'
    file = '/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.merge.AOD.r10258_p3399_tid13243079_00/AOD.13243079._000005.pool.root.1'
#/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.deriv.DAOD_STDM7.r10258_p3399_p4030/DAOD_STDM7.20036794._000007.pool.root.1'
#/afs/cern.ch/work/k/kristin/dataAFP/data17_13TeV.00337176.physics_Main.recon.AOD.r10258_p3412_r11501/AOD.18508508._000007.pool.root.1'
#/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00354309/data18_13TeV.00354309.physics_Main.recon.AOD.f946/data18_13TeV.00354309.physics_Main.recon.AOD.f946._lb0130._0001.1'
   # nightly = '/eos/atlas/atlastier0/tzero/prod/data18_13TeV/physics_Main/00357750/data18_13TeV.00357750.physics_Main.recon.AOD.f1041/'
   # file = 'data18_13TeV.00357750.physics_Main.recon.AOD.f1041._lb0105._SFO-7._0103.1'

    #nightly = '/eos/atlas/atlastier0/tzero/prod/data17_13TeV/physics_Main/00337176/data17_13TeV.00337176.physics_Main.recon.AOD.f871/'
    #file = 'data17_13TeV.00337176.physics_Main.recon.AOD.f871._lb0142._0006.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'AFPOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = Run3AFPExampleMonitoringConfig(ConfigFlags)
    cfg.merge(exampleMonitorAcc)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
 #   cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run(100) #use cfg.run(20) to only run on first 20 events

