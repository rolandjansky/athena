#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def LArRODMonConfig(inputFlags, cellDebug=False, dspDebug=False):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArRODMonCfg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    from LArMonitoring.LArMonitoringConf import LArRODMonAlg
    larRODMonAlg = helper.addAlgorithm(LArRODMonAlg,'larRODMonAlg')

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

    # adding BadChan masker private tool
    from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
    acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys","almostDead","short","sporadicBurstNoise"],ToolName="BadLArChannelMask")
    larRODMonAlg.LArBadChannelMask=acc.popPrivateTools()
    helper.resobj.merge(acc)

    Group = helper.addGroup(
        larRODMonAlg,
        GroupName,
        '/LAr/DSPMonitoring'
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
                                  labels=lArDQGlobals.Partitions+lArDQGlobals.Gains)

    Group.defineHistogram('partition,gain;Summary_Q', 
                                  title='Summary of errors on Quality per partition and per gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight_q',
                                  xbins=lArDQGlobals.N_Partitions, xmin=-0.5, xmax=lArDQGlobals.N_Partitions-0.5,
                                  ybins=lArDQGlobals.N_Gains,ymin=-0.5,ymax=lArDQGlobals.N_Gains-0.5,
                                  labels=lArDQGlobals.Partitions+lArDQGlobals.Gains)

    Group.defineHistogram('partition,gain;Summary_T', 
                                  title='Summary of errors on Time per partition and per gain',
                                  type='TH2F',
                                  path=summary_hist_path,
                                  weight='weight_t',
                                  xbins=lArDQGlobals.N_Partitions, xmin=-0.5, xmax=lArDQGlobals.N_Partitions-0.5,
                                  ybins=lArDQGlobals.N_Gains,ymin=-0.5,ymax=lArDQGlobals.N_Gains-0.5,
                                  labels=lArDQGlobals.Partitions+lArDQGlobals.Gains)

    Group.defineHistogram('Ediff;E_all', 
                                  title='E_{offline} - E_{online} for all partitions;E_{offline} - E_{online} (MeV)',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPEnergy_Bins, xmin=lArDQGlobals.DSPEnergy_Min, xmax=lArDQGlobals.DSPEnergy_Max)
    Group.defineHistogram('Tdiff;T_all', 
                                  title='T_{offline} - T_{online} for all partitions;T_{offline} - T_{online} (ps)',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPTime_Bins, xmin=lArDQGlobals.DSPTime_Min, xmax=lArDQGlobals.DSPTime_Max)
    Group.defineHistogram('Qdiff;Q_all', 
                                  title='Q_{offline} - Q_{online} / #sqrt{Q_{offline}} for all partitions;Q_{offline} - Q_{online}  / sqrt{Q_{offline}} (ps)',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPQuality_Bins, xmin=lArDQGlobals.DSPQuality_Min, xmax=lArDQGlobals.DSPQuality_Max)

    #Infos histos (vs. LB)
    info_hist_path='Infos/'
    cut = "#delta ADC>"+str(larRODMonAlg.ADCthreshold)+" and |t_{offline}| < "+str(larRODMonAlg.peakTimeCut)+" ns"
    Group.defineHistogram('LBN,partition:EErrorsPerLB',
                                  title='Nb of errors in E per LB - ' +cut+':Luminosity Block:Partition',
                                  type='TH2I',
                                  weight='numE',
                                  path=info_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  # and labels for Y ?
          )
    Group.defineHistogram('LBN,partition:TErrorsPerLB',
                                  title='Nb of errors in T per LB - ' +cut+':Luminosity Block:Partition',
                                  type='TH2I',
                                  weight='numT',
                                  path=info_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  # and labels for Y ?
          )
    Group.defineHistogram('LBN,partition:QErrorsPerLB',
                                  title='Nb of errors in Q per LB - ' +cut+':Luminosity Block:Partition',
                                  type='TH2I',
                                  weight='numQ',
                                  path=info_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  # and labels for Y ?
          )

    #DQMD histos
    dqmd_hist_path='DQMD/'
    darray = helper.addArray([lArDQGlobals.Partitions+['all']],larRODMonAlg,"RODMon")
    darray.defineHistogram('Ediff,Erange;DE_ranges', title='E_{online} - E_{offline} for all ranges : E_{offline} - E_{online} (MeV) : Energy range ',
                           type='TH2F', path=dqmd_hist_path,
                           xbins=lArDQGlobals.DSP1Energy_Bins, xmin=lArDQGlobals.DSP1Energy_Min, xmax=lArDQGlobals.DSP1Energy_Max,
                           ybins=lArDQGlobals.DSPRanges_Bins, ymin=lArDQGlobals.DSPRanges_Min, ymax=lArDQGlobals.DSPRanges_Max,
                           #labels=?+lArDQGlobals.DSPRanges # how to put labels only on Y-axis ?
          )

    Group.defineHistogram('Ediff,Erange;E_ranges_all', title='E_{online} - E_{offline} for all ranges : E_{offline} - E_{online} (MeV) : Energy range ',
                           type='TH2F', path=dqmd_hist_path,
                           xbins=lArDQGlobals.DSP1Energy_Bins, xmin=lArDQGlobals.DSP1Energy_Min, xmax=lArDQGlobals.DSP1Energy_Max,
                           ybins=lArDQGlobals.DSPRanges_Bins, ymin=lArDQGlobals.DSPRanges_Min, ymax=lArDQGlobals.DSPRanges_Max,
                           #labels=?+lArDQGlobals.DSPRanges # how to put labels only on Y-axis ?
          )
    #per partition, currently in one dir only
    part_hist_path='perPartition/'
    darray.defineHistogram('Ediff;DE', title='E_{offline} - E_{online}:E_{offline} - E_{online}',
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPEnergy_Bins, xmin=lArDQGlobals.DSPEnergy_Min, xmax=lArDQGlobals.DSPEnergy_Max)
    darray.defineHistogram('Tdiff;DT', title='T_{offline} - T_{online}:T_{offline} - T_{online}',
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPTime_Bins, xmin=lArDQGlobals.DSPTime_Min, xmax=lArDQGlobals.DSPTime_Max)
    darray.defineHistogram('Qdiff;DG', title='Q_{offline} - Q_{online}:Q_{offline} - Q_{online} / #sqrt{Q_{offline}}' ,
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPTime_Bins, xmin=lArDQGlobals.DSPTime_Min, xmax=lArDQGlobals.DSPTime_Max)

    darray.defineHistogram('slot,FT;Out_E_FT_vs_SLOT',title='# of cells with E_{offline} - E_{online} > numerical precision : Slot : Feedthrough',
                           type='TH2I', path=part_hist_path,
                           weight='weight_e',
                           xbins=lArDQGlobals.FEB_Slot["EMECA"][1], xmin=lArDQGlobals.FEB_Slot["EMECA"][0]-0.5, xmax=lArDQGlobals.FEB_Slot["EMECA"][1]+0.5,
                           ybins=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+1, ymin=lArDQGlobals.FEB_Feedthrough["EMBA"][0]-0.5, ymax=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+0.5)

    darray.defineHistogram('slot,FT;Out_T_FT_vs_SLOT',title='# of cells with E_{offline} - E_{online} > numerical precision : Slot : Feedthrough',
                           type='TH2I', path=part_hist_path,
                           weight='weight_t',
                           xbins=lArDQGlobals.FEB_Slot["EMECA"][1], xmin=lArDQGlobals.FEB_Slot["EMECA"][0]-0.5, xmax=lArDQGlobals.FEB_Slot["EMECA"][1]+0.5,
                           ybins=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+1, ymin=lArDQGlobals.FEB_Feedthrough["EMBA"][0]-0.5, ymax=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+0.5)

    darray.defineHistogram('slot,FT;Out_Q_FT_vs_SLOT',title='# of cells with E_{offline} - E_{online} > numerical precision : Slot : Feedthrough',
                           type='TH2I', path=part_hist_path,
                           weight='weight_q',
                           xbins=lArDQGlobals.FEB_Slot["EMECA"][1], xmin=lArDQGlobals.FEB_Slot["EMECA"][0]-0.5, xmax=lArDQGlobals.FEB_Slot["EMECA"][1]+0.5,
                           ybins=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+1, ymin=lArDQGlobals.FEB_Feedthrough["EMBA"][0]-0.5, ymax=lArDQGlobals.FEB_Feedthrough["EMBA"][1]+0.5)

    darray.defineHistogram('Eoff,Eon;Eon_VS_Eoff', title='E_{online} VS E_{offline}:E_{offline} (MeV):E_{online} (MeV)',
                           type='TH2F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPEonEoff_Bins, xmin=-lArDQGlobals.DSPEonEoff_Max, xmax=lArDQGlobals.DSPEonEoff_Max,
                           ybins=lArDQGlobals.DSPEonEoff_Bins, ymin=-lArDQGlobals.DSPEonEoff_Max, ymax=lArDQGlobals.DSPEonEoff_Max)

    darray.defineHistogram('Toff,Ton;Ton_VS_Toff', title='T_{online} VS T_{offline}:T_{offline} (ps):T_{online} (ps)',
                           type='TH2F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPTonToff_Bins, xmin=-lArDQGlobals.DSPTonToff_Max, xmax=lArDQGlobals.DSPTonToff_Max,
                           ybins=lArDQGlobals.DSPTonToff_Bins, ymin=-lArDQGlobals.DSPTonToff_Max, ymax=lArDQGlobals.DSPTonToff_Max)

    darray.defineHistogram('Qoff,Qon;Qon_VS_Qoff', title='Q_{online} VS Q_{offline}:Q_{offline} :Q_{online} ',
                           type='TH2F', path=part_hist_path,
                           xbins=lArDQGlobals.DSPQonQoff_Bins, xmin=-lArDQGlobals.DSPQonQoff_Max, xmax=lArDQGlobals.DSPQonQoff_Max,
                           ybins=lArDQGlobals.DSPQonQoff_Bins, ymin=-lArDQGlobals.DSPQonQoff_Max, ymax=lArDQGlobals.DSPQonQoff_Max)

    darray.defineHistogram('Sweetc;Sweet_cells', title='Number of sweet Cells in LAr:Sweet cells per feb',
                           type='TH1F', path=part_hist_path,
                           xbins=lArDQGlobals.FEB_N_channels, xmin=lArDQGlobals.FEB_channels_Min, xmax=lArDQGlobals.FEB_channels_Max)

    return helper.result()

    

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
