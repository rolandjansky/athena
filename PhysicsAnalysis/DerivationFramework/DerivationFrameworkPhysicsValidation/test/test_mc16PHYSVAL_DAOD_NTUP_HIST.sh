#!/bin/sh -x
#
# art-description: AOD to DAOD to PHYSVAL
# art-type: grid
# art-input: user.khoo.mc16_13TeV.410389.MadGraphPythia8EvtGen_A14NNPDF23_ttgamma_nonallhadronic.recon.AOD.e6155_s3126_r10201.DAOD_PHYSVAL_ART
# art-include: 21.2/AthDerivation
# art-input-nfiles: 10
# art-cores: 5
# art-output: art_core_*
# art-output: PHYSVAL_WEB
# art-output: DAOD_PHYSVAL.part1.*
# art-runon: Saturday

echo "ArtProcess: $ArtProcess"

case $ArtProcess in

    "start")
	echo "Starting"
	echo "List of files = " ${ArtInFile}
	;;

    "end")
	echo "Ending"
	
	echo "Merging PHYSVAL NTUP"
	echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
	unset  ATHENA_NUM_PROC
	unset  ATHENA_PROC_NUMBER
	echo "Unset ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
	
	NTUPMerge_tf.py --inputNTUP_PHYSVALFile=art_core_*/PHYSVAL.*.root --outputNTUP_PHYSVAL_MRGFile=PHYSVAL_all.root
	echo  "art-result: $? merge"

        # Web display script is in DQ packages not compiled in 21.2, so we set up 21.0 instead
        export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
        source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh || true
        asetup --platform=x86_64-slc6-gcc62-opt 21.0,Athena,latest,slc6
        asetup --version
	
	# We need to place a reference file on cvmfs
	#physval_make_web_display.py --ratio  --reffile Ref:PHYSVAL_all.root --title Test PHYSVAL_all.root --outdir PHYSVAL_WEB
        mkdir PHYSVAL_WEB
	#domains="BTag Ele Gam IDTrk Jets Muons MET PFlow TopoClusters"
	domains="BTag Electron IDPerformanceMon Jets MET Muons PFlow Photon Summary Tau TopoClusters"
	for slice in ${domains}
      	do 
		physval_make_web_display.py --ratio --reffile Ref:/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkPhysicsValidation/PHYSVAL_all_reference.root --title Test PHYSVAL_all.root --outdir PHYSVAL_WEB/${slice} --startpath ${slice}
	done
	echo  "art-result: $? web"
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
	echo "Unset ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
	
	DAOD_PE="rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = \"BTagCalibRUN12-08-40\"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"InDetTrackParticlesFixer\", Containers = [ \"InDetTrackParticlesAux.\" ] )"
	
	NTUP_PE="all:from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);"
	
	#VALIDFLAGS="doExample,doMET,doPFlow,doEgamma,doInDet,doJet,doBtag,doMuon,doTopoCluster"
	VALIDFLAGS="doExample,doMET,doPFlow,doEgamma,doInDet,doJet,doBtag,doMuon,doTau,doTopoCluster"
	
	Reco_tf.py --inputAODFile=$x --maxEvents=5000 --reductionConf PHYSVAL --preExec "$DAOD_PE" --outputDAODFile part1.pool.root
	echo  "art-result: $? daod_part1"

	Reco_tf.py $NTUP_ARGS --inputAODFile=./DAOD_PHYSVAL.part1.pool.root --outputNTUP_PHYSVALFile PHYSVAL.part1.root --preExec "$NTUP_PE" --valid=True --validationFlags $VALIDFLAGS
	echo  "art-result: $? physval_part1"
	
        if [ -f ../DAOD_PHYSVAL.part1.*.pool.root ]
        then
                echo "DAOD found in top directory, removing $(pwd)/DAOD_PHYSVAL part_1"
		ls -lR
		rm -f DAOD_PHYSVAL.part1.pool.root
		echo "Removed DAOD part 1"
		ls -lR
        else
                echo "DAOD not found, moving the $(pwd)/DAOD_PHYSVAL part_1 file to top directory"
		ls -lR
                mv ./DAOD_PHYSVAL.part1.pool.root ../DAOD_PHYSVAL.part1.${ArtProcess}.pool.root
		echo "Moved DAOD part 1"
		ls -lR 
        fi

	
	Reco_tf.py --inputAODFile=$x --maxEvents=5000 --reductionConf PHYSVAL --preExec "$DAOD_PE" --outputDAODFile part2.pool.root --skipEvents 5000
	echo  "art-result: $? daod_part2"
	
	Reco_tf.py $NTUP_ARGS --inputAODFile=./DAOD_PHYSVAL.part2.pool.root --outputNTUP_PHYSVALFile PHYSVAL.part2.root --preExec "$NTUP_PE" --valid=True --validationFlags $VALIDFLAGS
	echo  "art-result: $? physval_part2"
	
	ls -lR
	echo "Removing DAOD part 2"
	rm -f DAOD_PHYSVAL.part2.pool.root
	echo "Removed DAOD part 2"
	ls -lR
	;;
esac
