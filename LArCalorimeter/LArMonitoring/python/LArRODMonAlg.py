#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArRODMonConfigOld(inputFlags,cellDebug=False, dspDebug=False):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import  LArRODMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArRODMonALgOldCfg')
    LArRODMonConfigCore(helper, LArRODMonAlg,inputFlags,cellDebug, dspDebug)

    return helper.result()

def LArRODMonConfig(inputFlags,cellDebug=False, dspDebug=False):

    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArRODMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    
    
    LArRODMonConfigCore(helper, CompFactory.LArRODMonAlg,inputFlags,cellDebug, dspDebug)

    return helper.result()


def LArRODMonConfigCore(helper, algoinstance,inputFlags, cellDebug=False, dspDebug=False):

    larRODMonAlg = helper.addAlgorithm(algoinstance,'larRODMonAlg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    
    GroupName="RODMon"
    nslots=[]
    for i in range(0,len(lArDQGlobals.FEB_Slot)): 
       nslots.append(lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[i]][1])

    larRODMonAlg.MonGroup=GroupName
    larRODMonAlg.LArRODSubDetNames=lArDQGlobals.SubDet
    larRODMonAlg.LArRODPartitionNames=lArDQGlobals.Partitions
    larRODMonAlg.LArRODNslots=nslots

    #Adding 1MeV on request of Alexis (truncating difference) (May 2016):

    larRODMonAlg.PrecisionERange0 = 2 # MeV (Precision on E is on Eoff - Eonl)
    larRODMonAlg.PrecisionERange1 = 9
    larRODMonAlg.PrecisionERange2 = 65
    larRODMonAlg.PrecisionERange3 = 513
    larRODMonAlg.PrecisionERangeMax = 8192

    larRODMonAlg.ADCthreshold = 0
    larRODMonAlg.peakTimeCut = 5.
    larRODMonAlg.SkipNullQT=True 

    # for detailed debugging
    if cellDebug:
       larRODMonAlg.DoCellsDump=True

    if dspDebug:
       larRODMonAlg.DoDspTestDump=True

    #from AthenaCommon.Constants import VERBOSE
    #larRODMonAlg.OutputLevel=VERBOSE
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg() :
        # adding BadChan masker private tool
        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys","almostDead","short","sporadicBurstNoise"],ToolName="BadLArChannelMask")
        larRODMonAlg.LArBadChannelMask=acc.popPrivateTools()
        helper.resobj.merge(acc)
    else :
        from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
        theLArBadChannelsMasker=LArBadChannelMasker("BadLArRawChannelMask")
        theLArBadChannelsMasker.DoMasking=True
        theLArBadChannelsMasker.ProblemsToMask=["deadReadout","deadPhys","short","almostDead","highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"]
        larRODMonAlg.LArBadChannelMask=theLArBadChannelsMasker
    

    Group = helper.addGroup(
        larRODMonAlg,
        GroupName,
        '/LAr/DSPMonitoringNewAlg'
    )


    #Summary histos
    summary_hist_path='Summary/'
    
    Group.defineHistogram('partition,gain;Summary_E', 
                                  title='Summary of errors on Energy per partition and per gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight_e',
                                  xbins=lArDQGlobals.N_Partitions, xmin=-0.5, xmax=lArDQGlobals.N_Partitions-0.5,
                                  ybins=lArDQGlobals.N_Gains,ymin=-0.5,ymax=lArDQGlobals.N_Gains-0.5,
                                  xlabels=lArDQGlobals.Partitions,ylabels=lArDQGlobals.Gains)

    Group.defineHistogram('partition,gain;Summary_Q', 
                                  title='Summary of errors on Quality per partition and per gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight_q',
                                  xbins=lArDQGlobals.N_Partitions, xmin=-0.5, xmax=lArDQGlobals.N_Partitions-0.5,
                                  ybins=lArDQGlobals.N_Gains,ymin=-0.5,ymax=lArDQGlobals.N_Gains-0.5,
                                  xlabels=lArDQGlobals.Partitions,ylabels=lArDQGlobals.Gains)

    Group.defineHistogram('partition,gain;Summary_T', 
                                  title='Summary of errors on Time per partition and per gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight_t',
                                  xbins=lArDQGlobals.N_Partitions, xmin=-0.5, xmax=lArDQGlobals.N_Partitions-0.5,
                                  ybins=lArDQGlobals.N_Gains,ymin=-0.5,ymax=lArDQGlobals.N_Gains-0.5,
                                  xlabels=lArDQGlobals.Partitions,ylabels=lArDQGlobals.Gains)

    Group.defineHistogram('Ediff;E_all', 
                                  title='E_offline - E_online for all partitions;E_offline - E_online (MeV)',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPEnergy_Bins, xmin=lArDQGlobals.DSPEnergy_Min, xmax=lArDQGlobals.DSPEnergy_Max)
    Group.defineHistogram('Tdiff;T_all', 
                                  title='T_offline - T_online for all partitions;T_offline - T_online (ps)',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPTime_Bins, xmin=lArDQGlobals.DSPTime_Min, xmax=lArDQGlobals.DSPTime_Max)
    Group.defineHistogram('Qdiff;Q_all', 
                                  title='Q_offline - Q_online / #sqrt{Q_offline} for all partitions;Q_offline - Q_online  / sqrt{Q_offline} (ps)',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPQuality_Bins, xmin=lArDQGlobals.DSPQuality_Min, xmax=lArDQGlobals.DSPQuality_Max)

    #Infos histos (vs. LB)
    info_hist_path='Infos/'
    cut = "#delta ADC>"+str(larRODMonAlg.ADCthreshold)+" and |t_offline| < "+str(larRODMonAlg.peakTimeCut)+" ns"
    Group.defineHistogram('LBN,partitionI;EErrorsPerLB',
                                  title='Nb of errors in E per LB - ' +cut+':Luminosity Block:Partition',
                                  type='TH2I',
                                  weight='numE',
                                  path=info_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  ylabels = lArDQGlobals.Partitions
          )
    Group.defineHistogram('LBN,partitionI;TErrorsPerLB',
                                  title='Nb of errors in T per LB - ' +cut+':Luminosity Block:Partition',
                                  type='TH2I',
                                  weight='numT',
                                  path=info_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  ylabels = lArDQGlobals.Partitions
          )
    Group.defineHistogram('LBN,partitionI;QErrorsPerLB',
                                  title='Nb of errors in Q per LB - ' +cut+':Luminosity Block:Partition',
                                  type='TH2I',
                                  weight='numQ',
                                  path=info_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  ylabels = lArDQGlobals.Partitions
          )

    #DQMD histos
    dqmd_hist_path='/LAr/DSPMonitoringNewAlg/DQMD/'
    darray = helper.addArray([lArDQGlobals.Partitions],larRODMonAlg,"RODMon")
    darray.defineHistogram('Ediff,Erange;DE_ranges', title='EOnline - E_offline for all ranges : E_offline - E_online (MeV) : Energy range',#'E_online - E_offline for all ranges : E_offline - E_online (MeV) : Energy range',
                           type='TH2F', path=dqmd_hist_path,
                           xbins=lArDQGlobals.DSP1Energy_Bins, xmin=lArDQGlobals.DSP1Energy_Min, xmax=lArDQGlobals.DSP1Energy_Max,
                           ybins=lArDQGlobals.DSPRanges_Bins, ymin=lArDQGlobals.DSPRanges_Min, ymax=lArDQGlobals.DSPRanges_Max,
                           ylabels=lArDQGlobals.DSPRanges 
          )
    Group.defineHistogram('Ediff,Erange;E_ranges_all', title='E_online - E_offline for all ranges : E_offline - E_online (MeV) : Energy range',
                           type='TH2F', path=dqmd_hist_path,
                           xbins=lArDQGlobals.DSP1Energy_Bins, xmin=lArDQGlobals.DSP1Energy_Min, xmax=lArDQGlobals.DSP1Energy_Max,
                           ybins=lArDQGlobals.DSPRanges_Bins, ymin=lArDQGlobals.DSPRanges_Min, ymax=lArDQGlobals.DSPRanges_Max,
                           ylabels=lArDQGlobals.DSPRanges
          )


    #per partition, currently in one dir only
    part_hist_path='/LAr/DSPMonitoringNewAlg/perPartition/'
    darray.defineHistogram('Ediff;DE', title='E_offline - E_online:E_offline - E_online',
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPEnergy_Bins, xmin=lArDQGlobals.DSPEnergy_Min, xmax=lArDQGlobals.DSPEnergy_Max)
    darray.defineHistogram('Tdiff;DT', title='T_offline - T_online:T_offline - T_online',
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPTime_Bins, xmin=lArDQGlobals.DSPTime_Min, xmax=lArDQGlobals.DSPTime_Max)
    darray.defineHistogram('Qdiff;DG', title='Q_offline - Q_online:Q_offline - Q_online / #sqrtQ_offline' ,
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPTime_Bins, xmin=lArDQGlobals.DSPTime_Min, xmax=lArDQGlobals.DSPTime_Max)

    darray.defineHistogram('slot,FT;Out_E_FT_vs_SLOT',title='# of cells with E_offline - E_online > numerical precision : Slot : Feedthrough',
                           type='TH2I', path=part_hist_path,
                           weight='weight_e',
                           xbins=lArDQGlobals.FEB_Slot["EMECA"][1], xmin=lArDQGlobals.FEB_Slot["EMECA"][0]-0.5, xmax=lArDQGlobals.FEB_Slot["EMECA"][1]+0.5,
                           ybins=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+1, ymin=lArDQGlobals.FEB_Feedthrough["EMBA"][0]-0.5, ymax=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+0.5)

    darray.defineHistogram('slot,FT;Out_T_FT_vs_SLOT',title='# of cells with E_offline - E_online > numerical precision : Slot : Feedthrough',
                           type='TH2I', path=part_hist_path,
                           weight='weight_t',
                           xbins=lArDQGlobals.FEB_Slot["EMECA"][1], xmin=lArDQGlobals.FEB_Slot["EMECA"][0]-0.5, xmax=lArDQGlobals.FEB_Slot["EMECA"][1]+0.5,
                           ybins=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+1, ymin=lArDQGlobals.FEB_Feedthrough["EMBA"][0]-0.5, ymax=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+0.5)

    darray.defineHistogram('slot,FT;Out_Q_FT_vs_SLOT',title='# of cells with E_offline - E_online > numerical precision : Slot : Feedthrough',
                           type='TH2I', path=part_hist_path,
                           weight='weight_q',
                           xbins=lArDQGlobals.FEB_Slot["EMECA"][1], xmin=lArDQGlobals.FEB_Slot["EMECA"][0]-0.5, xmax=lArDQGlobals.FEB_Slot["EMECA"][1]+0.5,
                           ybins=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+1, ymin=lArDQGlobals.FEB_Feedthrough["EMBA"][0]-0.5, ymax=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+0.5)

    darray.defineHistogram('Eoff,Eon;Eon_VS_Eoff', title='E_online VS E_offline:E_offline (MeV):E_online (MeV)',
                           type='TH2F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPEonEoff_Bins, xmin=-lArDQGlobals.DSPEonEoff_Max, xmax=lArDQGlobals.DSPEonEoff_Max,
                           ybins=lArDQGlobals.DSPEonEoff_Bins, ymin=-lArDQGlobals.DSPEonEoff_Max, ymax=lArDQGlobals.DSPEonEoff_Max)

    darray.defineHistogram('Toff,Ton;Ton_VS_Toff', title='T_online VS T_offline:T_offline (ps):T_online (ps)',
                           type='TH2F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPTonToff_Bins, xmin=-lArDQGlobals.DSPTonToff_Max, xmax=lArDQGlobals.DSPTonToff_Max,
                           ybins=lArDQGlobals.DSPTonToff_Bins, ymin=-lArDQGlobals.DSPTonToff_Max, ymax=lArDQGlobals.DSPTonToff_Max)

    darray.defineHistogram('Qoff,Qon;Qon_VS_Qoff', title='Q_online VS Q_offline:Q_offline :Q_online ',
                           type='TH2F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPQonQoff_Bins, xmin=-lArDQGlobals.DSPQonQoff_Max, xmax=lArDQGlobals.DSPQonQoff_Max,
                           ybins=lArDQGlobals.DSPQonQoff_Bins, ymin=-lArDQGlobals.DSPQonQoff_Max, ymax=lArDQGlobals.DSPQonQoff_Max)

    darray.defineHistogram('Sweetc;Sweet_cells', title='Number of sweet Cells in LAr:Sweet cells per feb',
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.FEB_N_channels, xmin=lArDQGlobals.FEB_channels_Min, xmax=lArDQGlobals.FEB_channels_Max)

    

if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArRODMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()


   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   #from CaloD3PDMaker.CaloD3PDConfig import CaloD3PDCfg,CaloD3PDAlg
   #cfg.merge(CaloD3PDCfg(ConfigFlags, filename=ConfigFlags.Output.HISTFileName, streamname='CombinedMonitoring'))

   aff_acc = LArRODMonConfig(ConfigFlags)
   cfg.merge(aff_acc)

   cfg.printConfig()

   ConfigFlags.dump()
   f=open("LArRODMon.pkl","w")
   cfg.store(f)
   f.close()

   #cfg.run(100,OutputLevel=WARNING)
