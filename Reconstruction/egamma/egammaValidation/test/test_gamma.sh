#!/bin/sh
#
# art-description: ART Monitoring Tool for gamma Validation
#
# art-type: grid
# art-output: *.pool.root
# art-output: *.txt
# art-output: *.root

Reco_tf.py --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/rootMCFiles/mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11223039._000027.pool.root.1 --outputAODFile=ART_Rec_gamma.pool.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "

checkFile.py ART_Rec_gamma.pool.root > checkFile_ART_gamma.txt
checkxAOD.py ART_Rec_gamma.pool.root > checkxAOD_ART_gamma.txt

diff checkFile_ART_gamma.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkFile_Baseline_gamma.txt > diffFile_gamma.txt
diff checkxAOD_ART_gamma.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkxAOD_Baseline_gamma.txt > diffxAOD_gamma.txt

get_files -jo egamma_art_checker_joboptions.py
athena -c "particleType='gamma'" egamma_art_checker_joboptions.py

