#!/bin/sh
#
# art-description: Test the muon digitization for New Small Wheel.
#
# art-type: grid
#
# art-include: 21.3/Athena

set -x

Reco_tf.py   --preExec 'from AthenaCommon.DetFlags import DetFlags;DetFlags.ID_setOn();DetFlags.Calo_setOn();DetFlags.Muon_setOn();DetFlags.MDT_setOn();DetFlags.CSC_setOff();DetFlags.TGC_setOn();DetFlags.RPC_setOn();DetFlags.sTGC_setOn();DetFlags.MM_setOn();DetFlags.Truth_setOn()' \
             --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/NSW/digit/*.root  \
             --maxEvents -1 \
             --outputRDOFile NSW_SingleMuon_Digitization_RDO_.pool.root

echo "art-result: $?"
