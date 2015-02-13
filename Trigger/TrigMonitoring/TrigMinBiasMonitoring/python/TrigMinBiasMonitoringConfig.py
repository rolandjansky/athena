# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigMinBiasMonitoringTool():
		
		#----NORMAL USER ACCESS-------
        triggers = ["mb_sptrk",	
					"mb_sp2000_trk70_hmt", 
					"mb_sp2000_pusup600_trk70_hmt", 
					"mb_sp2000_pusup600_trk70_hmt_L1TE30", 
					"mb_sp2000_pusup700_trk70_hmt_L1TE30", 
					"mb_sp2000_pusup600_trk70_hmt_L1TE40",	
					"mb_sp1800_hmtperf_L1TE20",	
					"mb_perf_L1MBTS_1",	
					"mb_perf_L1MBTS_2",	
					"mb_idperf_L1MBTS_2",
					"noalg_mb_L1MBTS_2", 
					"mb_mbts_L1MBTS_2",
					"mb_sptrk_noisesup", 
					"mb_sptrk_costr"]
		
		#!----NORMAL USER ACCESS-------
		
		#----ADVANCED USER ACCESS-----
		#				PLEASE DO NOT MODIFY EXISTING CODE UNLESS YOU ARE PERFECTLY SURE WHAT YOU ARE DOING 
		
		#forces uniqueness of items on the list
        triggersFiltered = []
        for x in triggers:
        	if x not in triggersFiltered:
        		triggersFiltered.append(x)
		
        from AthenaCommon.AppMgr import ToolSvc
        
		#---CRITICAL---
		
        NOALG, MBTS, LUCID, IDMINBIAS, ZDC, BCM, HMT, ALL = 0, 1, 2, 4, 8, 16, 32, 63
        NOCUT, LOOSE, LOOSEPRIMARY, TIGHTPRIMARY = 0, 1, 2, 3
		
		#!---CRITICAL---
		
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
										  
										  monitoring_minbias = triggersFiltered,
										  MinBiasRefTrigItem = "mb_sptrk", # "mb_sptrk" should be used
										  #MinBiasTrigItem = ["noalg_mb_L1MBTS_2", "mb_perf_L1MBTS_2", "mb_mbts_L1MBTS_2", "mb_sptrk", "mb_sptrk_noisesup", "mb_sptrk_costr"],
                                          MinBiasAvailableAlgorithms = ['mbts', 'sptrk', 'noalg', 'idperf', 	 'perf',       'hmt'], #the order here should matter (?) more specific names before more general eg. 'ideperf' before 'perf'... to think it through...
                                          MinBiasHistoTargets = 	   [MBTS,      0,     MBTS,       0, 	MBTS + IDMINBIAS, 	HMT],
                                          MinBiasEffCuts =             [LOOSE,   LOOSE,  LOOSE,     LOOSE, 		LOOSE,       LOOSEPRIMARY],
										  MinBiasPurCuts = [LOOSE],
										  
                                          MBTS_countsSideA = 16,
                                          MBTS_countsSideC = 16
                                          );
        ToolSvc += HLTMinBiasMon;
        list = [ "HLTMinBiasMonTool/HLTMinBiasMon" ];

        return list
		#!----ADVANCED USER ACCESS-----



