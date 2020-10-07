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

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArCoverageAlgOldCfg')
    LArCoverageConfigCore(helper,LArCoverageAlg,inputFlags)

    return helper.result()

def LArCoverageConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCoverageCfgAlg')


    from AthenaConfiguration.ComponentFactory import CompFactory
    LArCoverageConfigCore(helper, CompFactory.LArCoverageAlg,inputFlags)

    rv = ComponentAccumulator()
    rv.merge(helper.result())
    return rv

def LArCoverageConfigCore(helper, algoinstance,inputFlags):

    larCoverageAlg = helper.addAlgorithm(algoinstance,'LArCoverageAlg')

    # adding BadChan masker private tool
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg():
       from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg#,LArBadChannelCfg
       acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="BadLArRawChannelMask")
       larCoverageAlg.LArBadChannelMask=acc.popPrivateTools()
       helper.resobj.merge(acc)
    else:   
       from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
       theLArRCBMasker=LArBadChannelMasker("BadLArRawChannelMask")
       theLArRCBMasker.DoMasking=True
       theLArRCBMasker.ProblemsToMask=["deadReadout","deadPhys","highNoiseHG","highNoiseMG","highNoiseLG"]
       larCoverageAlg.LArBadChannelMask=theLArRCBMasker


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

    badChannelToolArrayBarrel = helper.addArray([lArDQGlobals.Sides],larCoverageAlg,badChannelsGroupName+"Barrel",'/LAr/','run')
    badChannelToolArrayEndcap = helper.addArray([lArDQGlobals.Sides],larCoverageAlg,badChannelsGroupName+"EndCap",'/LAr/','run')

    #-- CoverageHW groups --
    availErrCode = larCoverageAlg.AvailableErrorCodes

    #-- Coverage groups (eta-phi plots) -- 
    coverageToolArrayEMBA = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"EMBA",'/LAr/','run')
    coverageToolArrayEMECA = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"EMECA",'/LAr/','run')
    coverageToolArrayHECA = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"HECA",'/LAr/','run')
    coverageToolArrayFCalA = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"FCalA",'/LAr/','run')
    coverageToolArrayEMBC = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"EMBC",'/LAr/','run')
    coverageToolArrayEMECC = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"EMECC",'/LAr/','run')
    coverageToolArrayHECC = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"HECC",'/LAr/','run')
    coverageToolArrayFCalC = helper.addArray([availErrCode],larCoverageAlg,coverageHWGroupName+"FCalC",'/LAr/','run')

    ### Configure histograms

    coveragePath='Coverage/'

    # -- caloNoiseTool histograms --

    caloNoiseTool_path=coveragePath+'CaloNoiseTool/'
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
    badChannels_path=coveragePath+'BadChannels/'
    badChannelToolArrayBarrel.defineHistogram('mon_FtSlot,single_channel;DBBadChannelsBarrel',
                                              type='TH2I',
                                              path=badChannels_path,
                                              title='Known Bad Channels - Barrel {0};Feedthrough(+Slot increasing);Channel',
                                              weight='flag',
                                              xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMBA"], #bins from A side also used for C, they're the same
                                              xmin=lArDQGlobals.Feedthrough_Slot_range["EMBA"][0],
                                              xmax=lArDQGlobals.Feedthrough_Slot_range["EMBA"][1],
                                              ybins=lArDQGlobals.FEB_N_channels,
                                              ymin=-0.5,
                                              ymax=lArDQGlobals.FEB_N_channels-0.5,
                                              xlabels=lArDQGlobals.Feedthrough_Slot_labels_Barrel,
                                              merge='weightedAverage')
    badChannelToolArrayEndcap.defineHistogram('mon_FtSlot,single_channel;DBBadChannelsEndcap',
                                              type='TH2I',
                                              path=badChannels_path,
                                              title='Known Bad Channels - Endcap {0};Feedthrough(+Slot increasing);Channel',
                                              weight='flag',
                                              xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMECA"], #bins from A side also used for C, they're the same
                                              xmin=lArDQGlobals.Feedthrough_Slot_range["EMECA"][0],
                                              xmax=lArDQGlobals.Feedthrough_Slot_range["EMECA"][1],
                                              ybins=lArDQGlobals.FEB_N_channels,
                                              ymin=-0.5,
                                              ymax=lArDQGlobals.FEB_N_channels-0.5,
                                              xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap,
                                              merge='weightedAverage')


    #--coverageHW histograms
    coverage_path=coveragePath+'perPartition/'
    coverageToolArrayEMBA.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_EMBA_statusCode',
                                            type='TH2I',
                                            path=coverage_path,
                                            title='Coverage - EMBA - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                            xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMBA"], 
                                            xmin=lArDQGlobals.Feedthrough_Slot_range["EMBA"][0], 
                                            xmax=lArDQGlobals.Feedthrough_Slot_range["EMBA"][1], 
                                            ybins=lArDQGlobals.FEB_N_channels,
                                            ymin=-0.5,
                                            ymax=lArDQGlobals.FEB_N_channels-0.5,
                                            xlabels=lArDQGlobals.Feedthrough_Slot_labels_Barrel)

    coverageToolArrayEMBC.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_EMBC_statusCode',
                                            type='TH2I',
                                            path=coverage_path,
                                            title='Coverage - EMBC - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                            xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMBC"], 
                                            xmin=lArDQGlobals.Feedthrough_Slot_range["EMBC"][0], 
                                            xmax=lArDQGlobals.Feedthrough_Slot_range["EMBC"][1], 
                                            ybins=lArDQGlobals.FEB_N_channels,
                                            ymin=-0.5,
                                            ymax=lArDQGlobals.FEB_N_channels-0.5,
                                            xlabels=lArDQGlobals.Feedthrough_Slot_labels_Barrel)
    
    coverageToolArrayEMECA.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_EMECA_statusCode',
                                           type='TH2I',
                                           path=coverage_path,
                                           title='Coverage - EMECA - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                           xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMECA"], 
                                           xmin=lArDQGlobals.Feedthrough_Slot_range["EMECA"][0], 
                                           xmax=lArDQGlobals.Feedthrough_Slot_range["EMECA"][1],
                                           ybins=lArDQGlobals.FEB_N_channels,
                                           ymin=-0.5,
                                           ymax=lArDQGlobals.FEB_N_channels-0.5,
                                           xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)

    coverageToolArrayEMECC.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_EMECA_statusCode',
                                           type='TH2I',
                                           path=coverage_path,
                                           title='Coverage - EMECC - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                           xbins=lArDQGlobals.Feedthrough_Slot_Nbins["EMECC"], 
                                           xmin=lArDQGlobals.Feedthrough_Slot_range["EMECC"][0], 
                                           xmax=lArDQGlobals.Feedthrough_Slot_range["EMECC"][1],
                                           ybins=lArDQGlobals.FEB_N_channels,
                                           ymin=-0.5,
                                           ymax=lArDQGlobals.FEB_N_channels-0.5,
                                           xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)

    coverageToolArrayHECA.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_HECA_statusCode',
                                           type='TH2I',
                                           path=coverage_path,
                                           title='Coverage - HECA - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                           xbins=lArDQGlobals.Feedthrough_Slot_Nbins["HECA"], 
                                           xmin=lArDQGlobals.Feedthrough_Slot_range["HECA"][0], 
                                           xmax=lArDQGlobals.Feedthrough_Slot_range["HECA"][1],
                                           ybins=lArDQGlobals.FEB_N_channels,
                                           ymin=-0.5,
                                           ymax=lArDQGlobals.FEB_N_channels-0.5,
                                           xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)

    coverageToolArrayHECC.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_HECA_statusCode',
                                           type='TH2I',
                                           path=coverage_path,
                                           title='Coverage - HECC - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                           xbins=lArDQGlobals.Feedthrough_Slot_Nbins["HECC"], 
                                           xmin=lArDQGlobals.Feedthrough_Slot_range["HECC"][0], 
                                           xmax=lArDQGlobals.Feedthrough_Slot_range["HECC"][1],
                                           ybins=lArDQGlobals.FEB_N_channels,
                                           ymin=-0.5,
                                           ymax=lArDQGlobals.FEB_N_channels-0.5,
                                           xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)

    coverageToolArrayFCalA.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_FCalA_statusCode',
                                           type='TH2I',
                                           path=coverage_path,
                                           title='Coverage - FCalA - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                           xbins=lArDQGlobals.Feedthrough_Slot_Nbins["FCalA"], 
                                           xmin=lArDQGlobals.Feedthrough_Slot_range["FCalA"][0], 
                                           xmax=lArDQGlobals.Feedthrough_Slot_range["FCalA"][1],
                                           ybins=lArDQGlobals.FEB_N_channels,
                                           ymin=-0.5,
                                           ymax=lArDQGlobals.FEB_N_channels-0.5,
                                           xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)

    coverageToolArrayFCalC.defineHistogram('mon_ChanFtSlot,mon_Channels;CoverageHW_FCalC_statusCode',
                                           type='TH2I',
                                           path=coverage_path,
                                           title='Coverage - FCalC - statusCode={0};Feedthrough(+Slot increasing);Channel',
                                           xbins=lArDQGlobals.Feedthrough_Slot_Nbins["FCalC"], 
                                           xmin=lArDQGlobals.Feedthrough_Slot_range["FCalC"][0], 
                                           xmax=lArDQGlobals.Feedthrough_Slot_range["FCalC"][1],
                                           ybins=lArDQGlobals.FEB_N_channels,
                                           ymin=-0.5,
                                           ymax=lArDQGlobals.FEB_N_channels-0.5,
                                           xlabels=lArDQGlobals.Feedthrough_Slot_labels_Endcap)


    coverageToolArrayEMBA.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMBA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 0 - EMBA;#eta;#phi',
                                          cutmask='isSampl0',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["0"])

    coverageToolArrayEMBA.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMBA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 1 - EMBA;#eta;#phi',
                                          cutmask='isSampl1',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["1"])

    coverageToolArrayEMBA.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMBA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 2 - EMBA;#eta;#phi',
                                          cutmask='isSampl2',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["2"])
    
    coverageToolArrayEMBA.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMBA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 3 - EMBA;#eta;#phi',
                                          cutmask='isSampl3',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["3"])

    coverageToolArrayEMBC.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMBC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 0 - EMBC;#eta;#phi',
                                          cutmask='isSampl0',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["0"])

    coverageToolArrayEMBC.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMBC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 1 - EMBC;#eta;#phi',
                                          cutmask='isSampl1',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["1"])

    coverageToolArrayEMBC.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMBC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 2 - EMBC;#eta;#phi',
                                          cutmask='isSampl2',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["2"])

    coverageToolArrayEMBC.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMBC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 3 - EMBC;#eta;#phi',
                                      cutmask='isSampl3',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["C"]["3"])


    coverageToolArrayEMECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMECA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 0 - EMECA;#eta;#phi',
                                           cutmask='isSampl0',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["0"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["0"])
    
    coverageToolArrayEMECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMECA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 1 - EMECA;#eta;#phi',
                                           cutmask='isSampl1',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["1"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["1"])

    coverageToolArrayEMECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMECA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 2 - EMECA;#eta;#phi',
                                           cutmask='isSampl2',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["2"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["2"])
                                                                                       
    coverageToolArrayEMECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMECA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 3 - EMECA;#eta;#phi',
                                           cutmask='isSampl3',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["3"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["A"]["3"])

    coverageToolArrayEMECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling0EMECC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 0 - EMECC;#eta;#phi',
                                           cutmask='isSampl0',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["0"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["0"])

    coverageToolArrayEMECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling1EMECC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 1 - EMECC;#eta;#phi',
                                           cutmask='isSampl1',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["1"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["1"])
    
    coverageToolArrayEMECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling2EMECC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 2 - EMECC;#eta;#phi',
                                           cutmask='isSampl2',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["2"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["2"])
    
    coverageToolArrayEMECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling3EMECC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 3 - EMECC;#eta;#phi',
                                           cutmask='isSampl3',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["3"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMEC"]["C"]["3"])

    coverageToolArrayHECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling0HECA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 0 - HECA;#eta;#phi',
                                          cutmask='isSampl0',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["0"])

    coverageToolArrayHECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling1HECA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 1 - HECA;#eta;#phi',
                                          cutmask='isSampl1',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["1"])

    coverageToolArrayHECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling2HECA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 2 - HECA;#eta;#phi',
                                          cutmask='isSampl2',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["2"])
                                                                                       
    coverageToolArrayHECA.defineHistogram('mon_Eta,mon_Phi;CoverSampling3HECA_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 3 - HECA;#eta;#phi',
                                          cutmask='isSampl3',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["3"])

    coverageToolArrayHECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling0HECC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 0 - HECC;#eta;#phi',
                                          cutmask='isSampl0',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["0"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["0"])

    coverageToolArrayHECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling1HECC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 1 - HECC;#eta;#phi',
                                          cutmask='isSampl1',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["1"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["1"])

    coverageToolArrayHECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling2HECC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 2 - HECC;#eta;#phi',
                                          cutmask='isSampl2',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["2"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["2"])
                                                                                       
    coverageToolArrayHECC.defineHistogram('mon_Eta,mon_Phi;CoverSampling3HECC_StatusCode',
                                          type='TH2I', 
                                          path=coverage_path,
                                          title='Coverage status code={0} - Sampling 3 - HECC;#eta;#phi',
                                          cutmask='isSampl3',
                                          xbins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["3"],
                                          ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["C"]["3"])

    coverageToolArrayFCalA.defineHistogram('mon_Eta,mon_Phi;CoverSampling1FCalA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 1 - FCalA;#eta;#phi',
                                           cutmask='isSampl1',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["1"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["1"])

    coverageToolArrayFCalA.defineHistogram('mon_Eta,mon_Phi;CoverSampling2FCalA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 2 - FCalA;#eta;#phi',
                                           cutmask='isSampl2',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["2"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["2"])
                                                                                       
    coverageToolArrayFCalA.defineHistogram('mon_Eta,mon_Phi;CoverSampling3FCalA_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 3 - FCalA;#eta;#phi',
                                           cutmask='isSampl3',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["3"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["3"])

    coverageToolArrayFCalC.defineHistogram('mon_Eta,mon_Phi;CoverSampling1FCalC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 1 - FCalC;#eta;#phi',
                                           cutmask='isSampl1',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["C"]["1"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["C"]["1"])

    coverageToolArrayFCalC.defineHistogram('mon_Eta,mon_Phi;CoverSampling2FCalC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 2 - FCalC;#eta;#phi',
                                           cutmask='isSampl2',
                                           xbins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["C"]["2"],
                                           ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["C"]["2"])
                                                                                       
    coverageToolArrayFCalC.defineHistogram('mon_Eta,mon_Phi;CoverSampling3FCalC_StatusCode',
                                           type='TH2I', 
                                           path=coverage_path,
                                           title='Coverage status code={0} - Sampling 3 - FCalC;#eta;#phi',
                                           cutmask='isSampl3',
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
    f=open("CoverageMaker.pkl","w")
    cfg.store(f)
    f.close()

