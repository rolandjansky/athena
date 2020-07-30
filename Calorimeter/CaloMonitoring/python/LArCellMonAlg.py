#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArCellMonConfigOld(inputFlags):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from CaloMonitoring.CaloMonitoringConf import  LArCellMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArCellMonAlgOldCfg')
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics':
       isCosmics=True
    else:
       isCosmics=False

    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.DataSource() == 'data':
       isMC=False
    else:
       isMC=True

    if not isMC:
        from LumiBlockComps.LBDurationCondAlgDefault import LBDurationCondAlgDefault
        LBDurationCondAlgDefault()
        from LumiBlockComps.TrigLiveFractionCondAlgDefault import TrigLiveFractionCondAlgDefault
        TrigLiveFractionCondAlgDefault()
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        LuminosityCondAlgDefault()

    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg()

    LArCellMonConfigCore(helper, LArCellMonAlg,inputFlags,isCosmics, isMC)

    from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool
    helper.monSeq.LArCellMonAlg.ReadyFilterTool = GetAtlasReadyFilterTool()
    from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
    helper.monSeq.LArCellMonAlg.BadLBTool = GetLArBadLBFilterTool()

    return helper.result()

def LArCellMonConfig(inputFlags):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'LArCellMonConfig' )

    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCellMonAlgCfg')

    if not inputFlags.DQ.enableLumiAccess:
       mlog.warning('This algo needs Lumi access, returning empty config')
       from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
       cfg=ComponentAccumulator()
       cfg.merge(helper.result())
       return cfg

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg = LArGMCfg(inputFlags)
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(TileGMCfg(inputFlags))

    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    cfg.merge(DetDescrCnvSvcCfg(inputFlags))

    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg.merge(LArCollisionTimeCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(inputFlags))

    if inputFlags.Input.isMC is False:
      from LumiBlockComps.LuminosityCondAlgConfig import  LuminosityCondAlgCfg
      cfg.merge(LuminosityCondAlgCfg(inputFlags))
      from LumiBlockComps.LBDurationCondAlgConfig import  LBDurationCondAlgCfg
      cfg.merge(LBDurationCondAlgCfg(inputFlags))

    from AthenaConfiguration.ComponentFactory import CompFactory
    lArCellMonAlg=CompFactory.LArCellMonAlg

    algname='LArCellMonAlg'
    if inputFlags.Beam.Type == 'cosmics':
        algname=algname+'Cosmics'

    isCosmics = ( inputFlags.Beam.Type == 'cosmics' )
    LArCellMonConfigCore(helper, lArCellMonAlg,inputFlags, isCosmics, inputFlags.Input.isMC, algname)

    acc=helper.result()

    from AthenaMonitoring.AtlasReadyFilterConfig import AtlasReadyFilterCfg
    acc.getEventAlgo(algname).ReadyFilterTool = cfg.popToolsAndMerge(AtlasReadyFilterCfg(inputFlags))

    if not inputFlags.Input.isMC:
       from AthenaMonitoring.BadLBFilterToolConfig import LArBadLBFilterToolCfg
       acc.getEventAlgo(algname).BadLBTool=cfg.popToolsAndMerge(LArBadLBFilterToolCfg(inputFlags))

    cfg.merge(acc)

    return cfg


def LArCellMonConfigCore(helper, algclass, inputFlags, isCosmics=False, isMC=False, algname='LArCellMonAlg'):


    LArCellMonAlg = helper.addAlgorithm(algclass, algname)

    if isCosmics:
       badChanMaskProblems=["deadReadout","deadPhys","short","sporadicBurstNoise","highNoiseHG","highNoiseMG","highNoiseLG"]
    else: 
       badChanMaskProblems=["deadReadout","deadPhys","almostDead","short","sporadicBurstNoise","unstableNoiseLG","unstableNoiseMG","unstableNoiseHG","highNoiseHG","highNoiseMG","highNoiseLG"]

    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg():
       from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg

       acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=badChanMaskProblems,ToolName="BadLArRawChannelMask")
       LArCellMonAlg.LArBadChannelMask=acc.popPrivateTools()
       helper.resobj.merge(acc)
    else:
       from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
       theLArBadChannelsMasker=LArBadChannelMasker("BadLArRawChannelMask")
       theLArBadChannelsMasker.DoMasking=True
       theLArBadChannelsMasker.ProblemsToMask=badChanMaskProblems
       LArCellMonAlg.LArBadChannelMask=theLArBadChannelsMasker

    if not isCosmics and not isMC:
        LArCellMonAlg.useReadyFilterTool=True
    else:
        LArCellMonAlg.useReadyFilterTool=False

    if isMC:
        LArCellMonAlg.useBadLBTool=False
    else:
        LArCellMonAlg.useBadLBTool=True

# FIXME: to be added:    if isCosmics or rec.triggerStream()!='CosmicCalo':
    LArCellMonAlg.useBeamBackgroundRemoval = False
# FIXME: to be added:    else:
# FIXME: to be added:       LArCellMonAlg.useBeamBackgroundRemoval = True

    GroupName="LArCellMon"
    LArCellMonAlg.MonGroupName = GroupName
    LArCellMonAlg.MonGroupName_perJob = GroupName+"PerJob"
    LArCellMonAlg.MonGroupName_OccupancyEtaPhi = GroupName+"_OccupancyEtaPhi"
    LArCellMonAlg.MonGroupName_PercentageOccupancyEtaPhi = GroupName+"_PercentageOccupancyEtaPhi"
    LArCellMonAlg.MonGroupName_OccupancyEta = GroupName+"_OccupancyEta"
    LArCellMonAlg.MonGroupName_OccupancyPhi = GroupName+"_OccupancyPhi"
    LArCellMonAlg.MonGroupName_TotEnergyEtaPhi = GroupName+"_TotEnergyEtaPhi"
    LArCellMonAlg.MonGroupName_AvgQualityEtaPhi = GroupName+"_AvgQualityEtaPhi"
    LArCellMonAlg.MonGroupName_FractionOverQthEtaPhi = GroupName+"_FractionOverQthEtaPhi"
    LArCellMonAlg.MonGroupName_AvgTimeEtaPhi = GroupName+"_AvgTimeEtaPhi"
    LArCellMonAlg.MonGroupName_FractionPastTthEtaPhi = GroupName+"_FractionPastTthEtaPhi"


    LArCellMonAlg.EnableLumi = True
    
    LArCellMonAlg.Sporadic_switch = isCosmics
    

    LArCellMonAlg.Threshold_EM_S0S1=5000.
    LArCellMonAlg.Threshold_HECFCALEMS2S3=15000.

    from CaloMonitoring.LArCellBinning import lArCellBinningScheme

    binlabels=["TotalEvents","ATLAS Ready","with Good LAr LB","with No LAr Collision","with No Beam Background", "with No Trigger Filter","with No LArError"] 


    # Global Settings:

    # All 2D plot occupancy are activate only for express and cosmiccalo
#FIXME to be added     if (isCosmics or rec.triggerStream()=='CosmicCalo' or rec.triggerStream()=='express' or rec.triggerStream()=='Main' or rec.triggerStream()<=='ZeroBias') or (inputFlags.Common.isOnline):
#FIXME to be added        do2DOcc = True
#FIXME to be added    else:
#FIXME to be added        do2DOcc = False

    do2DOcc = True #TMP


    thresholdDict = {}
    thresholdDict["ThresholdType"]         = [ "noEth_CSCveto", "noEth_rndm_CSCveto", "medEth_CSCveto", "5Sigma_CSCveto", "hiEth_CSCveto", "hiEth_noVeto", "met_CSCveto"  ]
    thresholdDict["ThresholdDirection"]     = [ "none" , "none"      , "both"  , "both"  , "over" , "over"        , "over" ]
    thresholdDict["TriggersToInclude"]      = [ "all"  , "all"       , "all"   , "all"   , "all"  , "all"         , "met"  ]
    thresholdDict["TriggersToExclude"]      = [ "none" , "none"      , "none"  , "none"  , "none" , "none"        , "none" ]

    thresholdDict["DoPercentageOccupancy"]  = [ True  , False       , True    , False   , False  , False         , False  ]
    thresholdDict["DoEtaPhiOccupancy"]      = [ True  , False       , False   , do2DOcc,do2DOcc, do2DOcc      , False  ]
    thresholdDict["DoEtaOccupancy"]         = [ False  , False       , isCosmics   , False   , False  , False         , False  ]
    thresholdDict["DoPhiOccupancy"]         = [ False  , False       , True    , False   , False  , False         , False  ]

    thresholdDict["DoEtaPhiAverageEnergy"]  = [ False  , do2DOcc    , False   , do2DOcc  , False   , False        , False  ]
    thresholdDict["DoEtaPhiTotalEnergy"]    = [ False  , False       , False   , False   , False   , False        , False  ]
    thresholdDict["DoEtaPhiEnergyRMS"]      = [ False  , False       , False   , False   , False   , False        , False  ]
    thresholdDict["DoEtaPhiRMSvsDBnoise"]   = [ False  , False       , False   , False   , False   , False        , False  ] 

    thresholdDict["DoEtaPhiAverageQuality"] = [ False  , False       , False   , False   , False   , False        , False  ]
    thresholdDict["DoEtaPhiFractionOverQth"]= [ False  , False       , False   , False   , do2DOcc , do2DOcc      , False  ]
    thresholdDict["QualityFactorThreshold"] = [ 4000. ]*7

    thresholdDict["DoEtaPhiAverageTime"]    = [ False  , False       , False   , (not isCosmics)    , False   , False        , False  ]
    thresholdDict["DoEtaPhiFractionPastTth"]= [ False  , False       , False   , (not isCosmics)    , False   , False        , False  ]
    thresholdDict["TimeThreshold"]          = [ 4.      ]*7
    thresholdDict["ThresholdTitleTemplates"]= ["no Threshold",
                                               "no Threshold",
                                               "|E_{cell}| > %0.f#sigma_{noise}^{database}",
                                               "E_{cell} beyond %0.f#sigma_{noise}^{database}",
                                               "E_{cell} > %0.f MeV",
                                               "E_{cell} > %0.f MeV",
                                               "E_{cell} > %0.f MeV, MET trigger"]

    thresholdDict["DefaultThresholds"]     = [-4000000.,-4000000.    , 3.      , 5.      , 500.   , 500.          , 1000.  ]
    thresholdDict["ThresholdinSigNoise"]   = [ False   , False       , True    , True    , False  , False         , False  ]


    #check that thresholds are set properly before passing them to the algorithm
    nthr = len(thresholdDict["ThresholdType"])
    if not all(len(thrProp)==nthr for thrProp in thresholdDict.values()):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'LArCellMonLog' )
        mlog.error("LArCellMonAlg:: CONFIGURATION ERROR, threshold properties have different lenght. Please check CaloMonitoring/python/LArCellMonAlg.py")
        exit(1)
        pass


    LArCellMonAlg.ThresholdType = thresholdDict["ThresholdType"]

    LArCellMonAlg.ThresholdDirection     = thresholdDict["ThresholdDirection"]
    LArCellMonAlg.TriggersToInclude      = thresholdDict["TriggersToInclude"]
    LArCellMonAlg.TriggersToExclude      = thresholdDict["TriggersToExclude"]

    LArCellMonAlg.QualityFactorThreshold = thresholdDict["QualityFactorThreshold"]
    
    LArCellMonAlg.TimeThreshold          = thresholdDict["TimeThreshold"]

    LArCellMonAlg.DoBeamBackgroundRemoval=[( "CSCveto" in thr ) for thr in thresholdDict["ThresholdType"]]

    # Defaults: (Can be over ridden by layer specific values) ; plots will be made for all layers with DefaultThreshold != -9999999
    LArCellMonAlg.ThresholdTitleTemplates= thresholdDict["ThresholdTitleTemplates"]

    LArCellMonAlg.DefaultThresholds     = thresholdDict["DefaultThresholds"] 
    LArCellMonAlg.ThresholdinSigNoise   = thresholdDict["ThresholdinSigNoise"]

    LArCellMonAlg.ThresholdColumnType   = ["hiEth_CSCveto", "hiEth_noVeto"]
    # EMB
    LArCellMonAlg.EMBP_Thresh           = [ 800.  , 800.          ]
    LArCellMonAlg.EMB1_Thresh           = [ 600.  , 600.          ]
    LArCellMonAlg.EMB2_Thresh           = [ 600.  , 600.          ]
    LArCellMonAlg.EMB3_Thresh           = [ 600.  , 600.          ]
    # EMEC
    LArCellMonAlg.EMECP_Thresh          = [ 1200. , 1200.         ]
    LArCellMonAlg.EMEC1_Thresh          = [ 800.  , 800.          ]
    LArCellMonAlg.EMEC2_Thresh          = [ 800.  , 800.          ]
    LArCellMonAlg.EMEC3_Thresh          = [ 800.  , 800.          ]
    # HEC
    LArCellMonAlg.HEC0_Thresh           = [ 4000. , 4000.         ]
    LArCellMonAlg.HEC1_Thresh           = [ 4000. , 4000.         ]
    LArCellMonAlg.HEC2_Thresh           = [ 4000. , 4000.         ]
    LArCellMonAlg.HEC3_Thresh           = [ 4000. , 4000.         ]
    # FCAL
    LArCellMonAlg.FCAL1_Thresh          = [ 6000. , 6000.         ]
    LArCellMonAlg.FCAL2_Thresh          = [ 6000. , 6000.         ]
    LArCellMonAlg.FCAL3_Thresh          = [ 6000. , 6000.         ]
   
    #set the string arrays for the groups
    LArCellMonAlg.DoEtaPhiTotalOccupancyNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if ( thresholdDict["DoEtaPhiOccupancy"][i_thr] and not thresholdDict["DoPercentageOccupancy"][i_thr] ) 
                            or thresholdDict["DoEtaPhiAverageEnergy"][i_thr] 
                            or thresholdDict["DoEtaPhiAverageQuality"][i_thr] 
                            or thresholdDict["DoEtaPhiAverageTime"][i_thr]]

    LArCellMonAlg.DoEtaPhiPercentageOccupancyNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaPhiOccupancy"][i_thr] and thresholdDict["DoPercentageOccupancy"][i_thr]]

    LArCellMonAlg.DoEtaOccupancyNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaOccupancy"][i_thr]]
    LArCellMonAlg.DoPhiOccupancyNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoPhiOccupancy"][i_thr]]
    LArCellMonAlg.DoEtaPhiTotEnergyNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaPhiTotalEnergy"][i_thr] or thresholdDict["DoEtaPhiAverageEnergy"][i_thr]]
    LArCellMonAlg.DoEtaPhiAvgQualityNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaPhiAverageQuality"][i_thr]]
    LArCellMonAlg.DoEtaPhiFractionOverQthNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaPhiFractionOverQth"][i_thr]]
    LArCellMonAlg.DoEtaPhiAvgTimeNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaPhiAverageTime"][i_thr]]
    LArCellMonAlg.DoEtaPhiFractionPastTthNames=[thr for i_thr,thr in enumerate(thresholdDict["ThresholdType"]) if thresholdDict["DoEtaPhiFractionPastTth"][i_thr]]

   #---Groups and arrays


#FIXME: need option to switch between weightedEfficiency and weightedAverage (depends on when the average  is computed: if post-processing is done after merging, I don't want 'weightedMerge' options, otherwise I do. Not implemented for now)

   #---single Group for non threshold histograms
    cellMonGroup = helper.addGroup(
        LArCellMonAlg,
        GroupName,
        '/CaloMonitoring/LArCellMon_NoTrigSelNewAlg/'

    )


    #--define histograms
    # Summary
    summ_hist_path='Summary/'

    from CaloMonitoring.CaloMonAlgBase import CaloBaseHistConfig
    CaloBaseHistConfig(cellMonGroup,summ_hist_path,binlabels)  

    cellMonGroup.defineHistogram('trigType;nEvtsByTrigger',
                                 title='Total Events: bin 0, RNDM Trigger: 1, Calo Trigger: 2, MinBias Trigger: 3, MET Trigger: 4, Misc Trigger: 5, Events Selected for Noise Plots: 6 ',
                                 type='TH1I', path=summ_hist_path,
                                 xbins=lArCellBinningScheme.larCellSummary["xbins"][0], xmin=lArCellBinningScheme.larCellSummary["xbins"][1], xmax=lArCellBinningScheme.larCellSummary["xbins"][2],
                                 xlabels=lArCellBinningScheme.larCellSummary["xlabels"])

    #total events, saved if percentage occupancy is required
    cellMonGroup.defineHistogram('eventCounter;nEvtsPerThreshold',
                                 title='Events events passed Trigger and Background removal for each threshold (for normalisation)',
                                 type='TH1I', path=summ_hist_path,
                                 xbins=len(thresholdDict["ThresholdType"]),xmin=-0.5, xmax=(len(thresholdDict["ThresholdType"])-0.5),
                                 xlabels=thresholdDict["ThresholdType"])

    # En.
    LArCellMonAlg.doUnnormalized1DEnergy=True
    energy_hist_path='Energy/'
    if not isCosmics:
        for part in LArCellMonAlg.LayerNames:
            cellMonGroup.defineHistogram('cellEnergy_'+part+';CellEnergy_'+part,
                                         title='Cell Energy in ' +part+' with CSC veto;Cell Energy [MeV];Cell Events',
                                         type='TH1F', path=energy_hist_path,
                                         xbins=lArCellBinningScheme.logEnergyBins)
            pass
    elif isCosmics:   
        for part in LArCellMonAlg.LayerNames:
            cellMonGroup.defineHistogram('cellEnergy_'+part+';CellEnergy_'+part,
                                         title='Cell Energy in ' +part+' with CSC veto;Cell Energy [MeV];Cell Events',
                                         type='TH1F', path=energy_hist_path,
                                         xbins=lArCellBinningScheme.cosmicEneBinning[part][0], xmin=lArCellBinningScheme.cosmicEneBinning[part][1], xmax=lArCellBinningScheme.cosmicEneBinning[part][2])
            pass     
    else:   
        for part in LArCellMonAlg.LayerNames:
            cellMonGroup.defineHistogram('cellEnergy_'+part+';CellEnergy_'+part,
                                         title='Cell Energy in ' +part+' with CSC veto;Cell Energy [MeV];Cell Events',
                                         type='TH1F', path=energy_hist_path,
                                         xbins=lArCellBinningScheme.defaultEnergyBins[0],xmin=lArCellBinningScheme.defaultEnergyBins[1],xmax=lArCellBinningScheme.defaultEnergyBins[2])
            pass

    #En vs. Time
    energyvstime_hist_path='EnergyVsTime/'

#    eCutForTiming = [1000., 1000., 3000., 1500., 3500., 3500., 3500., 3500., 1500., 3000., 3000., 2000., 10000., 10000., 10000.]
    eCutForTiming = LArCellMonAlg.EcutForTiming #taken from the values hardcoded in the .h, only way to prevent mixing everything up with the enum in the code
    idx=0
    for part in LArCellMonAlg.LayerNames:
         cellMonGroup.defineHistogram('cellTime_'+part+',cellEnergy_'+part+';CellEnergyVsTime_'+part,
                           title='Cell Energy vs Cell Time in '+part+' with CSC veto;Cell Time [ns];Cell Energy [MeV]',
                           type='TH2F', path=energyvstime_hist_path,
                           xbins=lArCellBinningScheme.timescale, ybins=lArCellBinningScheme.energyscale)

         cellMonGroup.defineHistogram('cellTime_'+part+'_cut;CellEnergyVsTime_'+part+'_'+str(eCutForTiming[idx//2]),
                           title='Cell Energy vs Cell Time in '+part+' with CSC veto - Cell Time (E>'+str(eCutForTiming[idx//2])+' [MeV]);Cell Time [ns];Cell Energy [MeV]',
                           weight='cellEnergy_'+part+'_cut',
                           cutmask='enGreaterThanCut_'+part,
                           type='TH1F', path=energyvstime_hist_path,
                           xbins=lArCellBinningScheme.timescale)
         idx=idx+1
         pass


    cellMonGroupPerJob = helper.addGroup(
        LArCellMonAlg,
        LArCellMonAlg.MonGroupName_perJob,
        '/CaloMonitoring/LArCellMon_NoTrigSelNewAlg/'
    )

    LArCellMonAlg.doKnownBadChannelsVsEtaPhi = True
    LArCellMonAlg.doDatabaseNoiseVsEtaPhi = True

    for part in LArCellMonAlg.LayerNames:
        cellMonGroupPerJob.defineHistogram('celleta_'+part+',cellphi_'+part+';KnownBadChannelsVsEtaPhi_'+part,
                                           title="Map of known bad channels in (#eta,#phi) for "+part+";cell #eta;cell #phi",
                                           weight='badCellWord_'+part,
                                           cutmask='fillBadChannelPlot',
                                           type='TH2F', path="KnownBadChannels/", 
                                           xbins = lArCellBinningScheme.etaRange[part],
                                           ybins = lArCellBinningScheme.phiRange[part],
                                           merge='weightedAverage'
)
        
        cellMonGroupPerJob.defineHistogram('celleta_'+part+',cellphi_'+part+';DatabaseNoiseVsEtaPhi_'+part,
                                           title="Map of Noise Values from the Database vs (#eta,#phi) for "+part+";cell #eta;cell #phi",
                                           weight='cellnoisedb_'+part,
                                           cutmask='doDatabaseNoisePlot',
                                           type='TH2F', path="DatabaseNoise/", 
                                           xbins = lArCellBinningScheme.etaRange[part],
                                           ybins = lArCellBinningScheme.phiRange[part],
                                           merge='weightedAverage')
        
        cellMonGroupPerJob.defineHistogram('celleta_'+part+';NCellsActiveVsEta_'+part,
                                           title="No. of Active Cells in #eta for "+part+";cell #eta",
                                           cutmask='doCellsActiveEtaPlot',
                                           type='TH1F', path="ActiveCells/", 
                                           xbins = lArCellBinningScheme.etaRange[part],
                                           merge='weightedAverage')
        
        cellMonGroupPerJob.defineHistogram('cellphi_'+part+';NCellsActiveVsPhi_'+part,
                                           title="No. of Active Cells in #phi for "+part+";cell #phi",
                                           cutmask='doCellsActivePhiPlot',
                                           type='TH1F', path="ActiveCells/", 
                                           xbins = lArCellBinningScheme.phiRange[part],
                                           merge='weightedAverage')
        

        pass #part loop

    #--- group array for threshold dependent histograms
    allMonArray = helper.addArray([LArCellMonAlg.LayerNames, LArCellMonAlg.ThresholdType], LArCellMonAlg, "allMon", 
                                    "/CaloMonitoring/LArCellMon_NoTrigSelNewAlg/")


    #now histograms
    for part in LArCellMonAlg.LayerNames:
        allMonArray.defineHistogram('dummy', type='TH1F', xbins=1, xmin=0, xmax=1) # dummy to have at least 1 plot defined

        allMonArray.defineHistogram('celleta,cellphi;CellOccupancyVsEtaPhi',
                                                title='No. of events in (#eta,#phi) for '+part+';cell #eta;cell #phi',
                                                type='TH2F', path="2d_Occupancy/",
                                                cutmask='passThrCut',
                                                xbins = lArCellBinningScheme.etaRange[part],
                                                ybins = lArCellBinningScheme.phiRange[part],
                                                pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiTotalOccupancyNames])

        allMonArray.defineHistogram('passThrCut,celleta,cellphi;CellOccupancyFractionVsEtaPhi',
                                                          title='Fraction of events in (#eta,#phi) for '+part+';cell #eta;cell #phi',
                                                          type='TEfficiency', path="2d_Occupancy/",
                                                          xbins = lArCellBinningScheme.etaRange[part],
                                                          ybins = lArCellBinningScheme.phiRange[part],
                                                          pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiPercentageOccupancyNames])

        allMonArray.defineHistogram('celleta;CellOccupancyVsEta', #needs weightedAverage/weightedEff
                                                 title='No. of events in (#eta) for '+part+';cell #eta;',
                                                 type='TH1F', path="1d_Occupancy/",
                                                 cutmask='passThrCut',
                                                 xbins = lArCellBinningScheme.etaRange[part],
                                                 pattern=[(part, _) for _ in LArCellMonAlg.DoEtaOccupancyNames])

        allMonArray.defineHistogram('cellphi;CellOccupancyVsPhi',
                                             title='No. of events in (#phi) for '+part+';cell #phi;',
                                             type='TH1F', path="1d_Occupancy/",
                                             cutmask='passThrCut',
                                             xbins = lArCellBinningScheme.phiRange[part],
                                             pattern=[(part, _) for _ in LArCellMonAlg.DoPhiOccupancyNames])

        allMonArray.defineHistogram('celleta,cellphi;TotalEnergyVsEtaPhi',
                                                title="Total Cell Energy vs (#eta,#phi) in "+part+";cell #eta;cell #phi",
                                                weight='cellEnergy',
                                                type='TH2F', path="2d_TotalEnergy/", 
                                                cutmask='passThrCut',
                                                xbins = lArCellBinningScheme.etaRange[part],
                                                ybins = lArCellBinningScheme.phiRange[part],
                                                pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiTotEnergyNames])

        allMonArray.defineHistogram('celleta,cellphi;TotalQualityVsEtaPhi',
                                                title="Cell Quality vs (#eta,#phi) in "+part+";cell #eta;cell #phi",
                                                weight='cellQuality',
                                                type='TH2F', path="2d_AvgQuality/", #needs to be divided by '2D_occupancy' at post processing stage
                                                cutmask='passThrCut',
                                                xbins = lArCellBinningScheme.etaRange[part],
                                                ybins = lArCellBinningScheme.phiRange[part],
                                                pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiAvgQualityNames])

        allMonArray.defineHistogram('celleta,cellphi;TotalTimeVsEtaPhi',
                                              title="Cell Time vs (#eta,#phi) in "+part+";cell #eta;cell #phi",
                                              weight='cellTime',
                                              type='TH2F', path="2d_AvgTime/", #needs to be divided by '2D_occupancy' at post processing stage
                                                cutmask='passThrCut',
                                              xbins = lArCellBinningScheme.etaRange[part],
                                              ybins = lArCellBinningScheme.phiRange[part],
                                              pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiAvgTimeNames])

        allMonArray.defineHistogram('celleta,cellphi;fractionOverQthVsEtaPhi',
                                                      title="Fraction of Events in "+part+" for which the Quality Factor exceeds Threshold;cell #eta;cell #phi",
                                                      weight='isPoorQuality',
                                                      type='TH2F', path="2d_PoorQualityFraction/", # needs postprocessing 
                                                cutmask='passThrCut',
                                                      xbins = lArCellBinningScheme.etaRange[part],
                                                      ybins = lArCellBinningScheme.phiRange[part],
                                                      pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiFractionOverQthNames])

        allMonArray.defineHistogram('celleta,cellphi;fractionPastTthVsEtaPhi',
                                                      title="Fraction of Events in "+part+" for which the Time is further than Threshold;cell #eta;cell #phi",
                                                      weight='isLateTime',
                                                      type='TH2F', path="2d_FractionOutOfTime/", # needs postprocessing 
                                                cutmask='passThrCut',
                                                      xbins = lArCellBinningScheme.etaRange[part],
                                                      ybins = lArCellBinningScheme.phiRange[part],
                                                      pattern=[(part, _) for _ in LArCellMonAlg.DoEtaPhiFractionPastTthNames])

        pass #part loop for occupancy



    #-- TTree for sporadic
    if LArCellMonAlg.Sporadic_switch:
        sporadic_hist_path='/Sporadic_'+str(LArCellMonAlg.Threshold_EM_S0S1)+'*1e-3_'+str(LArCellMonAlg.Threshold_HECFCALEMS2S3)+'*1e-3GeV/'
        cellMonGroup.defineTree('sporadicCellE,sporadicCellTime,sporadicCellQuality,sporadicCellID,lumiBlock,adoptedEThreshold;SporadicNoisyCellsTree', path=sporadic_hist_path,
                                treedef='sporadicCellE/F:sporadicCellTime/F:sporadicCellQuality/s:sporadicCellID/l:lumiBlock/i')



if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Constants import WARNING
    from AthenaCommon.Logging import log
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    # to test tier0 workflow:
    #ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/data15_13TeV.00278748.physics_ZeroBias.merge.RAW._lb0384._SFO-ALL._0001.1']
    #ConfigFlags.Calo.Cell.doPileupOffsetBCIDCorr=True
    ConfigFlags.Output.HISTFileName = 'LArCellMonOutput.root'
    ConfigFlags.DQ.enableLumiAccess = True
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.Environment = 'tier0'
    ConfigFlags.lock()


    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg = MainServicesCfg(ConfigFlags)

    # in case of tier0 workflow:
    #from CaloRec.CaloRecoConfig import CaloRecoCfg
    #cfg.merge(CaloRecoCfg(ConfigFlags))

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge(LArCellMonConfig(ConfigFlags)) 

    f=open("LArCellMon.pkl","w")
    cfg.store(f)
    f.close()

    cfg.run(10,OutputLevel=WARNING) #use cfg.run() to run on all events
