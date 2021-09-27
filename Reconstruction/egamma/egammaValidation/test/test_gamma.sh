#!/bin/sh
#
# art-description: ART Monitoring Tool for gamma Validation
#
# art-type: grid
# art-input: mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3112_r12663
# art-input-nfiles: 30
# art-cores: 4
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.png
# art-output: log.*
# art-output: dcube

echo "ArtProcess: $ArtProcess"

case $ArtProcess in
    
    "start")
	echo "Starting"
	echo "List of files = " ${ArtInFile}
	;;

    "end")
	echo "Ending"
	
	echo "Merging AODs"
        echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC}"
        unset  ATHENA_NUM_PROC

        AODMerge_tf.py --inputAODFile=art_core_*/Nightly_AOD_gamma.pool.root --outputAOD_MRGFile=Nightly_AOD_gamma.pool.root  --preExec "from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyMergeFlags; setRunEgammaOnlyMergeFlags()" --postInclude "all:egammaValidation/egammaArtCaloCalPostInclude.py"
	
	echo  "art-result: $? AODMerge"

	set +e

	checkFile.py Nightly_AOD_gamma.pool.root > checkFile_Nightly_gamma.txt 

	echo  "art-result: $? checks_files"

	get_files -jo egamma_art_checker_joboptions.py
	athena -c "particleType='gamma'" egamma_art_checker_joboptions.py

	echo  "art-result: $? athena_job"

	EgammaARTmonitoring_plotsMaker.py /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/rootFiles/Baseline-monitoring_gamma.hist.root Nightly-monitoring_gamma.hist.root gamma

	echo  "art-result: $? final_comparison"

	$ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py -p -x dcube -c /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/DCube_Config/gamma.xml -r /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Nightly_Files/Nightly-monitoring_gamma.hist.root  Nightly-monitoring_gamma.hist.root
	echo  "art-result: $? plot"

	;;

    *)
	echo "Test $ArtProcess"

	mkdir "art_core_${ArtProcess}"
	cd "art_core_${ArtProcess}"

	IFS=',' read -r -a file <<< "${ArtInFile}"
        file=${file[${ArtProcess}]}
	x="../$file"
		
	echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC}"
	unset  ATHENA_NUM_PROC
	
	Reco_tf.py --inputRDOFile=$x --outputAODFile=Nightly_AOD_gamma.pool.root --maxEvents=2000 --autoConfiguration="everything" --conditionsTag="OFLCOND-MC16-SDR-RUN2-09" --preExec="from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyRecoFlags; setRunEgammaOnlyRecoFlags()"  --postInclude "ESDtoAOD:egammaValidation/egammaArtCaloCalPostInclude.py" "POOLMergeAthenaMPAOD0:egammaValidation/egammaArtCaloCalPostInclude.py"

	echo  "art-result: $? reconstruction"

	;;
esac
