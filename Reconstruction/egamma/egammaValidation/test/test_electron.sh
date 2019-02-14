#!/bin/sh
#
# art-description: ART Monitoring Tool for electron Validation
#
# art-type: grid
# art-input: user.artprod.mc16_13TeV.423000.ParticleGun_single_electron_egammaET.recon.RDO.e3566_s3113_r9388
# art-input-nfiles: 30
# art-cores: 4
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.png

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

	AODMerge_tf.py --inputAODFile=art_core_*/Nightly_AOD_electron.pool.root --outputAOD_MRGFile=Nightly_AOD_electron.pool.root --postExec "all:CILMergeAOD.removeItem(\"xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList\");CILMergeAOD.add(\"xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.ENG_CALIB_TOT.ENG_CALIB_OUT_L.ENG_CALIB_EMB0.ENG_CALIB_EME0.ENG_CALIB_TILEG3.ENG_CALIB_DEAD_TOT.ENG_CALIB_DEAD_EMB0.ENG_CALIB_DEAD_TILE0.ENG_CALIB_DEAD_TILEG3.ENG_CALIB_DEAD_EME0.ENG_CALIB_DEAD_HEC0.ENG_CALIB_DEAD_FCAL.ENG_CALIB_DEAD_LEAKAGE.ENG_CALIB_DEAD_UNCLASS.ENG_CALIB_FRAC_EM.ENG_CALIB_FRAC_HAD.ENG_CALIB_FRAC_REST.LATERAL.ENG_FRAC_EM\");StreamAOD.ItemList=CILMergeAOD()"
	
	echo  "art-result: $? AODMerge"

	set +e

	checkFile.py Nightly_AOD_electron.pool.root > checkFile_Nightly_electron.txt 

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
	
	Reco_tf.py --inputRDOFile=$x --outputAODFile=Nightly_AOD_electron.pool.root --maxEvents=2000 --autoConfiguration="everything" --preExec="from ParticleBuilderOptions.AODFlags import AODFlags; AODFlags.ThinGeantTruth.set_Value_and_Lock(False); rec.doTrigger=False; rec.doMuon=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; " --postExec "ESDtoAOD:CILMergeAOD.removeItem(\"xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList\");CILMergeAOD.add(\"xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.LATERAL.ENG_FRAC_EM.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.ENG_CALIB_TOT.ENG_CALIB_OUT_L.ENG_CALIB_EMB0.ENG_CALIB_EME0.ENG_CALIB_TILEG3.ENG_CALIB_DEAD_TOT.ENG_CALIB_DEAD_EMB0.ENG_CALIB_DEAD_TILE0.ENG_CALIB_DEAD_TILEG3.ENG_CALIB_DEAD_EME0.ENG_CALIB_DEAD_HEC0.ENG_CALIB_DEAD_FCAL.ENG_CALIB_DEAD_LEAKAGE.ENG_CALIB_DEAD_UNCLASS.ENG_CALIB_FRAC_EM.ENG_CALIB_FRAC_HAD.ENG_CALIB_FRAC_REST\");StreamAOD.ItemList=CILMergeAOD()" "POOLMergeAthenaMPAOD0:CILMergeAOD.removeItem(\"xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList\");CILMergeAOD.add(\"xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.LATERAL.ENG_FRAC_EM.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.ENG_CALIB_TOT.ENG_CALIB_OUT_L.ENG_CALIB_EMB0.ENG_CALIB_EME0.ENG_CALIB_TILEG3.ENG_CALIB_DEAD_TOT.ENG_CALIB_DEAD_EMB0.ENG_CALIB_DEAD_TILE0.ENG_CALIB_DEAD_TILEG3.ENG_CALIB_DEAD_EME0.ENG_CALIB_DEAD_HEC0.ENG_CALIB_DEAD_FCAL.ENG_CALIB_DEAD_LEAKAGE.ENG_CALIB_DEAD_UNCLASS.ENG_CALIB_FRAC_EM.ENG_CALIB_FRAC_HAD.ENG_CALIB_FRAC_REST\");StreamAOD.ItemList=CILMergeAOD()"
	
	echo  "art-result: $? reconstruction"

	;;
esac
