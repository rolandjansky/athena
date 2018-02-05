#!/bin/sh
#
# art-description: ART Monitoring Tool for electron Validation
#
# art-type: grid
# art-output: *.pool.root
# art-output: *.txt
# art-output: *.root

Reco_tf.py --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/rootMCFiles/mc16_13TeV.423000.ParticleGun_single_electron_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11229424._000031.pool.root.1 --outputAODFile=ART_Rec_electron.pool.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "

checkFile.py ART_Rec_electron.pool.root > checkFile_ART_electron.txt
checkxAOD.py ART_Rec_electron.pool.root > checkxAOD_ART_electron.txt

diff checkFile_ART_electron.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkFile_Baseline_electron.txt > diffFile_electron.txt
diff checkxAOD_ART_electron.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkxAOD_Baseline_electron.txt > diffxAOD_electron.txt

get_files -jo egamma_art_checker_joboptions.py
athena -c "particleType='electron'" egamma_art_checker_joboptions.py
