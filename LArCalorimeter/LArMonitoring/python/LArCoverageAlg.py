#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file LArCoverageAlg
@author M. Spalla
@date 2019-07-24
@brief Adapted from ExampleLArMonitorAlgorithm.py by C. D. Burton and P. Onyisi 
'''
def LArCoverageConfigOld(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArCoverageAlg

    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
    theLArRCBMasker=LArBadChannelMasker("BadLArRawChannelMask")
    theLArRCBMasker.DoMasking=True
    theLArRCBMasker.ProblemsToMask=["deadReadout","deadPhys","highNoiseHG","highNoiseMG","highNoiseLG"]

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArCoverageAlgOldCfg')
    LArCoverageConfigCore(helper,LArCoverageAlg,inputFlags)
    helper.monSeq.LArCoverageAlg.LArBadChannelMask=theLArRCBMasker

    return helper.result()

def LArCoverageConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCoverageCfgAlg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    LArCoverageConfigCore(helper, CompFactory.LArCoverageAlg,inputFlags)

    # adding BadChan masker private tool
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    cfg=ComponentAccumulator()
    from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg#,LArBadChannelCfg
    acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="BadLArRawChannelMask")
    helper.monSeq.LArCoverageAlg.LArBadChannelMask=acc.popPrivateTools()
    cfg.merge(acc)

    cfg.merge(helper.result())
    return cfg

def LArCoverageConfigCore(helper, algoinstance,inputFlags):

    larCoverageAlg = helper.addAlgorithm(algoinstance,'LArCoverageAlg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    #define the group names here, as you'll use them multiple times
    caloNoiseToolGroupName="CaloNoise"
    nLayers = 4
    badChannelsGroupName="BadChannels"
    coverageHWGroupName="Coverage"
    partitionsBarrel = [lArDQGlobals.SubDet[0]+side for side in lArDQGlobals.Sides]
    partitionsEndcap = [lArDQGlobals.SubDet[ip]+side for side in lArDQGlobals.Sides for ip in range(1,len(lArDQGlobals.SubDet))]

    # Edit properties of a algorithm
    larCoverageAlg.CaloNoiseToolGroupName=caloNoiseToolGroupName
    larCoverageAlg.Nsample = nLayers
    larCoverageAlg.BadChannelsGroupName=badChannelsGroupName
    larCoverageAlg.Sides = lArDQGlobals.Sides
    larCoverageAlg.CoverageHWGroupName=coverageHWGroupName
    larCoverageAlg.CoverageBarrelPartitions = partitionsBarrel
    larCoverageAlg.CoverageEndcapPartitions = partitionsEndcap
    larCoverageAlg.NphiBinsEMB1=lArDQGlobals.Cell_Variables["phiNbin"]["EMB"]["A"]["1"]
    larCoverageAlg.NphiBinsEMEC2=lArDQGlobals.Cell_Variables["phiNbin"]["EMEC"]["A"]["2"]
    larCoverageAlg.NphiBinsHEC=[lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["A"]["0"],lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["A"]["1"],lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["A"]["2"],lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["A"]["3"]]


    #Configure the CaloNoise
    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg(noisetype="electronicNoise")

    #-- caloNoise groups --
    caloNoiseToolArrayEM = helper.addArray([nLayers],larCoverageAlg,caloNoiseToolGroupName+"EM")
    caloNoiseToolArrayHEC = helper.addArray([nLayers],larCoverageAlg,caloNoiseToolGroupName+"HEC")
    caloNoiseToolArrayFCAL = helper.addArray([nLayers],larCoverageAlg,caloNoiseToolGroupName+"FCAL")
    caloNoiseToolGroup = helper.addGroup( #for the LB histogram
        larCoverageAlg,
        caloNoiseToolGroupName,
        '/LAr/',
        'run'
    )

    #-- badChannels groups --

    badChannelToolArrayBarrel = helper.addArray([lArDQGlobals.Sides],larCoverageAlg,badChannelsGroupName,'/LAr/','lb')
    badChannelToolArrayEndcap = helper.addArray([lArDQGlobals.Sides],larCoverageAlg,badChannelsGroupName,'/LAr/','lb')

    #-- CoverageHW groups -- 
    coverageHWToolArrayBarrel = helper.addArray([partitionsBarrel],larCoverageAlg,coverageHWGroupName,'/LAr/','run') 
    coverageHWToolArrayEndcap = helper.addArray([partitionsEndcap],larCoverageAlg,coverageHWGroupName,'/LAr/','run') 

    #-- Coverage groups (eta-phi plots) -- 
    coverageGroupEMBA0 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBA0','/LAr/','run') 
    coverageGroupEMBA1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBA1','/LAr/','run') 
    coverageGroupEMBA2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBA2','/LAr/','run') 
    coverageGroupEMBA3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBA3','/LAr/','run') 
    coverageGroupEMBC0 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBC0','/LAr/','run')
    coverageGroupEMBC1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBC1','/LAr/','run')
    coverageGroupEMBC2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBC2','/LAr/','run')
    coverageGroupEMBC3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMBC3','/LAr/','run')
    coverageGroupEMECA0 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECA0','/LAr/','run') 
    coverageGroupEMECA1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECA1','/LAr/','run') 
    coverageGroupEMECA2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECA2','/LAr/','run') 
    coverageGroupEMECA3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECA3','/LAr/','run')
    coverageGroupEMECC0 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECC0','/LAr/','run')
    coverageGroupEMECC1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECC1','/LAr/','run')
    coverageGroupEMECC2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECC2','/LAr/','run')
    coverageGroupEMECC3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'EMECC3','/LAr/','run')  
    coverageGroupHECA0 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECA0','/LAr/','run') 
    coverageGroupHECA1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECA1','/LAr/','run') 
    coverageGroupHECA2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECA2','/LAr/','run') 
    coverageGroupHECA3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECA3','/LAr/','run') 
    coverageGroupHECC0 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECC0','/LAr/','run')
    coverageGroupHECC1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECC1','/LAr/','run')
    coverageGroupHECC2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECC2','/LAr/','run')
    coverageGroupHECC3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'HECC3','/LAr/','run')
    coverageGroupFCalA1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'FCalA1','/LAr/','run')
    coverageGroupFCalA2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'FCalA2','/LAr/','run')
    coverageGroupFCalA3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'FCalA3','/LAr/','run')
    coverageGroupFCalC1 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'FCalC1','/LAr/','run')
    coverageGroupFCalC2 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'FCalC2','/LAr/','run')
    coverageGroupFCalC3 = helper.addGroup(larCoverageAlg,coverageHWGroupName+'FCalC3','/LAr/','run')


    ### Configure histograms

    # -- caloNoiseTool histograms --

    caloNoiseTool_path='CoverageNewAlg/CaloNoiseTool/'
    #LB histogram: need to know which LB the CaloNoiseTool histogram is about. Only add to caloNoiseToolGroup to avoid duplicates 
    caloNoiseToolGroup.defineHistogram('lb1_x;FirstLBnumber',
                                       type='TH1D',
                                       path=caloNoiseTool_path,
                                       title='CaloNoiseTool histogram are filled from this LB;;LB number',
                                       weight='lb1',
                                       xbins=1,xmin=-1,xmax=1)
    
    caloNoiseToolArrayEM.defineHistogram('etaChan,noise;CaloNoiseEM_Sampling',
                                         type='TProfile',
                                         path='LAr/'+caloNoiseTool_path,
                                         title='DBNoise in EM',
                                         xbins=lArDQGlobals.etaCaloNoise_Bins,xmax=lArDQGlobals.etaCaloNoise_Max,xmin=lArDQGlobals.etaCaloNoise_Min)

    caloNoiseToolArrayHEC.defineHistogram('etaChan,noise;CaloNoiseHEC_Sampling',
                                         type='TProfile',
                                         path='LAr/'+caloNoiseTool_path,
                                         title='DBNoise in HEC',
                                         xbins=lArDQGlobals.etaCaloNoise_Bins,xmax=lArDQGlobals.etaCaloNoise_Max,xmin=lArDQGlobals.etaCaloNoise_Min)
    caloNoiseToolArrayFCAL.defineHistogram('etaChan,noise;CaloNoiseFCAL_Sampling',
                                         type='TProfile',
                                         path='LAr/'+caloNoiseTool_path,
                                         title='DBNoise in FCAL',
                                         xbins=lArDQGlobals.etaCaloNoise_FcalBins,xmax=lArDQGlobals.etaCaloNoise_FcalMax,xmin=lArDQGlobals.etaCaloNoise_FcalMin)


    # -- badChannels histograms --
    badChannels_path='Coverage/BadChannels/'
    for side in  lArDQGlobals.Sides:
       badChannelToolArrayBarrel.defineHistogram('mon_FtSlot,single_channel;DBBadChannelsBarrel',
                                              type='TH2I',
                                              path=badChannels_path,
                                              title='Known Bad Channels - Barrel;Feedthrough(+Slot increasing);Channel',
                                              weight='flag',
                                              xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMB"+side],
                                              xmin=lArDQGlobals.Feedthrough_Slot_range["EMB"+side][0],
                                              xmax=lArDQGlobals.Feedthrough_Slot_range["EMB"+side][1],
                                              ybins=lArDQGlobals.FEB_N_channels,
                                              ymin=-0.5,
                                              ymax=lArDQGlobals.FEB_N_channels-0.5,
                                              xlabels=lArDQGlobals.Feedthrough_Slot_labels_Barrel)
       badChannelToolArrayEndcap.defineHistogram('mon_FtSlot,single_channel;DBBadChannelsEndcap',
                                              type='TH2I',
                                              path=badChannels_path,
                                              title='Known Bad Channels - Endcap '+side+';Feedthrough(+Slot increasing);Channel',
                                              weight='flag',
                                              xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMEC"+side],
                                              xmin=lArDQGlobals.Feedthrough_Slot_range["EMEC"+side][0],
                                              xmax=lArDQGlobals.Feedthrough_Slot_range["EMEC"+side][1],
                                              ybins=lArDQGlobals.FEB_N_channels,
                                              ymin=-0.5,
                                              ymax=lArDQGlobals.FEB_N_channels-0.5,
                                              xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)


    #--coverageHW histograms
    coverage_path='CoverageNewAlg/perPartition/'
    coverageHWToolArrayBarrel.defineHistogram('mon_FtSlot,mon_Channels;CoverageHW',
                                              type='TH2I',
                                              path=coverage_path,
                                              title='Coverage;Feedthrough(+Slot increasing);Channel',
                                              weight='mon_coverage',
                                              xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMBA"], #used also for EMBC
                                              xmin=lArDQGlobals.Feedthrough_Slot_range["EMBA"][0], #used also for EMBC
                                              xmax=lArDQGlobals.Feedthrough_Slot_range["EMBA"][1], #used also for EMBC
                                              ybins=lArDQGlobals.FEB_N_channels,
                                              ymin=-0.5,
                                              ymax=lArDQGlobals.FEB_N_channels-0.5,
                                              xlabels=lArDQGlobals.Feedthrough_Slot_labels_Barrel)

    coverageHWToolArrayEndcap.defineHistogram('mon_FtSlot,mon_Channels;CoverageHW',
                                              type='TH2I',
                                              path=coverage_path,
                                              title='Coverage;Feedthrough(+Slot increasing);Channel',
                                              weight='mon_coverage',
                                              xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMECA"], #used also for HEC, FCAL and C side
                                              xmin=lArDQGlobals.Feedthrough_Slot_range["EMECA"][0], #used also for HEC, FCAL and C side
                                              xmax=lArDQGlobals.Feedthrough_Slot_range["EMECA"][1], #used also for HEC, FCAL and C side
                                              ybins=lArDQGlobals.FEB_N_channels,
                                              ymin=-0.5,
                                              ymax=lArDQGlobals.FEB_N_channels-0.5,
                                              xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)


    coverageGroupEMBA0.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMBA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 0 - EMBA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["0"])

    coverageGroupEMBA1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMBA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - EMBA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["1"])

    coverageGroupEMBA2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMBA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - EMBA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["2"])

    coverageGroupEMBA3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMBA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - EMBA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["3"])


    coverageGroupEMBC0.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMBC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 0 - EMBC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["0"])

    coverageGroupEMBC1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMBC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - EMBC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["1"])

    coverageGroupEMBC2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMBC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - EMBC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["2"])

    coverageGroupEMBC3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMBC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - EMBC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["3"])
                                                                                       
    coverageGroupEMECA0.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 0 - EMECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["0"])

    coverageGroupEMECA1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - EMECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["1"])

    coverageGroupEMECA2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - EMECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["2"])

    coverageGroupEMECA3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - EMECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["3"])

    coverageGroupEMECC0.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 0 - EMECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["0"])

    coverageGroupEMECC1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - EMECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["1"])

    coverageGroupEMECC2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - EMECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["2"])

    coverageGroupEMECC3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - EMECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["3"])


    coverageGroupHECA0.defineHistogram('mon_Eta,mon_Phi;CoverSampling0HECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 0 - HECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["0"])

    coverageGroupHECA1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1HECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - HECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["1"])

    coverageGroupHECA2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2HECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - HECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["2"])

    coverageGroupHECA3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3HECA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - HECA;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["3"])

    coverageGroupHECC0.defineHistogram('mon_Eta,mon_Phi;CoverSampling0HECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 0 - HECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["0"])

    coverageGroupHECC1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1HECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - HECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["1"])

    coverageGroupHECC2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2HECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - HECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["2"])

    coverageGroupHECC3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3HECC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - HECC;#eta;#phi',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["3"])
    
    coverageGroupFCalA1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1FCalA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - FCalA;cell number (from outside to inside);sector number (#phi)',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["1"])

    coverageGroupFCalA2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2FCalA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - FCalA;cell number (from outside to inside);sector number (#phi)',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["2"])

    coverageGroupFCalA3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3FCalA',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - FCalA;cell number (from outside to inside);sector number (#phi)',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["3"])

    coverageGroupFCalC1.defineHistogram('mon_Eta,mon_Phi;CoverSampling1FCalC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 1 - FCalC;cell number (from outside to inside);sector number (#phi)',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["C"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["C"]["1"])

    coverageGroupFCalC2.defineHistogram('mon_Eta,mon_Phi;CoverSampling2FCalC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 2 - FCalC;cell number (from outside to inside);sector number (#phi)',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["C"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["C"]["2"])

    coverageGroupFCalC3.defineHistogram('mon_Eta,mon_Phi;CoverSampling3FCalC',
                                          type='TH2I',
                                          path=coverage_path,
                                          title='Coverage - Sampling 3 - FCalC;cell number (from outside to inside);sector number (#phi)',
                                          weight='mon_coverage',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["C"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["C"]["3"])




if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
    createLArMonConfigFlags()

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.DQ.useTrigger = False

    ConfigFlags.Output.HISTFileName = 'LArCoverageOutput.root'
    ConfigFlags.lock()

   ## Cell building
    from CaloRec.CaloRecoConfig import CaloRecoCfg
    cfg=CaloRecoCfg(ConfigFlags)

    larCoverageAcc = LArCoverageConfig(ConfigFlags)    
    cfg.merge(larCoverageAcc)

    ConfigFlags.dump()
    f=open("CoverageMaker.pkl","wb")
    cfg.store(f)
    f.close()

