# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def TrigMinBiasMonitoringTool():

        #---CRITICAL---
        NOALG, MBTS, LUCID, IDMINBIAS, ZDC, BCM, HMT, ALL = 0, 1, 2, 4, 8, 16, 32, 63  # noqa: F841
        NOCUT, LOOSE, LOOSEPRIMARY, TIGHTPRIMARY = 0, 1, 2, 3                          # noqa: F841
        PHYSICS, L1_ISPASSEDBEFOREPRESCALE = 0, 1
        #!---CRITICAL---

        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConf import HLTMinBiasMonTool
        from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool

        HLTMinBiasMon = HLTMinBiasMonTool(name           = 'HLTMinBiasMon',
                                          histoPathBase  = "/Trigger/HLT",
                                          SPContainerName       = "HLT_xAOD__TrigSpacePointCountsContainer_spacepoints",
                                          TCContainerName       = "HLT_xAOD__TrigTrackCountsContainer_trackcounts",
                                          T2MBTSContainerName   = "HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts",
                                          VCContainerName               = "HLT_xAOD__TrigVertexCountsContainer_vertexcounts",
                                          VColContainerName     = "HLT_TrigVertexCollection_TrigL2SiTrackFinder_FullScan_ZF_Only",
                                          MBTSContainerName     = "MBTSContainer",
                                          BCMContainerName      = "BCM_RDOs",
                                          LUCID_RawDataContainerName    = "Lucid_RawData",
                                          ZDC_ContainerName             = "ZdcRawChannelCollection",
                                          Calo_ContainerName            = "AllCalo",
                                          InDetTrackParticleContainerName = "InDetTrackParticles",
                                          monitoring_minbias = hltmonList.monitoring_minbias,
                                          MinBiasRefTrigItem = "mb_sptrk", # "mb_sptrk" should be used, "noalg_L1RD0_FILLED" for pp
                                          MinBiasAvailableAlgorithms = ['sp', 'sptrk', 'mbts', 'noalg', 'hmtperf',  'idperf',   'perf',   'hmt'], #the order here matters: more specific names should go before general eg. 'ideperf' before 'perf'... 
                                          MinBiasHistoTargets =     [IDMINBIAS,  IDMINBIAS, MBTS,  0,  HMT,   0,  MBTS + IDMINBIAS,   HMT],
                                          IsPassedCondtitions =     [PHYSICS,  PHYSICS, PHYSICS, L1_ISPASSEDBEFOREPRESCALE,  PHYSICS,   PHYSICS,   PHYSICS,   PHYSICS],
                                          #CollectiveHistogramForAlgorithm = ['1', '1', '1', '1', '2', '2', '2', '3'],
                                          CollectiveHistogramForAlgorithm = ['1', '1', '1', '1', '1', '1', '1', '1'],
                                          MBTS_countsSideA = 12,
                                          MBTS_countsSideC = 12
                                          )
        # ---------------------- InDetTrackSelectionTool

        #'mbts'
        HLTMinBiasMon.mbtsEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #'sptrk'
        HLTMinBiasMon.sptrkEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #'noalg'
        HLTMinBiasMon.noalgEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #'hmtperf'
        HLTMinBiasMon.hmtperfEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #'idperf'
        HLTMinBiasMon.idperfEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #'perf'
        HLTMinBiasMon.perfEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #'hmt'
        HLTMinBiasMon.hmtEfficiencyTrackSelectionTool.CutLevel = "LoosePrimary"

        #purity
        HLTMinBiasMon.PurityTrackSelectionTool.CutLevel = "LoosePrimary"

        # ---------------------- !InDetTrackSelectionTool

        # ---------------------- Collective Histograms Management

        tmpList = []
        for i in range(len(HLTMinBiasMon.CollectiveHistogramForAlgorithm)):
        #for i in HLTMinBiasMon.CollectiveHistogramForAlgorithm:
                HLTMinBiasMon.CollectiveHistogramForAlgorithm[i] = HLTMinBiasMon.CollectiveHistogramForAlgorithm[i] if HLTMinBiasMon.CollectiveHistogramForAlgorithm[i] != '' else '_' #forces uniqueness
                if HLTMinBiasMon.CollectiveHistogramForAlgorithm[i] not in tmpList:
                        tmpList.append(HLTMinBiasMon.CollectiveHistogramForAlgorithm[i])
        HLTMinBiasMon.CollectiveHistogramsNames = tmpList

        list = [ HLTMinBiasMon ]

        return list
