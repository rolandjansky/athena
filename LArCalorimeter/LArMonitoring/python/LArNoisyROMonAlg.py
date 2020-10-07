#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArNoisyROMonConfig(inputFlags, inKey="", 
                              NoisyFEBDefStr="(>30 chan with Q>4000)", 
                              MNBTightFEBDefStr="",
                              MNBTight_PsVetoFEBDefStr="",
                              MNBLooseFEBDefStr=""):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArNoisyROMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    NoisyFEBDefStr="(>"+str(inputFlags.LAr.NoisyRO.BadChanPerFEB)+" chan with Q>"+str(inputFlags.LAr.NoisyRO.CellQuality)+")"
    MNBTightFEBDefStr="(>"+str(inputFlags.LAr.NoisyRO.MNBTightCut)+" chan with Q>"+str(inputFlags.LAr.NoisyRO.CellQuality)+")"
    MNBTight_PsVetoFEBDefStr="(>"+str(inputFlags.LAr.NoisyRO.MNBTight_PsVetoCut[0])+" chan with Q>"+str(inputFlags.LAr.NoisyRO.CellQuality)+") + PS veto (<"+str(inputFlags.LAr.NoisyRO.MNBTight_PsVetoCut[1])+" channels)"
    MNBLooseFEBDefStr="(>"+str(inputFlags.LAr.NoisyRO.MNBLooseCut)+" chan with Q>"+str(inputFlags.LAr.NoisyRO.CellQuality)+")"

    return LArNoisyROMonConfigCore(helper,CompFactory.LArNoisyROMonAlg, inputFlags, inKey, NoisyFEBDefStr, MNBTightFEBDefStr, MNBTight_PsVetoFEBDefStr, MNBLooseFEBDefStr)


def LArNoisyROMonConfigOld(inputFlags, inKey="", 
                              NoisyFEBDefStr="", 
                              MNBTightFEBDefStr="",
                              MNBTight_PsVetoFEBDefStr="",
                              MNBLooseFEBDefStr=""):

    from LArCellRec.LArNoisyROFlags import larNoisyROFlags
    NoisyFEBDefStr =  '(>'+str(larNoisyROFlags.BadChanPerFEB())+' chan with Q>'+str(larNoisyROFlags.CellQualityCut())+')'
    MNBTightFEBDefStr =  '(>'+str(larNoisyROFlags.MNBTightCut())+' chan with Q>'+str(larNoisyROFlags.CellQualityCut())+')'      
    MNBTight_PsVetoFEBDefStr =  '(>'+str(larNoisyROFlags.MNBTight_PsVetoCut()[0])+' chan with Q>'+str(larNoisyROFlags.CellQualityCut())+') + PS veto (<'+str(larNoisyROFlags.MNBTight_PsVetoCut()[1])+' channels)'
    MNBLooseFEBDefStr =  '(>'+str(larNoisyROFlags.MNBLooseCut())+' chan with Q>'+str(larNoisyROFlags.CellQualityCut())+')'
    from AthenaMonitoring import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArNoisyROMonAlg
    helper = AthMonitorCfgHelperOld(inputFlags,'LArNoisyROMonAlgOldCfg')

    LArNoisyROMonConfigCore(helper,LArNoisyROMonAlg, inputFlags, inKey, NoisyFEBDefStr, MNBTightFEBDefStr, MNBTight_PsVetoFEBDefStr, MNBLooseFEBDefStr)

    return helper.result()



def LArNoisyROMonConfigCore(helper,algoinstance,inputFlags, 
                              inKey="", 
                              NoisyFEBDefStr="(>30 chan with Q>4000)", 
                              MNBTightFEBDefStr="",
                              MNBTight_PsVetoFEBDefStr="",
                              MNBLooseFEBDefStr=""):

    # first configure known bad FEBs
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg():
       from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
       cfg=ComponentAccumulator()
       from LArBadChannelTool.LArBadFebsConfig import LArKnownBadFebCfg, LArKnownMNBFebCfg
       cfg.merge(LArKnownBadFebCfg(inputFlags))
       cfg.merge(LArKnownMNBFebCfg(inputFlags))

    larNoisyROMonAlg = helper.addAlgorithm(algoinstance,'larNoisyROMonAlg')

    NoisyROMonGroupName="LArNoisyROMonGroup"

    from LArMonitoring.GlobalVariables import lArDQGlobals

    larNoisyROMonAlg.NoisyROGroupName=NoisyROMonGroupName
    larNoisyROMonAlg.SubDetNames=lArDQGlobals.SubDet[0:2]
    larNoisyROMonAlg.PartitionNames=lArDQGlobals.Partitions[0:4]

    #FIXME: True only for testing
    larNoisyROMonAlg.storeLooseMNBFEBs=False
    if inKey != "":
       larNoisyROMonAlg.inputKey=inKey 

    # variable for testing on ESD
    try:
      LArNoisyROMonForceTrigger
    except NameError:
      LArNoisyROMonForceTrigger = False

    EFNoiseBurstTriggersList=[
         "EF_j165_u0uchad_LArNoiseBurst",
         "EF_j30_u0uchad_empty_LArNoiseBurst",
         "EF_j55_u0uchad_firstempty_LArNoiseBurst",
         "EF_j55_u0uchad_empty_LArNoiseBurst",
         "EF_xe45_LArNoiseBurst",
         "EF_xe55_LArNoiseBurst",
         "EF_xe60_LArNoiseBurst",
         "EF_j55_u0uchad_firstempty_LArNoiseBurstT",
         "EF_j100_u0uchad_LArNoiseBurstT",
         "EF_j165_u0uchad_LArNoiseBurstT",
         "EF_j130_u0uchad_LArNoiseBurstT",
         "EF_j35_u0uchad_empty_LArNoiseBurst",
         "EF_j35_u0uchad_firstempty_LArNoiseBurst",
         "EF_j80_u0uchad_LArNoiseBurstT"
    ]
    L1NoiseBurstTriggersList = [
         "L1_J75",
         "L1_J10_EMPTY",
         "L1_J30_FIRSTEMPTY",
         "L1_J30_EMPTY",
         "L1_XE40",
         "L1_XE50",
         "L1_XE50_BGRP7",
         "L1_XE70"
    ]
    doTrigger=False
    if isRun3Cfg():
      if inputFlags.Trigger.doHLT or LArNoisyROMonForceTrigger:
        doTrigger=True
    else:    
      if inputFlags.doHLTMon or LArNoisyROMonForceTrigger:
        doTrigger=True

    if doTrigger:
       larNoisyROMonAlg.doTrigger = True  
       larNoisyROMonAlg.EFNoiseBurstTriggers = EFNoiseBurstTriggersList
       larNoisyROMonAlg.L1NoiseBurstTriggers = L1NoiseBurstTriggersList
    else:   
       larNoisyROMonAlg.doTrigger = False  
       larNoisyROMonAlg.EFNoiseBurstTriggers = []
       larNoisyROMonAlg.L1NoiseBurstTriggers = []

    noisyROGroup = helper.addGroup(
        larNoisyROMonAlg,
        NoisyROMonGroupName,
        '/LAr/NoisyRO/'
    )


    larNoisyRO_hist_path='Summary/'

    # first trees
    noisyROGroup.defineTree('time,time_ns,algo;LArNoise',path=larNoisyRO_hist_path,
                            treedef='time/i:time_ns/i:algo/b')

    noisyROGroup.defineTree('candidate_MNB_time,candidate_MNB_time_ns,n_candidate_MNBTight_FEB,n_candidate_MNBTight_PsVeto_FEB,n_candidate_MNBLoose_FEB,v_candidate_MNBTightFEB,v_candidate_MNBTight_PsVetoFEB,v_candidate_MNBLooseFEB;CandidateMNBFebs',path=larNoisyRO_hist_path,
                            treedef='candidate_MNB_time/i:candidate_MNB_time_ns/i:n_candidate_MNBTight_FEB/i:n_candidate_MNBTight_PsVeto_FEB/i:n_candidate_MNBLoose_FEB/i:v_candidate_MNBTightFEB/vector<int>:v_candidate_MNBTight_PsVetoFEB/vector<int>:v_candidate_MNBLooseFEB/vector<int>')

    #then global histo
    noisyROGroup.defineHistogram('n_noisyFEBs;NoisyFEB',
                                  title='Number of noisy FEB '+ NoisyFEBDefStr + '  per event;# of noisy FEB',
                                  type='TH1I',
                                  path=larNoisyRO_hist_path,
                                  xbins=lArDQGlobals.noisyFEB_Bins,xmin=lArDQGlobals.noisyFEB_Min,xmax=lArDQGlobals.noisyFEB_Max)
    noisyROGroup.defineHistogram('LBN;LBN',type='TH1I',
                                 title='Event counter per LB', path=larNoisyRO_hist_path,
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

    for subdet in range(0,2): 
       hist_path='/LAr/NoisyRO/'+lArDQGlobals.SubDet[subdet]+'/'
       slot_low = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       slot_up  = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       slot_n = int(slot_up - slot_low)
       ft_low = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       ft_up  = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       ft_n = int(ft_up - ft_low)

       darray = helper.addArray([lArDQGlobals.Partitions[2*subdet:2*subdet+2]],larNoisyROMonAlg,lArDQGlobals.SubDet[subdet],topPath=hist_path)
       # Known bad FEBS
       darray.defineHistogram('slotBad,FTBad;KnownBadFEB', title='Known Bad FEBs {0} ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotMNB,FTMNB;MNBKnownFEB', title='Known MNB FEBs {0} ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       # 2D plots of noisy FEBs with various cuts
       darray.defineHistogram('slotNoisy,FTNoisy;NoisyFEBPerEvt', title='Yield of events with {0} FEB noisy -'+NoisyFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotTight,FTTight;MNBTightFEBPerEvt', title='Yield of events with {0} FEB noisy -'+MNBTightFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotTightCan,FTTightCan;CandidateMNBTightFEBPerEvt', title='Yield of events with {0} FEB noisy -'+MNBTightFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slot_PsVetoTight,FT_PsVetoTight;MNBTight_PsVetoFEBPerEvt', title='Yield of events with {0} FEB noisy -'+MNBTight_PsVetoFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slot_PsVetoTightCan,FT_PsVetoTightCan;CandidateMNBTight_PsVetoFEBPerEvt', title='Yield of events with {0} FEB noisy -'+MNBTight_PsVetoFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotLoose,FTLoose;MNBLooseFEBPerEvt', title='Yield of events with {0} FEB noisy -'+MNBLooseFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotLooseCan,FTLooseCan;CandidateMNBLooseFEBPerEvt', title='Yield of events with {0} FEB noisy -'+MNBLooseFEBDefStr+' (only vetoed events) ; Slot ; FT', 
                              type='TH2I', 
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       # 1D plots of noisy events of various type
       darray.defineHistogram('LBStd;NoisyEvent',type='TH1I',
                                 title='Yield of events flagged as RNB-Standard - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBSat;SaturatedNoisyEvent',type='TH1I',
                                 title='Yield of events flagged as RNB-Saturated - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBMNBTight;MNBTightEvent',type='TH1I',
                                 title='Yield of events flagged as MNB-Tight - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBMNBTight_PsVet;MNBTight_PsVetoEvent',type='TH1I',
                                 title='Yield of events flagged as MNB-Tight_PsVeto - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBMNBLoose;MNBLooseEvent',type='TH1I',
                                 title='Yield of events flagged as MNB-Loose - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       # 1D plots of events still noisy after EventVeto
       darray.defineHistogram('LBStd_Veto;NoisyEvent_TimeVeto',type='TH1I',
                                 title='Yield of events flagged as RNB-Standard not vetoed by time window - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBSat_Veto;SaturatedNoisyEvent_TimeVeto',type='TH1I',
                                 title='Yield of events flagged as RNB-Saturated not vetoed by time window - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBMNBTight_Veto;MNBTightEvent_TimeVeto',type='TH1I',
                                 title='Yield of events flagged as MNB-Tight not vetoed by time window - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBMNBTight_PsVeto_Veto;MNBTight_PsVetoEvent_TimeVeto',type='TH1I',
                                 title='Yield of events flagged as MNB-Tight_PsVeto not vetoed by time window - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       darray.defineHistogram('LBMNBLoose_Veto;MNBLooseEvent_TimeVeto',type='TH1I',
                                 title='Yield of events flagged as MNB-Loose not vetoed by time window - {0} ; Luminosity Block', 
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

       # Trigger histos
       if larNoisyROMonAlg.doTrigger:
          siz=len(larNoisyROMonAlg.EFNoiseBurstTriggers)
          darray.defineHistogram('Triggers;NoisyEventTrigger',type='TH1I',
                                 title='Trigger fired for RNB flagged events - {0} ; Special trigger fired', 
                                 xbins=siz+1,xmin=0.5,xmax=siz+1.5,
                                 xlabels=larNoisyROMonAlg.EFNoiseBurstTriggers.append("NONE"))

          l1siz=len(larNoisyROMonAlg.L1NoiseBurstTriggers)
          darray.defineHistogram('L1Triggers;NoisyEventL1Term',type='TH1I',
                                 title='L1 term fired for RNB flagged events - {0} ; Special trigger fired', 
                                 xbins=l1siz+1,xmin=0.5,xmax=l1siz+1.5,
                                 xlabels=larNoisyROMonAlg.L1NoiseBurstTriggers.append("NONE"))

    pass

    if isRun3Cfg():
       cfg.merge(helper.result())
       return cfg
    

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

    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/v1/'
    file = 'myESD.pool.root'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.DQ.useTrigger = True
    
    ConfigFlags.Output.HISTFileName = 'LArNoisyROMonitoringOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    # try NoisyRO algo 
    #first geometry
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
    cfg.merge(TileGMCfg(ConfigFlags))
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    cfg.merge(LArOnOffIdMappingCfg(ConfigFlags))
    # then NoisyROSummary creator
    from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
    noisyROSumm = LArNoisyROSummaryCfg(ConfigFlags)
    noisyROSumm.getEventAlgo("LArNoisyROAlg").OutputKey="LArNoisyROSummary_recomputed"
    cfg.merge(noisyROSumm)
    # then monitoring algo
    noisemon=LArNoisyROMonConfig(ConfigFlags, inKey="LArNoisyROSummary_recomputed")
    cfg.merge(noisemon) 

    ConfigFlags.dump()
    f=open("NoisyROMonMaker.pkl","w")
    cfg.store(f)
    f.close()
   
    cfg.run(20,OutputLevel=DEBUG) #use cfg.run() to run on all events
    #cfg.run() #use cfg.run() to run on all events
