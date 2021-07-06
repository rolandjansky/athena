#!/bin/sh -x
#
# art-description: AOD to DAOD to PHYSVAL
# art-type: grid
# art-input: valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e4993_s3227_r12287
# art-include: master/Athena
# art-input-nfiles: 10
# art-cores: 5
# art-output: art_core_*
# art-output: PHYSVAL_WEB
# art-output: DAOD_PHYSVAL.part1.*

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

        export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
        source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh || true
        asetup master,Athena,latest
        asetup --version
	
	# We need to place a reference file on cvmfs
	#physval_make_web_display.py --ratio  --reffile Ref:PHYSVAL_all.root --title Test PHYSVAL_all.root --outdir PHYSVAL_WEB
        mkdir PHYSVAL_WEB
	#domains="BTag Ele Gam IDTrk Jets Muons MET PFlow TopoClusters"
	domains="BTag Electron IDPerformanceMon Jets MET Muons PFlow Photon Summary Tau TopoClusters"
	for slice in ${domains}
      	do 
		physval_make_web_display.py --ratio --reffile Ref:/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkPhysicsValidation/PHYSVAL_all_reference.root --title Test PHYSVAL_all.root --outdir PHYSVAL_WEB/${slice} --startpath ${slice} --jsRoot
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
	
	DAOD_PE="default:from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); from AthenaMP.AthenaMPFlags import jobproperties as ampjp;ampjp.AthenaMPFlags.UseSharedWriter=True; import AthenaPoolCnvSvc.AthenaPool;ServiceMgr.AthenaPoolCnvSvc.OutputMetadataContainer=\"MetaData\"; from AthenaCommon.AlgSequence import AlgSequence;topSequence = AlgSequence ();topSequence += CfgMgr.xAODMaker__DynVarFixerAlg(\"BTaggingELFixer\", Containers = [\"BTagging_AntiKt4EMTopoAux.\" ] ); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg(\"JetELFixer\", Containers = [\"AntiKt4EMTopoJetsAux.\"] );"

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
