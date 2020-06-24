#!/bin/sh
#
# art-description: test
#
# art-type: grid
# art-input: mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.merge.EVNT.e6337_e5984
# art-input-nfiles: 1
# art-cores: 10
# art-include: master/Athena
# art-output: *.root
# art-output: art_core*
# art-output: dcube-*
# art-html: dcube-g4msVsaf2

echo "ArtProcess: $ArtProcess"

case $ArtProcess in
    
    "start")
	echo "Starting"
	echo "List of files = " ${ArtInFile}
	;;

    "end")

	echo "Ending"

      	echo "Merging PHYS_VAL outputs"
        echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC}"
        unset  ATHENA_NUM_PROC

        # merge the outputs
        merge_ntup_file=physval_g4ms_validation_merge.root
        NTUPMerge_tf.py --inputNTUP_PHYSVALFile=art_core_*/physval_g4ms_* --outputNTUP_PHYSVAL_MRGFile=${merge_ntup_file}
        echo  "art-result: $? ntup-merge"

        dcubeXmlNTUP="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}/physval-validation.xml"
        dcubeRefNTUP="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_g4ms_validation/${merge_ntup_file}"
        dcubeRefAF2NTUP="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_af2_validation/physval_af2_validation_merge.root"

        # Histogram comparison of G4MS with AF2 DCube
         $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
         -p -x dcube-physval \
         -c ${dcubeXmlNTUP} -r ${dcubeRefNTUP} ./${merge_ntup_file}
         echo  "art-result: $? dcube-physval"

         # Histogram comparison of G4MS with AF2 DCube
         $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
         -p -x dcube-g4msVsaf2 \
         -c ${dcubeXmlNTUP} -r ${dcubeRefAF2NTUP} ./${merge_ntup_file}
         echo  "art-result: $? dcube-g4msVsaf2"

	;;

    *)
	echo "Test $ArtProcess"

	mkdir "art_core_${ArtProcess}"
	cd "art_core_${ArtProcess}"

        IFS=',' read -r -a file <<< "${ArtInFile}"
        file=${file[0]}
        x="../$file"

        
        max_events=1000
        skip_events=$((${max_events}*${ArtProcess}))
        echo "Skipping ${skip_events} events, processing ${max_events} events"
        
	echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC}"
	unset  ATHENA_NUM_PROC
	
        # Output files to be saved
        hits_file=HITS_${ArtProcess}.pool.root
        aod_file=AOD_${ArtProcess}.pool.root
        ntup_file=physval_g4ms_${ArtProcess}.root

        # Sim & Reco step
        Sim_tf.py --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
                  --physicsList 'FTFP_BERT_ATL' --truthStrategy 'MC15aPlus' \
                  --simulator 'ATLFASTIIF_G4MS' \
                  --postInclude 'default:PyJobTransforms/UseFrontier.py' 'EVNTtoHITS:G4AtlasTests/postInclude.DCubeTest.py' \
                  --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
                  --preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
                  --DataRunNumber '284500' --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
                  --inputEVNTFile=${x} \
                  --outputHITSFile=${hits_file} \
                  --maxEvents ${max_events} --skipEvents ${skip_events} \
                  --randomSeed=1234 --imf False
        rc=$?
	echo  "art-result: $rc Sim_${ArtProcess}"
        if [ $rc -eq 0 ]
        then
            Reco_tf.py --autoConfiguration 'everything' \
                  --inputHITSFile=${hits_file} --conditionsTag 'default:OFLCOND-MC16-SDR-25' \
                  --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
                  --postExec "all:CfgMgr.MessageSvc().setError+=['HepMcParticleLink']" \
                  --postInclude "default:PyJobTransforms/UseFrontier.py" \
                  --preExec "all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)" 'HITtoRDO:userRunLumiOverride={"run":310000,"lb":61,"starttstamp":1550003600,"mu":60.500};' "from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);rec.doCalo=True;rec.doForwardDet=False; rec.doInDet=True;" 'RAWtoESD:from EventTagRawAlgs.EventTagRawAlgsConf import RawInfoSummaryForTagWriter;RawInfoSummaryForTagWriter.BCM_RDOKey=""' \
                  --outputAODFile=${aod_file} \
                  --valid=True --validationFlags 'doInDet,doMET,doMuon,doZee,doJet' \
                  --outputNTUP_PHYSVALFile=${ntup_file} \
                  --maxEvents -1 --skipEvents 0 --imf False
            rc2=$?
	    echo  "art-result: $rc2 Reco_${ArtProcess}"
            # if no problem, remove all files except the output files
	fi
        # for i in `ls | grep -v "${aod_file}\|${hits_file}\|${ntup_file}"` ; do rm -vrf $i; done
        for i in `ls | grep -v "${aod_file}\|${ntup_file}"` ; do rm -vrf $i; done
        

	;;
esac

