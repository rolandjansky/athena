#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_JPsi_no_pileup
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
#
# art-input: user.artprod.user.zhidong.valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.RDO.recon.no_pileup.e5112_s3195_tid12019932_01_EXT0
# art-input-nfiles: 6
# art-output: art_core_*
# art-cores: 4

set -x


echo $AtlasVersion
if [ ${AtlasVersion:0:4} == "22.0" ]; then
    ValOption=""
else
    ValOption="--outputNTUP_PHYSVALFile valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4_no_pileup.PHYSVAL.root --validationFlags noExample doMuon"
fi
echo "ValOption=", $ValOption


echo "ArtProcess: $ArtProcess"

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

     Reco_tf.py --maxEvents=-1 --inputRDOFile=${infile} --outputAODFile=valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4_no_pileup.AOD.pool.root $ValOption  --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'

     echo  "art-result: $? reco_${ArtProcess}"
     ls -lR
     rm -f tmp.ESD
     rm -f core.*
     ;;
esac
