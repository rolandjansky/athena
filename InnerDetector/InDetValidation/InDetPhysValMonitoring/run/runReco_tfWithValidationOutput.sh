#!/bin/bash
exec 2>&1
run() { (set -x; exec "$@") }

dcubemon_rec=physval.root
artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
hits_ref=$artdata/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3126/HITS.12860054._032508.pool.root.1

hits="$hits_ref"
run ls -lL "$hits"
run Reco_tf.py \
    --inputHITSFile   "$hits" \
    --outputAODFile   physval.AOD.root \
    --outputNTUP_PHYSVALFile "$dcubemon_rec" \
    --steering        doRAWtoALL \
    --checkEventCount False \
    --ignoreErrors    True \
    --maxEvents       10 \
    --valid           True \
    --validationFlags doInDet \
    --preExec 'from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doSlimming.set_Value_and_Lock(False); rec.doTrigger.set_Value_and_Lock(False); from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True); InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(True); InDetPhysValFlags.doValidateGSFTracks.set_Value_and_Lock(False); rec.doDumpProperties=True; rec.doCalo=False; rec.doEgamma=False; rec.doForwardDet=False; rec.doInDet=True; rec.doJetMissingETTag=False; rec.doLArg=False; rec.doLucid=False; rec.doMuon=False; rec.doMuonCombined=False; rec.doSemiDetailedPerfMon=True; rec.doTau=False; rec.doTile=False;'
  echo "art-result: $? reco"
