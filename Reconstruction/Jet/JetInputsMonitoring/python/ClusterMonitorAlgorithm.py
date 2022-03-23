#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file ClusterMonitorAlgorithm.py
@author L. Sawyer
@date 2019-08-15
@python configuration for the Run III Calo Cluster Monitoring package
'''

def ClusterMonitoringConfig(inputFlags, ClusterKey="CaloCalTopoClusters"):
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
    helper = AthMonitorCfgHelper(inputFlags,'ClusterAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # This uses the new Configurables object system.
    from AthenaConfiguration.ComponentFactory import CompFactory
    clusterMonAlg = helper.addAlgorithm(CompFactory.ClusterMonitorAlgorithm,ClusterKey+'MonAlg')

    clusterMonAlg.ClusterKey=ClusterKey

    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #clusterMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    # cluster energy threshold in GeV
    clusterMonAlg.lowEthresh = 5.0
    clusterMonAlg.medEthresh = 10.0
    clusterMonAlg.medhiEthresh = 15.0
    clusterMonAlg.hiEthresh = 20.0


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # clusterMonAlg.MyDomainTool = MyDomainTool()


    #Def hist-path based on input key
    sgKeytoHistName={"CaloCalTopoClusters":"CaloTopoClusters",
                     "egClusterCollection":"LArClusterEM"}
        
    histName=sgKeytoHistName[ClusterKey] if ClusterKey in sgKeytoHistName else ClusterKey

    

    # Add monitor groups. 
    clustersAllGroup = helper.addGroup(
        alg=clusterMonAlg,
        name='ClusterMonitorAllClusters',
        topPath=histName+'/AllClusters/'
    )

    clustersExpertGroup = helper.addGroup(
        alg=clusterMonAlg,
        name='ClusterMonitorExpertPlots',
        topPath=histName+'/AllClusters/Expert/'
    )

    clustersCalECAGroup = helper.addGroup(
        alg=clusterMonAlg,
        name='ClusterMonitorCalECA',
        topPath=histName+'/CalECA/'
    )

    clustersCalBARGroup = helper.addGroup(
        alg=clusterMonAlg,
        name='ClusterMonitorCalBAR',
        topPath=histName+'/CalBAR/'
    )

    clustersCalECCGroup = helper.addGroup(
        alg=clusterMonAlg,
        name='ClusterMonitorCalECC',
        topPath=histName+'/CalECC/'
    )


    # Configure histograms - All Clusters
    clustersAllGroup.defineHistogram('nClusters',title='No. of Clusters;N_{clusters};Events',xbins=200,xmin=0.0,xmax=1000.0)    
    clustersAllGroup.defineHistogram('clusterEta',title='Cluster eta;eta;Events',xbins=98,xmin=-4.9,xmax=4.9)
    clustersAllGroup.defineHistogram('clusterPhi', title='Cluster phi;phi;Events',xbins=64,xmin=-3.15,xmax=3.15)
    clustersAllGroup.defineHistogram('clusterE', title='Cluster energy;E;Events',xbins=400,xmin=0.0,xmax=4000)
    clustersAllGroup.defineHistogram('clusterET', title='Cluster transverse energy;ET;Events',xbins=400,xmin=0.0,xmax=4000)

    clustersAllGroup.defineHistogram('clusterEta,clusterE', type='TH2F',title='Cluster energy vs eta;eta;E',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=300,ymin=0.0,ymax=3000.0 )
    clustersAllGroup.defineHistogram('clusterEta,clusterET', type='TH2F', title='Cluster transverse energy vs eta;eta;E_{T}',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=300,ymin=0.0,ymax=3000.0 )
    clustersAllGroup.defineHistogram('clusterPhi,clusterE', type='TH2F',title='Cluster energy vs phi;phi;E',
                            xbins=64,xmin=-3.15,xmax=3.15,ybins=300,ymin=0.0,ymax=3000.0 )
    clustersAllGroup.defineHistogram('clusterPhi,clusterET', type='TH2F', title='Cluster transverse energy vs phi;phi;E_{T}',
                            xbins=64,xmin=-3.15,xmax=3.15,ybins=300,ymin=0.0,ymax=3000.0 )
    clustersAllGroup.defineHistogram('clusterEta,clusterPhi,clusterE', type='TProfile2D', title='Average Cluster Energy;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersAllGroup.defineHistogram('clusterEta,clusterPhi,clusterET', type='TProfile2D', title='Average Cluster Transverse Energy;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )

    # Configure histograms - All Clusters:Expert Plots
    clustersExpertGroup.defineHistogram('nCells',title='No. of Cells in a Clusters;N_{cells};Events',xbins=200,xmin=0.0,xmax=200.0)    
    clustersExpertGroup.defineHistogram('nBadCells',title='No. of Bad Cells in a Clusters;N_{bad cells};Events',xbins=11,xmin=-1.0,xmax=10.0)
    clustersExpertGroup.defineHistogram('EBadCells',title='E of Bad Cells in a Clusters;E_{bad cells};Events',xbins=210,xmin=-10.0,xmax=100.0)
    clustersExpertGroup.defineHistogram('clusterEta,clusterPhi,nCells', type='TProfile2D', title='No. of Cells in a Clusters;eta;phi',    
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersExpertGroup.defineHistogram('clusterEta,clusterPhi,nBadCells', type='TProfile2D', title='No. of Bad Cells in a Clusters;eta;phi',    
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersExpertGroup.defineHistogram('clusterTime',title='Cluster Time;time;Events',xbins=200,xmin=-100.0,xmax=100.0)    
    clustersExpertGroup.defineHistogram('clusterTime,clusterE',type='TH2F',title='Cluster Time vs E;time;E',
                            xbins=200,xmin=-100.0,xmax=100.0,ybins=300,ymin=0.0,ymax=1500.0)   
    clustersExpertGroup.defineHistogram('HotRat',title='Energy Fraction of Highest Cell;E_{frac};Events',xbins=200,xmin=-0.2,xmax=1.8)
    clustersExpertGroup.defineHistogram('clusterIsol',title='Cluster Isolation;Isolation;Events',xbins=60,xmin=-1.0,xmax=2.0)
    clustersExpertGroup.defineHistogram('BadLARQFrac',title='Bad LAR Q Frac.;Frac;Events',xbins=25,xmin=-1.0,xmax=1.5)
    clustersExpertGroup.defineHistogram('EngPos',title='E from Positive Cells;E;Events',xbins=500,xmin=0.0,xmax=10000.0)
    clustersExpertGroup.defineHistogram('AveLARQ',title='Averge LAR Q;LAR Q;Events',xbins=31,xmin=-1000.0,xmax=30000.0)
    clustersExpertGroup.defineHistogram('AveTileQ',title='Average Tile Q;Tile Q;Events',xbins=21,xmin=-10.0,xmax=200.0)
    
    clustersExpertGroup.defineHistogram('clusterEta,clusterPhi,HotRat',type='TProfile2D', title='Energy Fraction of Highest Cell;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersExpertGroup.defineHistogram('clusterEta,clusterPhi,HotRat;vHotRatE',type='TProfile2D', title='Energy Fraction of Highest Cell for E_{frac}>0.9;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='HighHotRat')
    clustersExpertGroup.defineHistogram('clusterEta,clusterPhi;vHotRatOcc',type='TH2F', title='Occupancy of Clusters with Highest Cell E_{frac}>0.9;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='HighHotRat')


    # Configure histograms - Occupancies and energies by region
    #ECA:
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi;Thresh0ECAOcc', type='TH2F',title='Cluster Occupancy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi;Thresh1ECAOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.lowEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold1' )
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi;Thresh2ECAOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.medEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold2' )
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi;Thresh3ECAOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.medhiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold3' )			   			    
    clustersCalECAGroup.defineHistogram('clusterEta4,clusterPhi;Thresh4ECAOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.hiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold4' )

    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh0ECAAveE', type='TProfile2D',title='Average Cluster Energy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh1ECAAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.lowEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold1' )
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh2ECAAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.medEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold2' )
    clustersCalECAGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh3ECAAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.medhiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold3' )
    clustersCalECAGroup.defineHistogram('clusterEta4,clusterPhi,E;Thresh4ECAAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.hiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold4' )
    #BAR:
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi;Thresh0BAROcc', type='TH2F',title='Cluster Occupancy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi;Thresh1BAROcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.lowEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold1' )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi;Thresh2BAROcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.medEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold2' )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi;Thresh3BAROcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.medhiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold3' )			   			    
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi;Thresh4BAROCc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.hiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold4' )			   			    

    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh0BARAveE', type='TProfile2D',title='Average Cluster Energy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh1BARAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.lowEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold1' )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh2BARAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.medEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold2' )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh3BARAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.medhiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold3' )
    clustersCalBARGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh4BARAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.hiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold4' )
    #ECC:
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi;Thresh0ECCOcc', type='TH2F',title='Cluster Occupancy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi;Thresh1ECCOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.lowEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold1' )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi;Thresh2ECCOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.medEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold2' )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi;Thresh3ECCOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.medhiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold3' )			   			    
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi;Thresh4ECCOcc', type='TH2F',title='Cluster Occupancy - Threshold='+str(clusterMonAlg.hiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold4' )			   			    

    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh0ECCAveE', type='TProfile2D',title='Average Cluster Energy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh1ECCAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.lowEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold1' )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh2ECCAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.medEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold2' )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh3ECCAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.medhiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold3' )
    clustersCalECCGroup.defineHistogram('clusterEta,clusterPhi,E;Thresh4ECCAveE', type='TProfile2D',title='Average Cluster Energy - Threshold='+str(clusterMonAlg.hiEthresh)+' GeV;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='Threshold4' )

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
    ConfigFlags.Output.HISTFileName = 'ClusterMonitorOutput.root'
    ConfigFlags.fillFromArgs(sys.argv[1:])
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesThreadedCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    clusterMonitorAcc = ClusterMonitoringConfig(ConfigFlags)
    cfg.merge(clusterMonitorAcc)

    # If you want to turn on more detailed messages ...
    # clusterMonitorAcc.getEventAlgo('ClusterMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
