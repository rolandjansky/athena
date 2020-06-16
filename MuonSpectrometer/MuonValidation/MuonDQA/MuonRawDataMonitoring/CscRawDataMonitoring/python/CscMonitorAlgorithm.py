#
#  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory

def CscMonitoringConfigOld(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    helper = AthMonitorCfgHelperOld(inputFlags,'CscAthMonitorCfg')
    return helper.result()

def CscMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))


       # Temporary, until we move to services/private tools-- from MuonSpectrometer/MuonConfig
   # result.addPublicTool( CompFactory.Muon__MuonIdHelperTool() )


    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'CscAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # This uses the new Configurables object system.
    cscClusMonAlg = helper.addAlgorithm(CompFactory.CscClusterValMonAlg,'CscClusMonAlg')
    cscPrdMonAlg = helper.addAlgorithm(CompFactory.CscPrdValMonAlg,'CscPrdMonAlg')


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
    cscClusGroup = helper.addGroup(cscClusMonAlg,'CscClusMonitor','Muon/MuonRawDataMonitoring/CSC/')
    cscPrdGroup = helper.addGroup(cscPrdMonAlg,'CscPrdMonitor','Muon/MuonRawDataMonitoring/CSC/')




    ### STEP 5 ###
    # Configure histograms
    cscClusGroup.defineHistogram('z,r',type='TH2F',title='R vs. Z Cluster hitmap;z(mm);R(mm)',
                                 path='Clusters/Shift',xbins=200,xmin=-10000.,xmax=10000., ybins=40, ymin=0., ymax=4000.)

    cscClusGroup.defineHistogram('y,x',type='TH2F',title='X vs. Y Cluster hitmap;y(mm);x(mm)',
                                 path='Clusters/Shift',xbins=100,xmin=-5000.,xmax=5000.,ybins=100,ymin=-5000,ymax=5000)

    cscClusGroup.defineHistogram('noStrips,secLayer',type='TH2F',title='Phi-Cluster width;# strips;[sector] + [0.2 times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer',type='TH2F',title='Eta-Cluster width;# strips;[sector] + [0.2 times layer]',
                                  path='Clusters/Expert',xbins=192, xmin=0, xmax=192, ybins=175, ymin=-17, ymax=18)

    cscClusGroup.defineHistogram('stripid,secLayer',type='TH2F',title='Cluster occupancy;channel;[sector]+[0.2 times layer]',
                                  path='Clusters/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscPrdGroup.defineHistogram('z,r',type='TH2F',title='Prd R vs. Z hitmap;z(mm);R(mm)',
                                path='PRD/Shift',xbins=200,xmin=-10000.,xmax=10000., ybins=40, ymin=0., ymax=4000.)


    #myGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
    #                       path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5,weight='testweight')
    #myGroup.defineHistogram('random', title='LB;x;Events',
    #                        path='ToBringThemAll',xbins=30,xmin=0,xmax=1,opt='kLBNHistoryDepth=10')
    #myGroup.defineHistogram('random', title='title;x;y',path='ToBringThemAll',
    #                        xbins=[0,.1,.2,.4,.8,1.6])
    ##myGroup.defineHistogram('random,pT', type='TH2F', title='title;x;y',path='ToBringThemAll',
     #                       xbins=[0,.1,.2,.4,.8,1.6],ybins=[0,10,30,40,60,70,90])
    # TEfficiencies
    ##myGroup.defineHistogram('pT_passed,pT', type='TEfficiency', title='Test TEfficiency;x;Eff',
    #                        path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0)
    #myGroup.defineHistogram('pT_passed,pT,random', type='TEfficiency', title='Test TEfficiency 2D;x;y;Eff',
    #                        path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0,
    #                        ybins=10, ymin=0.0, ymax=2.0)
    # # use a cutmask to only fill certain events
    #myGroup.defineHistogram('pT;pT_with_cut', title='p_{T};p_{T};Events', path='AndInTheDarkness',
    #                         xbins=50, xmin=0, xmax=50, cutmask='pT_passed')
    # make a TTree
    #myGroup.defineTree('pT,lb,pT_vec,strvec,str;testtree', path='BindThem',
    #                    treedef='pT/F:lb/i:pT_vec/vector<float>:strvec/vector<string>:str/string')

    #anotherGroup.defineHistogram('lbWithFilter',title='Lumi;lb;Events',
    #                             path='top',xbins=1000,xmin=-0.5,xmax=999.5)
    #anotherGroup.defineHistogram('run',title='Run Number;run;Events',
    #                             path='top',xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Example defining an array of histograms. This is useful if one seeks to create a
    # number of histograms in an organized manner. (For instance, one plot for each ASIC
    # in the subdetector, and these components are mapped in eta, phi, and layer.) Thus,
    # one might have an array of TH1's such as quantity[etaIndex][phiIndex][layerIndex].
   # for alg in [exampleMonAlg,anotherExampleMonAlg]:
        # Using an array of groups
    #    topPath = 'OneRing' if alg == exampleMonAlg else ''
    #    array = helper.addArray([2],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('a,b',title='AB',type='TH2F',path='Eta',
    #                          xbins=10,xmin=0.0,xmax=10.0,
    #                          ybins=10,ymin=0.0,ymax=10.0)
    #    array.defineHistogram('c',title='C',path='Eta',
     #                         xbins=10,xmin=0.0,xmax=10.0)
    #    array = helper.addArray([4,2],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('a',title='A',path='EtaPhi',
     #                         xbins=10,xmin=0.0,xmax=10.0)
        # Using a map of groups
    #    layerList = ['layer1','layer2']
    #    clusterList = ['clusterX','clusterB']
    #    array = helper.addArray([layerList],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('c',title='C',path='Layer',
     #                         xbins=10,xmin=0,xmax=10.0)
    #    array = helper.addArray([layerList,clusterList],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('c',title='C',path='LayerCluster',
     #                         xbins=10,xmin=0,xmax=10.0)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #return helper.result()
    
    # # Otherwise, merge with result object and return
    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD


    # ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'CscMonitorOutput.root'
    #ConfigFlags.fillFromArgs(sys.argv[1:])
    # ConfigFlags.GeoModel.AtlasVersion="ATLAS-R2-2016-00-01-00"
    ConfigFlags.Muon.doCSCs = True
    ConfigFlags.Muon.doRPCs = False
    ConfigFlags.Muon.doTGCs = False
    ConfigFlags.Detector.GeometryMuon=False
    ConfigFlags.Detector.GeometryCSC=True
    ConfigFlags.Detector.GeometryRPC=False
    ConfigFlags.Detector.GeometryTGC=False
    ConfigFlags.Detector.GeometryMM=False
    ConfigFlags.Detector.GeometryMDT=False
    ConfigFlags.Muon.doMicromegas = False
    #ConfigFlags.Muon.useAlignmentCorrections=False
    ConfigFlags.Muon.Align.UseILines = False
    #ConfigFlags.Muon.Align.UseAsBuilt = True
    ConfigFlags.Muon.Align.UseALines = False
    ConfigFlags.Muon.Align.UseBLines = False


    ConfigFlags.lock()
    ConfigFlags.dump()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cscMonitorAcc = CscMonitoringConfig(ConfigFlags)
    cfg.merge(cscMonitorAcc)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run(20) #use cfg.run(20) to only run on first 20 events
