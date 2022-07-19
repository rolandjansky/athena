#!/usr/bin/bash
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

input_rdo=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/inputs/601237_ttbar_allhad_PU200_ITk_master_v1.RDO.root
n_events=10

Reco_tf.py --CA --preInclude "InDetConfig.ConfigurationHelpers.OnlyTrackingPreInclude,ActsInterop.ActsCIFlags.actsSiSpacePointSeedMakerFlags" \
  --steering doRAWtoALL \
  --inputRDOFile ${input_rdo} \
  --outputAODFile test.AOD.pool.root  \
  --maxEvents ${n_events}

