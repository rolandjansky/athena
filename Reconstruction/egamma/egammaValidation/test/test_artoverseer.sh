#!/bin/sh

# art-type: build
# art-output: *.pool.root
# art-output: *.txt
# art-output: *.root

Reco_tf.py --inputRDOFile=/afs/.cern.ch/work/m/mscodegg/public/QualiTask/rootMCFiles/mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11223039._000027.pool.root.1 --outputAODFile=ART_Rec_gamma.pool.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "

checkFile.py ART_Rec_gamma.pool.root > checkFile_ART_gamma.txt
checkxAOD.py ART_Rec_gamma.pool.root > checkxAOD_ART_gamma.txt

diff checkFile_ART_gamma.txt /afs/cern.ch/work/m/mscodegg/public/ART_Baseline_Files/txtFiles/checkFile_Baseline_gamma.txt > diffFile_gamma.txt
diff checkxAOD_ART_gamma.txt /afs/cern.ch/work/m/mscodegg/public/ART_Baseline_Files/txtFiles/checkxAOD_Baseline_gamma.txt > diffxAOD_gamma.txt

get_files -jo egamma_art_checker_joboptions.py
athena -c "particleType='gamma'" egamma_art_checker_joboptions.py

Reco_tf.py --inputRDOFile=/afs/cern.ch/work/m/mscodegg/public/QualiTask/rootMCFiles/mc16_13TeV.423000.ParticleGun_single_electron_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11229424._000031.pool.root.1 --outputAODFile=ART_Rec_electron.pool.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "

checkFile.py ART_Rec_electron.pool.root > checkFile_ART_electron.txt
checkxAOD.py ART_Rec_electron.pool.root > checkxAOD_ART_electron.txt

diff checkFile_ART_electron.txt /afs/cern.ch/work/m/mscodegg/public/QualiTask/Baseline_Files/txtFiles/checkFile_Baseline_electron.txt > diffFile_electron.txt
diff checkxAOD_ART_electron.txt /afs/cern.ch/work/m/mscodegg/public/QualiTask/Baseline_Files/txtFiles/checkxAOD_Baseline_electron.txt > diffxAOD_electron.txt

get_files -jo egamma_art_checker_joboptions.py
athena -c "particleType='electron'" egamma_art_checker_joboptions.py