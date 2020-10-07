
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#


def LArNoiseCorrelationMonConfigOld(inputFlags):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArNoiseCorrelationMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArNoiseCorrelationMonAlgCfg')
    LArNoiseCorrelationMonConfigCore(helper, LArNoiseCorrelationMonAlg,inputFlags)
    return helper.result()

def LArNoiseCorrelationMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link                                                                   
    # them to GenericMonitoringTools                                                                                                                                 
    
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArNoiseCorrelationMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    return LArNoiseCorrelationMonConfigCore(helper, CompFactory.LArNoiseCorrelationMonAlg,inputFlags)

def LArNoiseCorrelationMonConfigCore(helper, algoinstance,inputFlags):


    from LArMonitoring.GlobalVariables import lArDQGlobals

    larNoiseCorrelMonAlg = helper.addAlgorithm(algoinstance,'larNoiseCorrelMonAlg')

    #set custom list of FEBs to be monitored (if you want one): each FEB should be passed as a string of the form "BarrelAft01slot10"
    FEBs_from_DQ_run_350440 = ["endcapAft19slot12","endcapAft19slot09","endcapAft20slot09"]

    customFEBStoMonitor=FEBs_from_DQ_run_350440


    #correct custom FEBs for upper-lower cases or single-digit ft and slot numbers (e.g. 3 instead of 03)
    from ROOT import LArStrHelper
    larStrHelp=LArStrHelper()
    customFEBStoMonitor=[larStrHelp.fixFEBname(nm) for nm in customFEBStoMonitor]


    # adding BadChan masker private tool
    from AthenaConfiguration.ComponentFactory import isRun3Cfg


    if isRun3Cfg():
       if inputFlags.DQ.Environment == 'online':
          isOnline=True
    else:
       from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
       if athenaCommonFlags.isOnline:
          isOnline=True

    isOnline=False #needed later
    if isRun3Cfg() :
        if inputFlags.DQ.Environment == 'online':
            isOnline=True

        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        cfg=ComponentAccumulator()

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="BadLArRawChannelMask")
        larNoiseCorrelMonAlg.LArBadChannelMask=acc.popPrivateTools()
        cfg.merge(acc)
    else :
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.isOnline:
            isOnline=True

        from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
        theLArBadChannelsMasker=LArBadChannelMasker("BadLArRawChannelMask")
        theLArBadChannelsMasker.DoMasking=True
        theLArBadChannelsMasker.ProblemsToMask=["deadReadout","deadPhys","short","almostDead","highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"]
        larNoiseCorrelMonAlg.LArBadChannelMask=theLArBadChannelsMasker
        pass

    larNoiseCorrelMonAlg.IgnoreBadChannels=True
    larNoiseCorrelMonAlg.TriggerChain = "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY" #turn off for calibration run 
    larNoiseCorrelMonAlg.IsCalibrationRun = False
#        larNoiseCorrelMonAlg.LArDigitContainerKey=Gain #test with calibration  

    #deal with custom febs to monitor (if any)
    if len(customFEBStoMonitor)==0: 
        #we do not want to plot everything if online
        if isOnline:
            larNoiseCorrelMonAlg.PlotsOFF=True
            pass
        febsToMonitorBarrelA=lArDQGlobals.febsBarrelA
        febsToMonitorEndcapA=lArDQGlobals.febsEndcapA
        febsToMonitorBarrelC=lArDQGlobals.febsBarrelC
        febsToMonitorEndcapC=lArDQGlobals.febsEndcapC
        larNoiseCorrelMonAlg.PlotCustomFEBSset=False
        larNoiseCorrelMonAlg.FEBlist=febsToMonitorBarrelA+febsToMonitorBarrelC+febsToMonitorEndcapA+febsToMonitorEndcapC
    else:
        setCustomFEBS=set(customFEBStoMonitor)
        febsToMonitorBarrelA=list(setCustomFEBS.intersection(lArDQGlobals.febsBarrelA))
        febsToMonitorEndcapA=list(setCustomFEBS.intersection(lArDQGlobals.febsEndcapA))
        febsToMonitorBarrelC=list(setCustomFEBS.intersection(lArDQGlobals.febsBarrelC))
        febsToMonitorEndcapC=list(setCustomFEBS.intersection(lArDQGlobals.febsEndcapC))

        if len(febsToMonitorBarrelA)==0 and len(febsToMonitorEndcapA)==0 and len(febsToMonitorBarrelC)==0 and len(febsToMonitorEndcapC)==0:
            print("LArNoiseCorrelationMonAlg:WARNING. None of the following FEBs were recognised, no plot will be produced")
            print(customFEBStoMonitor)
            larNoiseCorrelMonAlg.PlotsOFF=True #lets protect ourselves against poor writing
            larNoiseCorrelMonAlg.PlotCustomFEBSset=False
            larNoiseCorrelMonAlg.FEBlist=lArDQGlobals.febsBarrelA+lArDQGlobals.febsEndcapA+lArDQGlobals.febsBarrelC+lArDQGlobals.febsEndcapC #to avoid having it empty, would it crash otherwise?
        else:
            #pass to algorithm
#            customFEBStoMonitor_forAlgo=[[lArDQGlobals.dictBarrelEndcap[i[0]],lArDQGlobals.dictSides[i[1]],int(i[2]),int(i[3])] for i in customFEBStoMonitor]
 #           larNoiseCorrelMonAlg.FEBsToMonitor=customFEBStoMonitor_forAlgo
            larNoiseCorrelMonAlg.PlotCustomFEBSset=True
            larNoiseCorrelMonAlg.FEBlist=febsToMonitorBarrelA+febsToMonitorBarrelC+febsToMonitorEndcapA+febsToMonitorEndcapC
            pass
        pass

    #prepare the monitoring group
    correlArray = helper.addArray([larNoiseCorrelMonAlg.FEBlist],larNoiseCorrelMonAlg,"NoiseCorrRAW",'/LAr/','run') 
#    correlArray = helper.addArray([lArDQGlobals.BarrelEndcap,lArDQGlobals.Sides,lArDQGlobals.Feedthrough_RangeMax,lArDQGlobals.Slot_RangeMax],larNoiseCorrelMonAlg,"NoiseCorrRAW",'/LAr/','run')

    hist_path='NoiseCorrelation/'
    
    average_plot_name="NoiseCorr_average_RAW"
    average_var_and_name="chanMeanX,chanMean;"+average_plot_name
    correlArray.defineHistogram(average_var_and_name,
                                title=average_plot_name,
                                type='TProfile',
                                path=hist_path+'BarrelA',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorBarrelA)

    correlArray.defineHistogram(average_var_and_name,
                                title=average_plot_name,
                                type='TProfile',
                                path=hist_path+'EndcapA',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorEndcapA)


    correlArray.defineHistogram(average_var_and_name,
                                title=average_plot_name,
                                type='TProfile',
                                path=hist_path+'BarrelC',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorBarrelC)

    correlArray.defineHistogram(average_var_and_name,
                                title=average_plot_name,
                                type='TProfile',
                                path=hist_path+'EndcapC',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorEndcapC)


    partialSum_plot_name="NoiseCorr_partialSum_RAW"
    partialSum_var_and_name="chanPartSumX,chanPartSumY;"+partialSum_plot_name

    correlArray.defineHistogram(partialSum_var_and_name,
                                title=partialSum_plot_name,
                                type='TH2F',
                                weight='chanPartSum',
                                path=hist_path+'BarrelA',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                ybins=lArDQGlobals.FEB_N_channels,ymin=lArDQGlobals.FEB_channels_Min,ymax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorBarrelA)
    
    correlArray.defineHistogram(partialSum_var_and_name,
                                title=partialSum_plot_name,
                                type='TH2F',
                                weight='chanPartSum',
                                path=hist_path+'EndcapA',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                ybins=lArDQGlobals.FEB_N_channels,ymin=lArDQGlobals.FEB_channels_Min,ymax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorEndcapA)


    correlArray.defineHistogram(partialSum_var_and_name,
                                title=partialSum_plot_name,
                                type='TH2F',
                                weight='chanPartSum',
                                path=hist_path+'BarrelC',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                ybins=lArDQGlobals.FEB_N_channels,ymin=lArDQGlobals.FEB_channels_Min,ymax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorBarrelC)
    
    correlArray.defineHistogram(partialSum_var_and_name,
                                title=partialSum_plot_name,
                                type='TH2F',
                                weight='chanPartSum',
                                path=hist_path+'EndcapC',
                                xbins=lArDQGlobals.FEB_N_channels,xmin=lArDQGlobals.FEB_channels_Min,xmax=lArDQGlobals.FEB_channels_Max,
                                ybins=lArDQGlobals.FEB_N_channels,ymin=lArDQGlobals.FEB_channels_Min,ymax=lArDQGlobals.FEB_channels_Max,
                                pattern=febsToMonitorEndcapC)

    print(correlArray)

    if isRun3Cfg():
        cfg.merge(helper.result())
        return cfg
    else:    
        return helper.result()
    

if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG #,WARNING
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArNoiseCorrMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()

   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
   cfg.merge(LArNoisyROSummaryCfg(ConfigFlags))

  # from LArMonitoring.LArNoiseCorrelationMonAlg import LArNoiseCorrelationMonConfig
   aff_acc = LArNoiseCorrelationMonConfig(ConfigFlags)

   cfg.merge(aff_acc)

   log.setLevel(DEBUG)
   ConfigFlags.dump()
   f=open("LArNoiseCorrelationMon.pkl","wb")
   cfg.store(f)
   f.close()

#   cfg.run(100,OutputLevel=DEBUG)
