#!/bin/sh
#
# art-description: Test the reconstruction of muon samples for toroid off.
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-input: user.zhidong.data17_13TeV.00325028.physics_Main.daq.RAW_subset01
# art-input-nfiles: 8
# art-output: art_core_*
# art-cores: 4

set -x

case $ArtProcess in

  "start")
     echo "Starting"
     ;;
     
  "end")
     echo "Ending"
     ;;
     
  *)
     echo "Test $ArtProcess"
     mkdir "art_core_${ArtProcess}"
     cd "art_core_${ArtProcess}"
     IFS=',' read -r -a file <<< "${ArtInFile}"
     infile="../${file[${ArtProcess}]}"
     echo "Processing files: " ${infile}
     unset  ATHENA_NUM_PROC
     unset  ATHENA_PROC_NUMBER


     Reco_tf.py --maxEvents=-1 --conditionsTag RAWtoESD:CONDBR2-BLKPA-2017-12 --geometryVersion ATLAS-R2-2016-01-00-01 --inputBSFile=${infile} --outputAODFile=MuonToroidOff_Reco.AOD.pool.root --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup' --postExec 'r2e:topSequence.BeamBackgroundFiller.Enable = False'

     echo  "art-result: $? reco_${ArtProcess}"
     ls -lR
     rm -f tmp.ESD
     rm -f core.*
     ;;
esac