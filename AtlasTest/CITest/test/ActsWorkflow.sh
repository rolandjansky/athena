#!/usr/bin/bash
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

input_rdo=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/inputs/601237_ttbar_allhad_PU200_ITk_master_v1.RDO.root
n_events=10

Reco_tf.py --CA --steering doRAWtoALL \
   --preInclude "InDetConfig.ConfigurationHelpers.OnlyTrackingPreInclude,ActsInterop.ActsCIFlags.actsWorkflowFlags" \
   --postInclude "ActsTrkSeeding.ActsTrkSeedingConfig.ActsTrkSeedingFromAthenaCfg,ActsTrkAnalysis.ActsTrkAnalysisConfig.ActsTrkClusterAnalysisCfg,ActsTrkAnalysis.ActsTrkAnalysisConfig.ActsTrkSpacePointAnalysisCfg,ActsTrkAnalysis.ActsTrkAnalysisConfig.ActsTrkSeedAnalysisCfg,ActsTrkAnalysis.ActsTrkAnalysisConfig.ActsTrkEstimatedTrackParamsAnalysisCfg" \
  --inputRDOFile ${input_rdo} \
  --outputAODFile AOD.pool.root \
  --maxEvents ${n_events}
