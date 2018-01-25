#!/bin/sh

# art-type: build

set -e


Reco_tf.py  --inputRDOFile=/afs/.cern.ch/work/m/mscodegg/public/QualiTask/mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11223039._000027.pool.root.1 --outputAODFile=/afs/cern.ch/work/m/mscodegg/public/QualiTask/athena/Reconstruction/egamma/egammaValidation/test/ART_Baseline_Checker/rootART/ART_Rec.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; " 
