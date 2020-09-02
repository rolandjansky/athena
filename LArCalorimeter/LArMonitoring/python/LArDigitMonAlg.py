
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
def LArDigitMonConfigOld(inputFlags):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArDigitMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArDigitMonAlgCfg')
    LArDigitMonConfigCore(helper, LArDigitMonAlg,inputFlags)
    return helper.result()

def LArDigitMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link                                                                   
    # them to GenericMonitoringTools                                                                                                                                 
    
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArDigitMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    return LArDigitMonConfigCore(helper, CompFactory.LArDigitMonAlg,inputFlags)

def LArDigitMonConfigCore(helper, algoinstance,inputFlags):


    from LArMonitoring.GlobalVariables import lArDQGlobals

    larDigitMonAlg = helper.addAlgorithm(algoinstance,'larDigitMonAlg')

    summaryGroupName="Summary"
    nslots=[]
    for i in range(0,len(lArDQGlobals.FEB_Slot)): 
       nslots.append(lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[i]][1])

    larDigitMonAlg.SummaryMonGroup=summaryGroupName
    larDigitMonAlg.LArDigitsSubDetNames=lArDQGlobals.SubDet
    larDigitMonAlg.LArDigitsPartitionNames=lArDQGlobals.Partitions
    larDigitMonAlg.LArDigitsNslots=nslots

    # adding BadChan masker private tool
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg() :
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        cfg=ComponentAccumulator()

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="BadLArRawChannelMask")
        larDigitMonAlg.LArBadChannelMask=acc.popPrivateTools()
        cfg.merge(acc)
    else :

        from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
        theLArBadChannelsMasker=LArBadChannelMasker("BadLArRawChannelMask")
        theLArBadChannelsMasker.DoMasking=True
        theLArBadChannelsMasker.ProblemsToMask=["deadReadout","deadPhys","short","almostDead","highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"]
        larDigitMonAlg.LArBadChannelMask=theLArBadChannelsMasker


    summaryGroup = helper.addGroup(
        larDigitMonAlg,
        summaryGroupName,
        '/LAr/DigitsNewAlg'
    )


    summary_hist_path=summaryGroupName+'/'
    
    summaryGroup.defineHistogram('sumbin,partition;Summary', 
                                  title='Gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.N_DigitsSummary,xmin=-0.5,xmax=lArDQGlobals.N_DigitsSummary-0.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  xlabels=lArDQGlobals.DigitsSummary,ylabels=lArDQGlobals.Partitions)
    summaryGroup.defineHistogram('gain,partition;summaryGain', 
                                  title='Gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight',
                                  xbins=lArDQGlobals.N_Gains,xmin=-0.5,xmax=lArDQGlobals.N_Gains-0.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  xlabels=lArDQGlobals.Gains,ylabels=lArDQGlobals.Partitions)
    
    # now individual partitions, because we need a different directories, will have only 2dim arrays (side)
    for subdet in range(0,lArDQGlobals.N_SubDet):
       array = helper.addArray([lArDQGlobals.Partitions[2*subdet:2*subdet+2]],larDigitMonAlg,lArDQGlobals.SubDet[subdet])
       hist_path='/LAr/DigitsNewAlg/'+lArDQGlobals.SubDet[subdet]+'/'
       slot_low = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       slot_up  = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       slot_n = int(slot_up - slot_low)
       ft_low = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       ft_up  = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       ft_n = int(ft_up - ft_low) 
       chan_n = lArDQGlobals.FEB_N_channels
       chan_low = lArDQGlobals.FEB_channels_Min
       chan_up = lArDQGlobals.FEB_channels_Max
       crates_n = lArDQGlobals.FEB_Crates[lArDQGlobals.Partitions[subdet*2]][1]
       crates_low = 0.5
       crates_up = lArDQGlobals.FEB_Crates[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       array.defineHistogram('Outslot,OutFT;tOutOfRange', 
                                  title='% chan/FEB/events with max out of ',
                                  type='TH2I',
                                  path=hist_path,
                                  weight='weight',
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('Outslot,OutFT,Outweight;OutOfRange', 
                                  title='% chan/FEB/events with max out of ',
                                  type='TProfile2D',
                                  path=hist_path,
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('Outcrate,Outchan;OutOfRangeChan', 
                                  title='% chan/FEB/events with max out of  ',
                                  type='TH2I',
                                  path=hist_path,
                                  weight='weight',
                                  xbins=crates_n,xmin=crates_low,xmax=crates_up,
                                  ybins=chan_n, ymin=chan_low, ymax=chan_up)

       array.defineHistogram('Saturslot,SaturFT;tSaturation', 
                                  title='% chan/FEB/events with max=4095 ADC ',
                                  type='TH2I',
                                  path=hist_path,
                                  weight='weight',
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('Saturslot,SaturFT,Saturweight;Saturation', 
                                  title='% chan/FEB/events with max=4095 ADC ',
                                  type='TProfile2D',
                                  path=hist_path,
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('Saturcrate,Saturchan;SaturationChan', 
                                  title='% chan/FEB/events with max=4095 ADC - Med/High Gain - All Stream',
                                  type='TH2I',
                                  path=hist_path,
                                  weight='Saturweight',
                                  xbins=crates_n,xmin=crates_low,xmax=crates_up,
                                  ybins=chan_n, ymin=chan_low, ymax=chan_up)

       array.defineHistogram('SaturLowslot,SaturLowFT;tSaturationLow', 
                                  title='% chan/FEB/events with max=4095 ADC ',
                                  type='TH2I',
                                  path=hist_path,
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('SaturLowslot,SaturLowFT,SaturLowweight;SaturationLow', 
                                  title='% chan/FEB/events with max=4095 ADC ',
                                  type='TProfile2D',
                                  path=hist_path,
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('SaturLowcrate,SaturLowchan;SaturationChanLow', 
                                  title='% chan/FEB/events with max=4095 ADC - Low Gain - All Stream',
                                  type='TH2I',
                                  path=hist_path,
                                  weight='SaturLowweight',
                                  xbins=crates_n,xmin=crates_low,xmax=crates_up,
                                  ybins=chan_n, ymin=chan_low, ymax=chan_up)

       array.defineHistogram('Nullslot,NullFT;tNullDigit', 
                                  title='% chan/FEB/events with min=0 ADC ',
                                  type='TH2I',
                                  path=hist_path,
                                  weight='weight',
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('Nullslot,NullFT,Nullweight;NullDigit', 
                                  title='% chan/FEB/events with min=0 ADC ',
                                  type='TProfile2D',
                                  path=hist_path,
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)
       array.defineHistogram('Nullcrate,Nullchan;NullDigitChan', 
                                  title='% chan/FEB/events with min=0 ADC - All Gain - All Stream',
                                  type='TH2I',
                                  path=hist_path,
                                  xbins=crates_n,xmin=crates_low,xmax=crates_up,
                                  ybins=chan_n, ymin=chan_low, ymax=chan_up)

       array.defineHistogram('slot,FT,MaxPos;AvePosMaxDig', 
                                  title='Average position of Max Digit ',
                                  type='TProfile2D',
                                  path=hist_path,
                                  xbins=int(slot_n),xmin=slot_low,xmax=slot_up,
                                  ybins=int(ft_n), ymin=ft_low, ymax=ft_up)

       array.defineHistogram('LBN,MaxPos;MaxVsTime', 
                                  title='Average Max Sample vs LumiBlock ',
                                  type='TProfile',
                                  path=hist_path,
                                  xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)
       array.defineHistogram('MaxPos,Energy;EnVsTime', 
                                  title='Energy vs max sample ',
                                  type='TH2F',
                                  path=hist_path,
                                  xbins=lArDQGlobals.Samples_Bins,xmin=lArDQGlobals.Samples_Min,xmax=lArDQGlobals.Samples_Max,
                                  ybins=lArDQGlobals.Energy_Bins, ymin=lArDQGlobals.Energy_Min, ymax=lArDQGlobals.Energy_Max)

       array.defineHistogram('l1trig,MaxPos;TriggerWord', 
                                  title='Position of max sample per L1 trigger word (8 bits) ',
                                  type='TProfile',
                                  path=hist_path,
                                  xbins=lArDQGlobals.L1Trig_Bins,xmin=lArDQGlobals.L1Trig_Min,xmax=lArDQGlobals.L1Trig_Max)

       array.defineHistogram('Sample,SignalNorm;SignShape', 
                                  title='Normalized Signal Shape ',
                                  type='TProfile',
                                  weight='weight',
                                  path=hist_path,
                                  xbins=lArDQGlobals.Samples_Bins,xmin=lArDQGlobals.Samples_Min,xmax=lArDQGlobals.Samples_Max)
    


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

   ConfigFlags.Output.HISTFileName = 'LArDigitsMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()

   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
   cfg.merge(LArNoisyROSummaryCfg(ConfigFlags))

  # from LArMonitoring.LArDigitMonAlg import LArDigitMonConfig
   aff_acc = LArDigitMonConfig(ConfigFlags)
   cfg.merge(aff_acc)

   ConfigFlags.dump()
   f=open("LArDigitMon.pkl","wb")
   cfg.store(f)
   f.close()

   #cfg.run(100,OutputLevel=WARNING)
