#!/bin/sh
#
# art-description: ART Monitoring Tool for electron Validation
#
# art-type: grid
# art-input: mc16_13TeV.423000.ParticleGun_single_electron_egammaET.recon.RDO.e3566_s3113_r9388
# art-input-nfiles: 5
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.pdf
# art-output: *.png

echo "List of files = " ${ArtInFile}

Reco_tf.py --inputRDOFile=${ArtInFile} --outputAODFile=Nightly_AOD_electron.pool.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doMuon=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "

echo  "art-result: $? reconstruction"

set +e

checkFile.py Nightly_AOD_electron.pool.root > checkFile_Nightly_electron.txt 
checkxAOD.py Nightly_AOD_electron.pool.root > checkxAOD_Nightly_electron.txt 

echo  "art-result: $? checks_files"

diff checkFile_Nightly_electron.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkFile_Baseline_electron.txt > diffFile_electron.txt || true
diff checkxAOD_Nightly_electron.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkxAOD_Baseline_electron.txt > diffxAOD_electron.txt || true

echo  "art-result: $? diff_files"

get_files -jo egamma_art_checker_joboptions.py
athena -c "particleType='electron'" egamma_art_checker_joboptions.py

echo  "art-result: $? athena_job"

get_files -jo plotsMaker.py
python plotsMaker.py /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/rootFiles/Baseline-monitoring_electron.hist.root Nightly-monitoring_electron.hist.root electron

echo  "art-result: $? final_comparison"
