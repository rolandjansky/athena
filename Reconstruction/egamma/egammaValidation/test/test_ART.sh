#!/bin/sh

# art-type: build
# art-output: *.root
# art-output: *.txt
# art-output: ART-monitoring.root

set -e

Reco_tf.py  --inputRDOFile=/afs/.cern.ch/work/m/mscodegg/public/QualiTask/mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11223039._000027.pool.root.1 --outputAODFile=ART_Rec.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "


checkFile.py ART_Rec.root > checkFile_ART.txt
checkxAOD.py ART_Rec.root > checkxAOD_ART.txt

diff checkFile_ART.txt /afs/cern.ch/work/m/mscodegg/public/QualiTask/ART_Baseline_Checker/checkFile_Baseline.txt > diffFile.txt
diff checkxAOD_ART.txt /afs/cern.ch/work/m/mscodegg/public/QualiTask/ART_Baseline_Checker/checkxAOD_Baseline.txt > diffxAOD.txt

get_files -jo egammaARTchecker_job_options.py
athena.py egammaARTchecker_joboptions.py