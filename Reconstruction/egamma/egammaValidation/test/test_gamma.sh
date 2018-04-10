#!/bin/sh
#
# art-description: ART Monitoring Tool for gamma Validation
#
# art-type: grid
# art-input: mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388
# art-input-nfiles: 5
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.pdf
# art-output: *.png

echo "List of files = " ${ArtInFile}

Reco_tf.py --inputRDOFile=${ArtInFile} --outputAODFile=Nightly_AOD_gamma.pool.root --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doMuon=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "

echo  "art-result: $? reconstruction"

set +e

checkFile.py Nightly_AOD_gamma.pool.root > checkFile_Nightly_gamma.txt
checkxAOD.py Nightly_AOD_gamma.pool.root > checkxAOD_Nightly_gamma.txt

echo  "art-result: $? checks_files"

diff checkFile_Nightly_gamma.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkFile_Baseline_gamma.txt > diffFile_gamma.txt || true
diff checkxAOD_Nightly_gamma.txt /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/txtFiles/checkxAOD_Baseline_gamma.txt > diffxAOD_gamma.txt || true

echo  "art-result: $? diff_files"

get_files -jo egamma_art_checker_joboptions.py
athena -c "particleType='gamma'" egamma_art_checker_joboptions.py

echo  "art-result: $? athena_job"

get_files -jo plotsMaker.py
python plotsMaker.py /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/rootFiles/Baseline-monitoring_gamma.hist.root Nightly-monitoring_gamma.hist.root gamma

echo  "art-result: $? final_comparison"
