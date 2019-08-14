#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file ExampleLArMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief Example python configuration for the Run III AthenaMonitoring package
'''

def LArCollisionTimeMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCollisionTimeMonCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from LArMonitoring.LArMonitoringConf import LArCollisionTimeMonAlg
    larCollTimeMonAlg = helper.addAlgorithm(LArCollisionTimeMonAlg,'larCollTimeMonAlg')

    #define the group names here, as you'll use them multiple times
    collTimeGroupName="LArCollisionTimeMonGroup"



    ### STEP 3 ###
    # Edit properties of a algorithm
    collTimeGroupName="LArCollisionTimeGroup"
    larCollTimeMonAlg.CollTimeGroupName=collTimeGroupName


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

    from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
    larCollTimeMonAlg.BunchCrossingTool = BunchCrossingTool()


    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    collTimeGroup = helper.addGroup(
        larCollTimeMonAlg,
        collTimeGroupName,
        '/LAr/'
    )


    ### STEP 5 ###
    # Configure histograms

    from LArMonitoring import GlobalVariables #to define the ranges
    larColTime_hist_path='CollisionTime/'
    
    collTimeGroup.defineHistogram('ecTimeDiff;LArCollTime', 
                                  title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV) per ns',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.colTime_Nbins,xmin=GlobalVariables.colTime_min,xmax=GlobalVariables.colTime_max)
    
    collTimeGroup.defineHistogram('ecTimeAvg;LArCollAvgTime', 
                                  title='LArCollisionAverageTime - avg time of ECC and ECA;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV) per ns',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.avgColTime_Nbins,xmin=GlobalVariables.avgColTime_min,xmax=GlobalVariables.avgColTime_max)
    
    
    collTimeGroup.defineHistogram('ecTimeDiff;LArCollTimeLumiBlock', #this one is the same as LArCollTime, but it's reset once per LB (FUNCTIONALITY TO BE ADDED!)
                                  title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV) per ns',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.colTime_Nbins,xmin=GlobalVariables.colTime_min,xmax=GlobalVariables.colTime_max,
                                  opt='kLBNHistoryDepth=1')
    
    
    collTimeGroup.defineHistogram('lumi_block_timeWindow;LArCollTimeLumiBlockTimeCut',
                                  title='Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB;Luminosity Block Number;Number of events per LB',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max)
    
    
    collTimeGroup.defineHistogram('lumi_block_singleBeam_timeWindow;LArCollTimeLumiBlockSingleBeamTimeCut',
                                  title='Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB;Luminosity Block Number;Number of events per LB',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max)
    
    
    collTimeGroup.defineHistogram('lumi_block,ecTimeDiff;LArCollTime_vs_LB',
                                  title='LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max,
                                  ybins=GlobalVariables.colTime_Nbins,ymin=GlobalVariables.colTime_min,ymax=GlobalVariables.colTime_max)
    
    
    collTimeGroup.defineHistogram('bunch_crossing_id,ecTimeDiff;LArCollTime_vs_BCID',
                                  title='LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID;Bunch Crossing Number;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.BCID_Bins,xmin=GlobalVariables.BCID_Min,xmax=GlobalVariables.BCID_Max,
                                  ybins=GlobalVariables.colTime_Nbins,ymin=GlobalVariables.colTime_min,ymax=GlobalVariables.colTime_max)
    
    collTimeGroup.defineHistogram('lumi_block,ecTimeAvg;LArCollAvgTime_vs_LB',
                                  title='LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max,
                                  ybins=GlobalVariables.avgColTime_Nbins,ymin=GlobalVariables.avgColTime_min,ymax=GlobalVariables.avgColTime_max)
    
    collTimeGroup.defineHistogram('bunch_crossing_id,ecTimeAvg;LArCollAvgTime_vs_BCID',
                                  title='LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of BCID;Bunch Crossing Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=GlobalVariables.BCID_Bins,xmin=GlobalVariables.BCID_Min,xmax=GlobalVariables.BCID_Max,
                                  ybins=GlobalVariables.avgColTime_Nbins,ymin=GlobalVariables.avgColTime_min,ymax=GlobalVariables.avgColTime_max)
    
    #in train monitoring, only done offline

    if not 'isOnline' in dir(): #DO WE NEED THIS IF HERE?
        isOnline=False
        pass

    if isOnline:

        collTimeGroupName_intrain=collTimeGroupName+"_intrain"
        collTimeGroup_intrain = helper_collTime.addGroup( 
            larCollTimeMonAlg,
            collTimeGroupName_intrain,
            "/LAr/"
        )
        larCollTimeMonAlg.InTrain_CollTimeGroupName=collTimeGroupName_intrain #pass the group name to the algorithm, this way you let the algorithm now that this group has been defined (won't be filled otherwise)
        
        intrain_tit=" inside the train"
        intrain_name="_intrain"
        
        collTimeGroup_intrain.defineHistogram('ecTimeDiff;LArCollTime'+intrain_name, 
                                              title='LArCollisionTime - difference of avg time from ECC and ECA'+intrain_tit+';<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV) per ns',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.colTime_Nbins,xmin=GlobalVariables.colTime_min,xmax=GlobalVariables.colTime_max)
        
        
        collTimeGroup_intrain.defineHistogram('ecTimeAvg;LArCollAvgTime'+intrain_name, 
                                              title='LArCollisionAverageTime - avg time of ECC and ECA'+intrain_tit+';(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV) per ns',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.avgColTime_Nbins,xmin=GlobalVariables.avgColTime_min,xmax=GlobalVariables.avgColTime_max)
        
        
        collTimeGroup_intrain.defineHistogram('ecTimeDiff;LArCollTimeLumiBlock'+intrain_name, #this one is the same as LArCollTime, but it's reset once per LB (FUNCTIONALITY TO BE ADDED!)
                                              title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> (ns)'+intrain_tit+';Number of events (weighted by energy/GeV) per ns',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.colTime_Nbins,xmin=GlobalVariables.colTime_min,xmax=GlobalVariables.colTime_max)
        
        
        collTimeGroup_intrain.defineHistogram('(lumi_block_timeWindow);LArCollTimeLumiBlockTimeCut'+intrain_name,
                                              title='Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB'+intrain_tit+';Luminosity Block Number;Number of events per LB',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max)
        
        
        collTimeGroup_intrain.defineHistogram('(lumi_block_singleBeam_timeWindow);LArCollTimeLumiBlockSingleBeamTimeCut'+intrain_name,
                                              title='Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB'+intrain_tit+';Luminosity Block Number;Number of events per LB',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max)
        
        
        collTimeGroup_intrain.defineHistogram('lumi_block,ecTimeDiff;LArCollTime_vs_LB'+intrain_name,
                                              title='LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number'+intrain_tit+';<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max,
                                              ybins=GlobalVariables.colTime_Nbins,ymin=GlobalVariables.colTime_min,ymax=GlobalVariables.colTime_max)
        
        
        collTimeGroup_intrain.defineHistogram('bunch_crossing_id,ecTimeDiff;LArCollTime_vs_BCID'+intrain_name,
                                              title='LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID'+intrain_tit+';Bunch Crossing Number;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.BCID_Bins,xmin=GlobalVariables.BCID_Min,xmax=GlobalVariables.BCID_Max,
                                              ybins=GlobalVariables.colTime_Nbins,ymin=GlobalVariables.colTime_min,ymax=GlobalVariables.colTime_max)
        
        collTimeGroup_intrain.defineHistogram('lumi_block,ecTimeAvg;LArCollAvgTime_vs_LB'+intrain_name,
                                              title='LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block'+intrain_tit+';Luminosity Block Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.LB_Bins,xmin=GlobalVariables.LB_Min,xmax=GlobalVariables.LB_Max,
                                              ybins=GlobalVariables.avgColTime_Nbins,ymin=GlobalVariables.avgColTime_min,ymax=GlobalVariables.avgColTime_max)
        
        collTimeGroup_intrain.defineHistogram('bunch_crossing_id,ecTimeAvg;LArCollAvgTime_vs_BCID'+intrain_name,
                                              title='LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of BCID'+intrain_tit+';Bunch Crossing Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=GlobalVariables.BCID_Bins,xmin=GlobalVariables.BCID_Min,xmax=GlobalVariables.BCID_Max,
                                              ybins=GlobalVariables.avgColTime_Nbins,ymin=GlobalVariables.avgColTime_min,ymax=GlobalVariables.avgColTime_max)

        pass #end of if isOnline


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
    ConfigFlags.Output.HISTFileName = 'LArCollisionTimeMonOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

#    larCollTimeMonitorAcc,larCollTimeMonitorAlg = LArCollisionTimeMonConfig(ConfigFlags)

    larCollTimeMonitorAcc = LArCollisionTimeMonConfig(ConfigFlags) 
   
    cfg.merge(larCollTimeMonitorAcc)
    Nevents=10
    cfg.run(Nevents) #use cfg.run() to run on all events
