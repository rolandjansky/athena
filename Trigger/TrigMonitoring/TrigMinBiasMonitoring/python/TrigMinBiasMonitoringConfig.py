# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigMinBiasMonitoringTool():

        from AthenaCommon.AppMgr import ToolSvc
        
        #PLEASE DO NOT MODIFY EXISTING VARIABLES
        NOALG, MBTS, LUCID, IDMINBIAS, ZDC, BCM, HMT, ALL = 0, 1, 2, 4, 8, 16, 32, 63
        NOCUT, LOOSE, LOOSEPRIMARY, TIGHTPRIMARY = 0, 1, 2, 3

        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConf import HLTMinBiasMonTool
        HLTMinBiasMon = HLTMinBiasMonTool(name           = 'HLTMinBiasMon',
                                          histoPathBase  = "/Trigger/HLT",
                                          SPContainerName       = "HLT_xAOD__TrigSpacePointCountsContainer_spacepoints",
                                          TCContainerName       = "HLT_xAOD__TrigTrackCountsContainer_trackcounts",
                                          T2MBTSContainerName   = "HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts",
										  VCContainerName  		= "HLT_xAOD__TrigVertexCountsContainer_vertexcounts",
										  VColContainerName     = "HLT_TrigVertexCollection_TrigL2SiTrackFinder_FullScan_ZF_Only",
                                          MBTSContainerName     = "MBTSContainer",
                                          BCMContainerName      = "BCM_RDOs",
                                          LUCID_RawDataContainerName    = "Lucid_RawData",
                                          ZDC_ContainerName             = "ZdcRawChannelCollection",
                                          Calo_ContainerName            = "AllCalo",
										  InDetTrackParticleContainerName = "InDetTrackParticles",
										  
										  MinBiasTrigItem = ["mb_sptrk",
															"mb_sp2000_trk70_hmt",
															"mb_sp2000_pusup600_trk70_hmt",
															"mb_sp2000_pusup600_trk70_hmt_L1TE30",
															"mb_sp2000_pusup700_trk70_hmt_L1TE30",
															"mb_sp2000_pusup600_trk70_hmt_L1TE40",
															"mb_sp2000_pusup700_trk70_hmt_L1TE30",
															"mb_perf_L1MBTS_1",
															"mb_perf_L1MBTS_2",
															"mb_idperf_L1MBTS_2",
															"noalg_mb_L1MBTS_2", 
															"mb_mbts_L1MBTS_2",  
															"mb_sptrk_noisesup", 
															"mb_sptrk_costr"],
										  #MinBiasTrigItem = ["noalg_mb_L1MBTS_2", "mb_perf_L1MBTS_2", "mb_mbts_L1MBTS_2", "mb_sptrk", "mb_sptrk_noisesup", "mb_sptrk_costr"],
                                          MinBiasAvailableAlgorithms = ['mbts', 'sptrk', 'noalg', 'perf', 'hmt', 'idperf'],
                                          MinBiasHistoTargets = [MBTS, 0, MBTS, MBTS + IDMINBIAS, HMT, 0],
                                          
                                          MBTS_countsSideA = 16,
                                          MBTS_countsSideC = 16,
										  CutLevel = LOOSE
                                          );
        ToolSvc += HLTMinBiasMon;
        list = [ "HLTMinBiasMonTool/HLTMinBiasMon" ]; 

        return list



