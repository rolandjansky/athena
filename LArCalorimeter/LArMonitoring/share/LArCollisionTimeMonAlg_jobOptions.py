# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#AthMonitorCfgHelperOld must be imported in upper level JO    


#define the group names here, as you'll use them multiple times
collTimeGroupName="LArCollisionTimeMonGroup"


#here are the helpers
helper_collTime = AthMonitorCfgHelperOld(DQMonFlags, collTimeGroupName)

#then the algorithms
#from AthenaMonitoring.AthenaMonitoringConf import ExampleMonitorAlgorithm
from LArMonitoring.LArMonitoringConf import LArCollisionTimeMonAlg
larCollTimeMonAlg = helper_collTime.addAlgorithm(LArCollisionTimeMonAlg, "larCollTimeMonAlg")
larCollTimeMonAlg.CollTimeGroupName=collTimeGroupName
larCollTimeMonAlg.IsOnline = isOnline 

# BunchCrossing info
from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()


#now the groups
collTimeGroup = helper_collTime.addGroup( 
   larCollTimeMonAlg,
   collTimeGroupName,
   "/LAr/"
   )

#define the histograms
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


#and finally add all to topSequence
topSequence += helper_collTime.result()
