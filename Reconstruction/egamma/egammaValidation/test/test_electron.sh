#!/bin/sh
#
# art-description: ART Monitoring Tool for electron Validation
#
# art-type: grid
# art-input: mc16_13TeV.423000.ParticleGun_single_electron_egammaET.recon.RDO.e3566_s3113_r9388
# art-input-nfiles: 30
# art-cores: 4
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.png
# art-output: log.RAWtoESD
# art-output: log.ESDtoAOD

echo "ArtProcess: $ArtProcess"

case $ArtProcess in
    
    "start")
	echo "Starting"
	echo "List of files = " ${ArtInFile}
	;;

    "end")
	echo "Ending"
	
	echo "Merging AODs"
        echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
        unset  ATHENA_NUM_PROC
        unset  ATHENA_PROC_NUMBER

	AODMerge_tf.py --inputAODFile=art_core_*/Nightly_AOD_electron.pool.root --outputAOD_MRGFile=Nightly_AOD_electron.pool.root
	
	echo  "art-result: $? AODMerge"

	set +e

	checkFile.py Nightly_AOD_electron.pool.root > checkFile_Nightly_electron.txt 
	checkxAOD.py Nightly_AOD_electron.pool.root > checkxAOD_Nightly_electron.txt 

	echo  "art-result: $? checks_files"

	get_files -jo egamma_art_checker_joboptions.py
	athena -c "particleType='electron'" egamma_art_checker_joboptions.py

	echo  "art-result: $? athena_job"

	EgammaARTmonitoring_plotsMaker.py /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/rootFiles/Baseline-monitoring_electron.hist.root Nightly-monitoring_electron.hist.root electron

	echo  "art-result: $? final_comparison"

	;;

    *)
	echo "Test $ArtProcess"

	mkdir "art_core_${ArtProcess}"
	cd "art_core_${ArtProcess}"
	
	IFS=',' read -r -a file <<< "${ArtInFile}"
	file=${file[${ArtProcess}]}
	x="../$file"

	echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
	unset  ATHENA_NUM_PROC
	unset  ATHENA_PROC_NUMBER
	
	Reco_tf.py --inputRDOFile=$x --outputAODFile=Nightly_AOD_electron.pool.root --maxEvents=2000 --autoConfiguration="everything" --preExec="from ParticleBuilderOptions.AODFlags import AODFlags; AODFlags.ThinGeantTruth.set_Value_and_Lock(False); rec.doTrigger=False; rec.doMuon=True; rec.doTau=True ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=True ; recAlgs.doEFlow=True ; recAlgs.doEFlowJet=True ; recAlgs.doMissingET=True ; recAlgs.doMissingETSig=True ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=True ; "
	
	echo  "art-result: $? reconstruction"

	;;
esac
