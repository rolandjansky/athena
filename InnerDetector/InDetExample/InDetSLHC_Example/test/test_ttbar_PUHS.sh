#!/bin/bash
# art-description: ttbar sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

# Following specify which steps to run.

dorec=1    # Reco_tf.py not yet working
dophy=1    # If dorec=0, set dophy=1 to run InDetPhysValMonitoring over old ESD

# Following specify DCube output directories. Set empty to disable.
dcube_sim_fixref="dcube_alt_sim"
dcube_sim_lastref="dcube_alt_sim_last"
dcube_rec_fixref="dcube_alt"
dcube_rec_lastref="dcube_alt_last"
dcube_HS_rec_fixref="dcube_HS_alt"
dcube_HS_rec_lastref="dcube_HS_alt_last"
dcube_PU_rec_fixref="dcube_PU_alt"
dcube_PU_rec_lastref="dcube_PU_alt_last"

artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
#artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="ttbar"
script="`basename \"$0\"`"

#MWMW rdo=/eos/home-w/wolter/public/pileup_data/mc15_14TeV.600012.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.RDO.e8185_s3654_s3657_r12441_tid24902026_00/RDO.24902026._000002.pool.root.1
rdo=${artdata}/InDetSLHC_Example/inputs/ttbar_mu_200_RDO.pool.root

# set clustering type to analogue
clustering_type='InDetSLHC_Example/postInclude.AnalogueClustering.py'
# clustering_type='InDetSLHC_Example/postInclude.DigitalClustering.py'


if [ $dorec -ne 0 ]; then
  esd=physval_alt.ESD.root
  daod=physval_alt.DAOD_IDTRKVALID.root
else
   echo "Reco job not configured to run... no input available for validation step, exiting test!"
  exit
fi
#jo=$artdata/InDetSLHC_Example/jobOptions/PhysValITk_jobOptions.py moved to share/
dcubemon_sim=SiHitValid_alt.root
dcubemon_rec=physval_alt.root
dcubemon_HS_rec=physval_alt_HS.root
dcubemon_PU_rec=physval_alt_PU.root
dcubecfg_sim=$artdata/InDetSLHC_Example/dcube/config/ttbar_SiHitValid.xml

dcubecfg_rec=$artdata/InDetSLHC_Example/dcube/config/ttbar_physval_PUHS.xml
dcuberef_sim=$artdata/InDetSLHC_Example/ReferenceHistograms/ttbar_SiHitValid.root
### Use reference data

dcuberef_HS_rec=${artdata}/InDetSLHC_Example/ReferenceHistograms/ttbar_physval_HS.root
dcuberef_PU_rec=${artdata}/InDetSLHC_Example/ReferenceHistograms/ttbar_physval_PU.root
dcuberef_rec=${artdata}/InDetSLHC_Example/ReferenceHistograms/ttbar_physval_All.root

art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube
lastref_dir=last_results

if [ \( $dosim -ne 0 -a -n "$dcube_sim_lastref" \) -o \( $dophy -ne 0 -a -n "$dcube_rec_lastref" \) ]; then
  run art.py download --user=artprod --dst="$lastref_dir" InDetSLHC_Example "$script"
  run ls -la "$lastref_dir"
fi

dcube() {
  # Run DCube and print art-result (if $2 is not empty)
  step="$1" statname="$2" dcubemon="$3" dcubecfg="$4" dcuberef="$5" dcubedir="$6"
  test -n "$dcubedir" || return
  run ls -lLU "$art_dcube" "$dcubemon" "$dcubecfg" "$dcuberef"
  if [ ! -s "$dcubemon" ]; then
    echo "$script: $step output '$dcubemon' not created. Don't create $dcubedir output." 2>&1
    test -n "$statname" && echo "art-result: 20 $statname"
    return
  fi
  if [ ! -s "$dcuberef" ]; then
    echo "$script: $step DCube reference '$dcuberef' not available. Don't create $dcubedir output." 2>&1
    test -n "$statname" && echo "art-result: 21 $statname"
    return
  fi
  echo "$script: DCube comparison of $step output '$dcubemon' with '$dcuberef' reference"
  keep=""
  test "$dcubedir" != "dcube" -a -d "dcube" && keep="dcube_keep_`uuidgen`"
  test -n "$keep" && run mv -f dcube "$keep"
  run "$art_dcube" "$name" "$dcubemon" "$dcubecfg" "$dcuberef"
  dcube_stat=$?
  test -n "$statname" && echo "art-result: $dcube_stat $statname"
  test "$dcubedir" != "dcube" && run mv -f dcube "$dcubedir"
  test -n "$keep" && run mv -f "$keep" dcube
}



if [ $dorec -ne 0 ]; then

  if [ $dosim -ne 0 ] && [ ! -s "$hits" ]; then
    echo "$script: Sim_tf output '$hits_muons_1GeV' not created. Not running Reco_tf and stopping" 2>&1
    exit 
  fi

  run ls -lL "$hits"

  # Reco step: r12520 for geometry 23-00-03.
  

  run Reco_tf.py \
    --inputRDOFile     "$rdo" \
    --maxEvents        20 \
    --AMI              r12520 \
    --outputESDFile    "$esd"


    
    
  reco_stat=$?
  echo "art-result: $reco_stat reco"
  if [ "$reco_stat" -ne 0 ]; then
    echo "$script: Reco_tf.py isn't working yet. Remove jobReport.json to prevent pilot declaring a failed job."
    run rm -f jobReport.json
  fi

fi


if [ $dophy -ne 0 ]; then

  # dcube test for All (HardScattered and PileUp) tracks
  run ls -lLU "$esd"

  if [ ! -s "$esd" ]; then
    echo "$script: Reco_tf output '$esd' not created - exit" 2>&1
    echo "art-result: 21 physval"
#   test -n "$dcube_HS_rec_fixref"  && echo "art-result: 22 plot-fixref"
    test -n "$dcube_HS_rec_lastref" && echo "art-result: 22 plot"
    exit
  fi

  # Run InDetPhysValMonitoring on ESD.
  # It should eventually be possible to include this in the reco step, but this needs Reco_tf to support the ITk IDPVM setup.
  ( set -x
    
    inputDAOD_IDTRKVALIDFile="$esd" exec athena.py InDetPhysValMonitoring/PhysValITk_jobOptions.py  -c "INFILE='$esd'"
  )
  echo "art-result: $? physval"

  mv ./MyPhysVal.root ./$dcubemon_rec

  # DCube InDetPhysValMonitoring performance plots
  dcube InDetPhysValMonitoring plot "$dcubemon_rec" "$dcubecfg_rec" "$lastref_dir/$dcubemon_rec" "$dcube_rec_lastref"
  dcube InDetPhysValMonitoring ""   "$dcubemon_rec" "$dcubecfg_rec"              "$dcuberef_rec" "$dcube_rec_fixref"



  # dcube test for HardScattered  tracks
  run ls -lLU "$esd"

  if [ ! -s "$esd" ]; then
    echo "$script: Reco_tf output '$esd' not created - exit" 2>&1
    echo "art-result: 21 physval"
#   test -n "$dcube_HS_rec_fixref"  && echo "art-result: 22 plot-fixref"
    test -n "$dcube_HS_rec_lastref" && echo "art-result: 22 plot"
    exit
  fi

  # Run InDetPhysValMonitoring on ESD.
  # It should eventually be possible to include this in the reco step, but this needs Reco_tf to support the ITk IDPVM setup.
  ( set -x
    

    inputDAOD_IDTRKVALIDFile="$esd" exec athena.py InDetPhysValMonitoring/PhysValITk_jobOptions_HS.py  -c "INFILE='$esd'"
  )
  echo "art-result: $? physval"

  mv ./MyPhysVal.root ./$dcubemon_HS_rec

  # DCube InDetPhysValMonitoring performance plots
  dcube InDetPhysValMonitoring plot "$dcubemon_HS_rec" "$dcubecfg_rec" "$lastref_dir/$dcubemon_HS_rec" "$dcube_HS_rec_lastref"
  dcube InDetPhysValMonitoring ""   "$dcubemon_HS_rec" "$dcubecfg_rec"              "$dcuberef_HS_rec" "$dcube_HS_rec_fixref"





  # dcube test for PileUp tracks
  run ls -lLU "$esd"

  if [ ! -s "$esd" ]; then
    echo "$script: Reco_tf output '$esd' not created - exit" 2>&1
    echo "art-result: 21 physval"
#   test -n "$dcube_PU_rec_fixref"  && echo "art-result: 22 plot-fixref"
    test -n "$dcube_PU_rec_lastref" && echo "art-result: 22 plot"
    exit
  fi

  # Run InDetPhysValMonitoring on ESD.
  # It should eventually be possible to include this in the reco step, but this needs Reco_tf to support the ITk IDPVM setup.
  ( set -x
    

    inputDAOD_IDTRKVALIDFile="$esd" exec athena.py InDetPhysValMonitoring/PhysValITk_jobOptions_PU.py  -c "INFILE='$esd'"
  )
  echo "art-result: $? physval"

  mv ./MyPhysVal.root ./$dcubemon_PU_rec

  # DCube InDetPhysValMonitoring performance plots
  dcube InDetPhysValMonitoring plot "$dcubemon_PU_rec" "$dcubecfg_rec" "$lastref_dir/$dcubemon_PU_rec" "$dcube_PU_rec_lastref"
  dcube InDetPhysValMonitoring ""   "$dcubemon_PU_rec" "$dcubecfg_rec"              "$dcuberef_PU_rec" "$dcube_PU_rec_fixref"

fi
