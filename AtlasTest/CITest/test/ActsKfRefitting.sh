#!/usr/bin/bash
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

input_rdo=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/inputs/601237_ttbar_allhad_PU200_ITk_master_v1.RDO.root
n_events=5

Reco_tf.py --CA --steering doRAWtoALL \
   --detectors ITkStrip ITkPixel Bpipe \
   --preExec "ConfigFlags.Reco.EnableHGTDExtension=False;" \
   --preInclude "InDetConfig.ConfigurationHelpers.OnlyTrackingPreInclude" \
   --postInclude "ActsTrkFittingAlgs.ActsTrkFittingAlgsConfig.ActsReFitterAlgCfg" \
   --inputRDOFile ${input_rdo} \
   --outputESDFile ESD.pool.root \
   --outputAODFile AOD.pool.root \
   --maxEvents ${n_events}

