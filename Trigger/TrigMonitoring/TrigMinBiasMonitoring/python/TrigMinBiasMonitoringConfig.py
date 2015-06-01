# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigMinBiasMonitoringTool():
	from AthenaCommon.AppMgr import ToolSvc      

	#---CRITICAL---		
	NOALG, MBTS, LUCID, IDMINBIAS, ZDC, BCM, HMT, ALL = 0, 1, 2, 4, 8, 16, 32, 63
	NOCUT, LOOSE, LOOSEPRIMARY, TIGHTPRIMARY = 0, 1, 2, 3
	PHYSICS, L1_ISPASSEDBEFOREPRESCALE = 0, 1
	#!---CRITICAL---
		
	from TrigMinBiasMonitoring.TrigMinBiasMonitoringConf import HLTMinBiasMonTool
	from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool

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
										  
										  monitoring_minbias = hltmonList.monitoring_minbias,
										  MinBiasRefTrigItem = "mb_sptrk", # "mb_sptrk" should be used
                                          MinBiasAvailableAlgorithms = ['mbts', 	  'sptrk', 		'noalg', 			'hmtperf', 		'idperf', 	 	'perf',       'hmt'], #the order here should matter (?) more specific names before more general eg. 'ideperf' before 'perf'... to think it through...
                                          MinBiasHistoTargets = 	   [MBTS,      	  IDMINBIAS,     MBTS, 					HMT,       		0, 		MBTS + IDMINBIAS, 	HMT],
                                          MinBiasEffCuts =             [LOOSEPRIMARY, LOOSEPRIMARY,  LOOSEPRIMARY,  	LOOSEPRIMARY,  LOOSEPRIMARY, LOOSEPRIMARY, LOOSEPRIMARY],
										  MinBiasPurCuts = 			   [LOOSEPRIMARY],
										  IsPassedCondtitions = 	   [PHYSICS, 	  PHYSICS, L1_ISPASSEDBEFOREPRESCALE,  PHYSICS,  	PHYSICS, 		PHYSICS, 	  PHYSICS],
										  
                                          MBTS_countsSideA = 12,
                                          MBTS_countsSideC = 12
                                          );
	ToolSvc += HLTMinBiasMon;
	list = [ "HLTMinBiasMonTool/HLTMinBiasMon" ];

	return list
#!----ADVANCED USER ACCESS-----



