# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigMinBiasMonitoringTool():

        from AthenaCommon.AppMgr import ToolSvc
        
        #PLEASE DO NOT MODIFY EXISTING VARIABLES
        NOALG, MBTS, LUCID, IDMINBIAS, ZDC, BCM, ALL = 0, 1, 2, 4, 8, 16, 31

        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConf import HLTMinBiasMonTool
        HLTMinBiasMon = HLTMinBiasMonTool(name           = 'HLTMinBiasMon',
                                          histoPathBase  = "/Trigger/HLT",
                                          SPContainerName       = "HLT_xAOD__TrigSpacePointCountsContainer_spacepoints",
                                          TCContainerName       = "HLT_xAOD__TrigTrackCountsContainer_trackcounts",
                                          T2MBTSContainerName   = "HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts",
                                          MBTSContainerName     = "MBTSContainer",
                                          BCMContainerName      = "BCM_RDOs",
                                          LUCID_RawDataContainerName    = "Lucid_RawData",
                                          ZDC_ContainerName             = "ZdcRawChannelCollection",
                                          Calo_ContainerName            = "AllCalo",
                                          
                                          EF_mbSpTrk = "HLT_mb_perf_L1MBTS_1",#"EF_mbSpTrk"
                                          EF_mbSp = "EF_mbSp",
                                          EF_mbMbts_2_eff = "EF_mbMbts_2_eff",
                                          EF_mbRd1_eff = "EF_mbRd1_eff",
                                          EF_mbLucid_eff = "EF_mbLucid_eff",
                                          EF_rd0_filled_NoAlg = "EF_rd0_filled_NoAlg",
                                          EF_L1ItemStreamer_L1_MBTS_2 = "EF_L1ItemStreamer_L1_MBTS_2",
                                          EF_mbZdc_eff = "EF_mbZdc_a_c_eff",
                                          #EF_mbLucid_eff = "EF_mbLucid_eff",
                                          EF_mbMbts_1_1_eff = "EF_mbMbts_1_1_eff",
                                          EF_mbMbts_2_2_eff = "EF_mbMbts_2_2_eff",
                                          EF_mbMbts_4_4_eff = "EF_mbMbts_4_4_eff",
                                          EF_mbSpTrkVtxMh = "EF_mbSpTrkVtxMh",
                                          EF_mbSpTrkVtxMh_eff = "EF_mbSpTrkVtxMh_eff",
                                          EF_mbSpTrkVtxMh_hip_trk150_L1TE50 = "EF_mbSpTrkVtxMh_hip_trk150_L1TE50",
                                          EF_mbSpTrkVtxMh_hip_trk190_L1TE50 = "EF_mbSpTrkVtxMh_hip_trk190_L1TE50",
                                          EF_mbSpTrkVtxMh_hip_trk120_L1TE35 = "EF_mbSpTrkVtxMh_hip_trk120_L1TE35",
                                          EF_mbSpTrkVtxMh_hip_trk190_L1TE35 = "EF_mbSpTrkVtxMh_hip_trk190_L1TE35",
                                          EF_mbSpTrkVtxMh_hip_trk240_L1TE35 = "EF_mbSpTrkVtxMh_hip_trk240_L1TE35",
                                          EF_mbSpTrkVtxMh_hip_trk180_L1TE65 = "EF_mbSpTrkVtxMh_hip_trk180_L1TE65",
                                          EF_mbSpTrkVtxMh_hip_trk200_L1TE65 = "EF_mbSpTrkVtxMh_hip_trk200_L1TE65",
                                          EF_mbSpTrkVtxMh_hip_trk240_L1TE65 = "EF_mbSpTrkVtxMh_hip_trk240_L1TE65",
                                          EF_mbSpTrkVtxMh_hip_trk175_L1MBTS = "EF_mbSpTrkVtxMh_hip_trk175_L1MBTS",
                                          EF_L1ZDC_A_NoAlg = "EF_L1ZDC_A_NoAlg",
                                          EF_L1ZDC_C_NoAlg = "EF_L1ZDC_C_NoAlg",
                                          EF_L1LHCF_NoAlg = "EF_L1LHCF_NoAlg",
                                          
                                          MinBiasTrigItem = ["mb_perf_L1MBTS_1", "mb_perf_L1MBTS_2", "mb_mbts_L1MBTS_2"],
                                          MinBiasAvailableAlgorithms = ['mbts', 'sptrk', 'noalg', 'perf', 'hmt', 'idperf'],
                                          MinBiasHistoTargets = [MBTS, 0, 0, MBTS + IDMINBIAS, 0, 0],
                                          
                                          MBTS_countsSideA = 16,
                                          MBTS_countsSideC = 16
                                          );
        ToolSvc += HLTMinBiasMon;
        list = [ "HLTMinBiasMonTool/HLTMinBiasMon" ]; 

        return list



