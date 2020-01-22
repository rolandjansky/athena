#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def LArCollisionTimeMonConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCollisionTimeMonCfg')

    # need to set up the following
    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg = LArCollisionTimeCfg(inputFlags)
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(inputFlags))
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(inputFlags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(TileGMCfg(inputFlags))

    from AthenaConfiguration.ComponentFactory import CompFactory
    larCollTimeMonAlg = helper.addAlgorithm(CompFactory.LArCollisionTimeMonAlg,'larCollTimeMonAlg')


    collTimeGroupName="LArCollisionTimeMonGroup"

    larCollTimeMonAlg.CollTimeGroupName=collTimeGroupName

    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    cfg.merge(BunchCrossingCondAlgCfg(inputFlags))

    collTimeGroup = helper.addGroup(
        larCollTimeMonAlg,
        collTimeGroupName,
        '/LAr/'
    )

    from LArMonitoring.GlobalVariables import lArDQGlobals

    larColTime_hist_path='CollisionTime/'
    
    collTimeGroup.defineHistogram('ecTimeDiff;LArCollTime', 
                                  title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV) per ns',
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min,xmax=lArDQGlobals.colTime_Max)
    
    collTimeGroup.defineHistogram('ecTimeAvg;LArCollAvgTime', 
                                  title='LArCollisionAverageTime - avg time of ECC and ECA;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV) per ns',
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.avgColTime_Bins,xmin=lArDQGlobals.avgColTime_Min,xmax=lArDQGlobals.avgColTime_Max)
    
    
    collTimeGroup.defineHistogram('ecTimeDiff;LArCollTimeLumiBlock', #this one is the same as LArCollTime, but it's reset once per LB (FUNCTIONALITY TO BE ADDED!)
                                  title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV) per ns',
                                  type='TH1F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min,xmax=lArDQGlobals.colTime_Max,
                                  opt='kLBNHistoryDepth=1')
    
    
    collTimeGroup.defineHistogram('lumi_block_timeWindow;LArCollTimeLumiBlockTimeCut',
                                  title='Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB;Luminosity Block Number;Number of events per LB',
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
                                  title='LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min,ymax=lArDQGlobals.colTime_Max)
    
    
    collTimeGroup.defineHistogram('bunch_crossing_id,ecTimeDiff;LArCollTime_vs_BCID',
                                  title='LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID;Bunch Crossing Number;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                  ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min,ymax=lArDQGlobals.colTime_Max)
    
    collTimeGroup.defineHistogram('lumi_block,ecTimeAvg;LArCollAvgTime_vs_LB',
                                  title='LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min,ymax=lArDQGlobals.avgColTime_Max)
    
    collTimeGroup.defineHistogram('bunch_crossing_id,ecTimeAvg;LArCollAvgTime_vs_BCID',
                                  title='LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of BCID;Bunch Crossing Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                  type='TH2F',
                                  path=larColTime_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                  ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min,ymax=lArDQGlobals.avgColTime_Max)
    
    #in train monitoring, only done offline
    if inputFlags.Common.isOnline:

        collTimeGroupName_intrain=collTimeGroupName+"_intrain"
        collTimeGroup_intrain = helper.collTime.addGroup( 
            larCollTimeMonAlg,
            collTimeGroupName_intrain,
            "/LAr/"
        )
        larCollTimeMonAlg.InTrain_CollTimeGroupName=collTimeGroupName_intrain #pass the group name to the algorithm, this way you let the algorithm now that this group has been defined (won't be filled otherwise)
        
        intrain_tit=" inside the train"
        intrain_name="_intrain"
        
        collTimeGroup_intrain.defineHistogram('ecTimeDiff;LArCollTime'+intrain_name, 
                                              title='LArCollisionTime - difference of avg time from ECC and ECA'+intrain_tit+';<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV) per ns',
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min,xmax=lArDQGlobals.colTime_Max)
        
        
        collTimeGroup_intrain.defineHistogram('ecTimeAvg;LArCollAvgTime'+intrain_name, 
                                              title='LArCollisionAverageTime - avg time of ECC and ECA'+intrain_tit+';(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV) per ns',
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.avgColTime_Bins,xmin=lArDQGlobals.avgColTime_Min,xmax=lArDQGlobals.avgColTime_Max)
        
        
        collTimeGroup_intrain.defineHistogram('ecTimeDiff;LArCollTimeLumiBlock'+intrain_name, #this one is the same as LArCollTime, but it's reset once per LB (FUNCTIONALITY TO BE ADDED!)
                                              title='LArCollisionTime - difference of avg time from ECC and ECA;<t_{C}> - <t_{A}> (ns)'+intrain_tit+';Number of events (weighted by energy/GeV) per ns',
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.colTime_Bins,xmin=lArDQGlobals.colTime_Min,xmax=lArDQGlobals.colTime_Max)
        
        
        collTimeGroup_intrain.defineHistogram('(lumi_block_timeWindow);LArCollTimeLumiBlockTimeCut'+intrain_name,
                                              title='Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB'+intrain_tit+';Luminosity Block Number;Number of events per LB',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
        
        
        collTimeGroup_intrain.defineHistogram('(lumi_block_singleBeam_timeWindow);LArCollTimeLumiBlockSingleBeamTimeCut'+intrain_name,
                                              title='Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB'+intrain_tit+';Luminosity Block Number;Number of events per LB',
                                              type='Th1F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
        
        
        collTimeGroup_intrain.defineHistogram('lumi_block,ecTimeDiff;LArCollTime_vs_LB'+intrain_name,
                                              title='LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block;Luminosity Block Number'+intrain_tit+';<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                              ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min,ymax=lArDQGlobals.colTime_Max)
        
        
        collTimeGroup_intrain.defineHistogram('bunch_crossing_id,ecTimeDiff;LArCollTime_vs_BCID'+intrain_name,
                                              title='LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID'+intrain_tit+';Bunch Crossing Number;<t_{C}> - <t_{A}> (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                              ybins=lArDQGlobals.colTime_Bins,ymin=lArDQGlobals.colTime_Min,ymax=lArDQGlobals.colTime_Max)
        
        collTimeGroup_intrain.defineHistogram('lumi_block,ecTimeAvg;LArCollAvgTime_vs_LB'+intrain_name,
                                              title='LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block'+intrain_tit+';Luminosity Block Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max,
                                              ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min,ymax=lArDQGlobals.avgColTime_Max)
        
        collTimeGroup_intrain.defineHistogram('bunch_crossing_id,ecTimeAvg;LArCollAvgTime_vs_BCID'+intrain_name,
                                              title='LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of BCID'+intrain_tit+';Bunch Crossing Number;(<t_{C}> + <t_{A}>) / 2 (ns);Number of events (weighted by energy/GeV)',
                                              type='TH2F',
                                              path=larColTime_hist_path,
                                              weight='weight',
                                              xbins=lArDQGlobals.BCID_Bins,xmin=lArDQGlobals.BCID_Min,xmax=lArDQGlobals.BCID_Max,
                                              ybins=lArDQGlobals.avgColTime_Bins,ymin=lArDQGlobals.avgColTime_Min,ymax=lArDQGlobals.avgColTime_Max)

        pass #end of if isOnline


    cfg.merge(helper.result())
    return cfg
    

if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    #from AthenaCommon.Constants import DEBUG
    #from AthenaConfiguration.TestDefaults import defaultTestFiles

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    #nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    #file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.RDO.e5458_s3126_d1437/'
    file = 'RDO.11426804._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    #ConfigFlags.Input.Files = defaultTestFiles.RDO
    #ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'LArCollisionTimeMonOutput.root'
    #ConfigFlags.DQ.enableLumiAccess = True
    #ConfigFlags.DQ.useTrigger = True
    #ConfigFlags.Beam.Type = 'collisions'
    ConfigFlags.Calo.Cell.doPileupOffsetBCIDCorr=False
    #ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    #from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    #cfg = MainServicesSerialCfg()
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    cfg=ComponentAccumulator()

    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    cfg.addEventAlgo(xAODMaker__EventInfoCnvAlg())
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    #from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    #cfg.merge(LArGMCfg(ConfigFlags))

    from CaloRec.CaloCellMakerConfig import CaloCellMakerCfg
    acc=CaloCellMakerCfg(ConfigFlags)
    acc.getPrimary().CaloCellsOutputName="AllCalo"
    cfg.merge(acc)

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(ConfigFlags))

    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg.merge(LArCollisionTimeCfg(ConfigFlags))

    collmon=LArCollisionTimeMonConfig(ConfigFlags)
    collmon.getEventAlgo("larCollTimeMonAlg").timeDiffCut=5.0
    collmon.getEventAlgo("larCollTimeMonAlg").nCells=1
    collmon.getEventAlgo("larCollTimeMonAlg").TrainFrontDistance=30
    cfg.merge(collmon) 
    f=open("CollTimeMon.pkl","wb")
    cfg.store(f)
    f.close()
   
    #cfg.run(10,OutputLevel=DEBUG) #use cfg.run() to run on all events
