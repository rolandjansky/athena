#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArCollisionTimeMonConfigOld(inputFlags):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArCollisionTimeMonAlg
    
    larColTime_hist_path='LArCollisionTime'

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArCollisionTimeMonAlgOldCfg')
    LArCollisionTimeMonConfigCore(helper, LArCollisionTimeMonAlg,inputFlags,larColTime_hist_path)

    larColTime_hist_path='LArClusterCollisionTime'
    LArCollisionTimeMonConfigCore(helper, LArCollisionTimeMonAlg,inputFlags,larColTime_hist_path)
    helper.monSeq.LArClusterCollisionTimeMonAlg.Key = "ClusterCollTime"
    helper.monSeq.LArClusterCollisionTimeMonAlg.nCells = 0

    return helper.result()

def LArCollisionTimeMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link them to GenericMonitoringTools                                                                                                                                                

    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCollisionTimeMonAlgCfg')

    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg = LArCollisionTimeCfg(inputFlags)

    larColTime_hist_path='LArCollisionTime'

    from AthenaConfiguration.ComponentFactory import CompFactory
    LArCollisionTimeMonConfigCore(helper, CompFactory.LArCollisionTimeMonAlg,inputFlags,larColTime_hist_path)

    larClusColTime_hist_path='LArClusterCollisionTime'
    LArCollisionTimeMonConfigCore(helper, CompFactory.LArCollisionTimeMonAlg('LArClusterCollisionTimeMonAlg'),inputFlags,larClusColTime_hist_path)
    for algo in helper.monSeq.Members:
       if algo.name == 'LArClusterCollisionTimeMonAlg':
           algo.Key = "ClusterCollTime"
           algo.nCells = 0

    cfg.merge(helper.result())
    return cfg

def LArCollisionTimeMonConfigCore(helper, algoinstance,inputFlags,larColTime_hist_path):


    larCollTimeMonAlg = helper.addAlgorithm(algoinstance,larColTime_hist_path+'MonAlg')


    collTimeGroupName="LArCollisionTimeMonGroup"

    larCollTimeMonAlg.CollTimeGroupName=collTimeGroupName

    #set the unit
    import AthenaCommon.SystemOfUnits as Units
    timeUnit = Units.picosecond
    larCollTimeMonAlg.TimeUnit = timeUnit

    collTimeGroup = helper.addGroup(
        larCollTimeMonAlg,
        collTimeGroupName,
        '/LAr/',
        'run'
    )

    from LArMonitoring.GlobalVariables import lArDQGlobals


    timeUnitName='ps'
    if timeUnit == Units.nanosecond:
        timeUnitName='ns'
        pass

    collTimeGroup.defineHistogram('ecTimeDiff;LArCollTime', 
                                  title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> ('+timeUnitName+');'+('Number of events (weighted by energy/GeV) per %.2f ' % (lArDQGlobals.colTime_BinWidth/timeUnit))+timeUnitName,
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min/timeUnit,xmax=lArDQGlobals.colTime_Max/timeUnit)
    
    collTimeGroup.defineHistogram('ecTimeAvg;LArCollAvgTime', 
                                  title='LArCollisionAverageTime - avg time of ECC and ECA;(<t_{C}> + <t_{A}>) / 2 ('+timeUnitName+(');Number of events (weighted by energy/GeV) per  %.2f ' % (lArDQGlobals.avgColTime_BinWidth/timeUnit))+timeUnitName,
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.avgColTime_Bins,xmin=lArDQGlobals.avgColTime_Min/timeUnit,xmax=lArDQGlobals.avgColTime_Max/timeUnit)
    
    
    collTimeGroup.defineHistogram('ecTimeDiff;LArCollTimeLumiBlock', 
                                  title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> ('+timeUnitName+(');Number of events (weighted by energy/GeV) per  %.2f ' % (lArDQGlobals.colTime_BinWidth/timeUnit))+timeUnitName,
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min/timeUnit,xmax=lArDQGlobals.colTime_Max/timeUnit,
                                  duration='lb')
    
    
    collTimeGroup.defineHistogram('lumi_block_timeWindow;LArCollTimeLumiBlockTimeCut',
                                  title='Events with abs(<t_{C}> - <t_{A}>) < 10 ns as a function of LB;Luminosity Block Number;Number of events per LB',
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
    
    
    collTimeGroup.defineHistogram('lumi_block_singleBeam_timeWindow;LArCollTimeLumiBlockSingleBeamTimeCut',
                                  title='Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB;Luminosity Block Number;Number of events per LB',
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
    
    
    collTimeGroup.defineHistogram('lumi_block,ecTimeDiff;LArCollTime_vs_LB',
                                  title='LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number;<t_{C}> - <t_{A}> ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min/timeUnit,ymax=lArDQGlobals.colTime_Max/timeUnit)
    
    
    collTimeGroup.defineHistogram('bunch_crossing_id,ecTimeDiff;LArCollTime_vs_BCID',
                                  title='LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID;Bunch Crossing Number;<t_{C}> - <t_{A}> ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                  ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min/timeUnit,ymax=lArDQGlobals.colTime_Max/timeUnit)
    
    collTimeGroup.defineHistogram('lumi_block,ecTimeAvg;LArCollAvgTime_vs_LB',
                                  title='LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number;(<t_{C}> + <t_{A}>) / 2 ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min/timeUnit,ymax=lArDQGlobals.avgColTime_Max/timeUnit)
    
    collTimeGroup.defineHistogram('bunch_crossing_id,ecTimeAvg;LArCollAvgTime_vs_BCID',
                                  title='LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of BCID;Bunch Crossing Number;(<t_{C}> + <t_{A}>) / 2 ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                  ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min/timeUnit,ymax=lArDQGlobals.avgColTime_Max/timeUnit)

    
    #in train monitoring, only done offline
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    if ConfigFlags.Common.isOnline:

        collTimeGroupName_intrain=collTimeGroupName+"_intrain"
        collTimeGroup_intrain = helper.collTime.addGroup( 
            larCollTimeMonAlg,
            collTimeGroupName_intrain,
            "/LAr/"
        )
        larCollTimeMonAlg.InTrain_CollTimeGroupName=collTimeGroupName_intrain #pass the group name to the algorithm, this way you let the algorithm now that this group has been defined (won't be filled otherwise)
        
        intrain_title=" inside the train"
        intrain_name="_intrain"
        
        collTimeGroup_intrain.defineHistogram('ecTimeDiff;LArCollTime'+intrain_name, 
                                              title='LArCollisionTime - difference of avg time from ECC and ECA'+intrain_title+';<t_{C}> - <t_{A}> ('+timeUnitName+(');Number of events (weighted by energy/GeV) per %.2f ' % (lArDQGlobals.colTime_BinWidth/timeUnit))+timeUnitName,
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min/timeUnit,xmax=lArDQGlobals.colTime_Max/timeUnit)
        
        
        collTimeGroup_intrain.defineHistogram('ecTimeAvg;LArCollAvgTime'+intrain_name, 
                                              title='LArCollisionAverageTime - avg time of ECC and ECA'+intrain_title+';(<t_{C}> + <t_{A}>) / 2 ('+timeUnitName+(');Number of events (weighted by energy/GeV) per %.2f ' % (lArDQGlobals.avgColTime_BinWidth/timeUnit))+timeUnitName,
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.avgColTime_Bins,xmin=lArDQGlobals.avgColTime_Min/timeUnit,xmax=lArDQGlobals.avgColTime_Max/timeUnit)
        
        
        collTimeGroup_intrain.defineHistogram('ecTimeDiff;LArCollTimeLumiBlock'+intrain_name, 
                                              title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> ('+timeUnitName+')'+intrain_title+(';Number of events (weighted by energy/GeV) per %.2f ' % (lArDQGlobals.colTime_BinWidth/timeUnit))+timeUnitName,
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min/timeUnit,xmax=lArDQGlobals.colTime_Max/timeUnit,
                                              duration='lb')

        
        
        collTimeGroup_intrain.defineHistogram('(lumi_block_timeWindow);LArCollTimeLumiBlockTimeCut'+intrain_name,
                                              title='Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB'+intrain_title+';Luminosity Block Number;Number of events per LB',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
        
        
        collTimeGroup_intrain.defineHistogram('(lumi_block_singleBeam_timeWindow);LArCollTimeLumiBlockSingleBeamTimeCut'+intrain_name,
                                              title='Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB'+intrain_title+';Luminosity Block Number;Number of events per LB',
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
        
        
        collTimeGroup_intrain.defineHistogram('lumi_block,ecTimeDiff;LArCollTime_vs_LB'+intrain_name,
                                              title='LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number'+intrain_title+';<t_{C}> - <t_{A}> ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                              ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min/timeUnit,ymax=lArDQGlobals.colTime_Max/timeUnit)
        
        
        collTimeGroup_intrain.defineHistogram('bunch_crossing_id,ecTimeDiff;LArCollTime_vs_BCID'+intrain_name,
                                              title='LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID'+intrain_title+';Bunch Crossing Number;<t_{C}> - <t_{A}> ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                              ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min/timeUnit,ymax=lArDQGlobals.colTime_Max/timeUnit)
        
        collTimeGroup_intrain.defineHistogram('lumi_block,ecTimeAvg;LArCollAvgTime_vs_LB'+intrain_name,
                                              title='LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block'+intrain_title+';Luminosity Block Number;(<t_{C}> + <t_{A}>) / 2 ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                              ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min/timeUnit,ymax=lArDQGlobals.avgColTime_Max/timeUnit)
        
        collTimeGroup_intrain.defineHistogram('bunch_crossing_id,ecTimeAvg;LArCollAvgTime_vs_BCID'+intrain_name,
                                              title='LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of BCID'+intrain_title+';Bunch Crossing Number;(<t_{C}> + <t_{A}>) / 2 ('+timeUnitName+');Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                              ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min/timeUnit,ymax=lArDQGlobals.avgColTime_Max/timeUnit)

        pass #end of if isOnline


if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Logging import log
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
    createLArMonConfigFlags()

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.DQ.enableLumiAccess = False #copied from LArRecoFromRaw
    ConfigFlags.DQ.useTrigger = False #copied from LArRecoFromRaw 
    ConfigFlags.Output.HISTFileName = 'LArCollTimeMonitoringOutput.root'
    ConfigFlags.lock()

    ## Cell building
    from CaloRec.CaloRecoConfig import CaloRecoCfg
    cfg=CaloRecoCfg(ConfigFlags)

    # try collision time algo 
    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg.merge(LArCollisionTimeCfg(ConfigFlags))
    cfg.getEventAlgo("LArCollisionTimeAlg").cutIteration=False
    # add cluster collision time algo
    # cluster config is still missing
    #from LArClusterRec.LArClusterSwConfig import LArClusterSwConfig
    #cfg.merge(LArClusterSwConfig(ConfigFlags))
    from LArClusterRec.LArClusterCollisionTimeConfig import LArClusterCollisionTimeCfg
    cfg.merge(LArClusterCollisionTimeCfg(ConfigFlags))

    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    cfg.merge(BunchCrossingCondAlgCfg(ConfigFlags))

    import AthenaCommon.SystemOfUnits as Units
    collmon=LArCollisionTimeMonConfig(ConfigFlags)
    collmon.getEventAlgo("LArCollisionTimeMonAlg").timeDiffCut=5.0*Units.nanosecond
    collmon.getEventAlgo("LArCollisionTimeMonAlg").nCells=1
    collmon.getEventAlgo("LArCollisionTimeMonAlg").TrainFrontDistance=int(30*Units.nanosecond)
    collmon.getEventAlgo("LArClusterCollisionTimeMonAlg").nCells=0
    collmon.getEventAlgo("LArClusterCollisionTimeMonAlg").TrainFrontDistance=int(30*Units.nanosecond)
    cfg.merge(collmon) 

    ConfigFlags.dump()
    f=open("CollTimeMonMaker.pkl","w")
    cfg.store(f)
    f.close()
   
    
