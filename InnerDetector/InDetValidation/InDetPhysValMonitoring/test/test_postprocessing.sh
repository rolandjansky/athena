#!/bin/bash
# art-description: InclinedDuals sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*
# art-output: report

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
#artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="InclinedDuals"
script="`basename \"$0\"`"

individual_ESD_1=$artdata/InDetSLHC_Example/inputs/physval_muons_1GeV.09244569._000001.ESD.root
individual_ESD_2=$artdata/InDetSLHC_Example/inputs/physval_muons_1GeV.09244569._000002.ESD.root
#combined_ESD=$artdata/InDetSLHC_Example/inputs/physval_muons_1GeV.ESD.root
combined_ESD=$individual_ESD_1,$individual_ESD_2

dcubemon_postprocessing=physval_muons_1GeV.root
dcubecfg_postprocessing=$artdata/InDetSLHC_Example/dcube/config/postprocessing_physval.xml
dcuberef_postprocessing=physval_muons_1GeV_ref.root
dcubeoutput_postprocessing=dcube_postprocessing
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube

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
  run "$art_dcube" "$name" "$dcubemon" "$dcubecfg" "$dcuberef"
  dcube_stat=$?
  test -n "$statname" && echo "art-result: $dcube_stat $statname"
  test "$dcubedir" != "dcube" && run mv -f dcube "$dcubedir"
}

# Running IDPVM on first ESD file
( set -x
  exec athena.py InDetPhysValMonitoring/PhysValITk_jobOptions.py --filesInput="$individual_ESD_1"
)
echo "art-result: $? physval_1"

mv ./MyPhysVal.root ./physval_1.root

# Running IDPVM on second ESD file
( set -x
  exec athena.py InDetPhysValMonitoring/PhysValITk_jobOptions.py --filesInput="$individual_ESD_2"
)
echo "art-result: $? physval_2"

mv ./MyPhysVal.root ./physval_2.root

# Combining two IDPVM outputs using postprocessing
( set -x
  hadd ./physval_hadd.root ./physval_1.root ./physval_2.root
  cp $artdata/InDetSLHC_Example/scripts/postprocessHistos .
  chmod a+x ./postprocessHistos  
  ./postprocessHistos ./physval_hadd.root
)

mv ./physval_hadd.root ./$dcubemon_postprocessing

# Running IDPVM on the combined ESD file
( set -x
  exec athena.py InDetPhysValMonitoring/PhysValITk_jobOptions.py --filesInput="$combined_ESD"
)
echo "art-result: $? physval_comb"
  
mv ./MyPhysVal.root ./$dcuberef_postprocessing
  
# DCube InDetPhysValMonitoring performance plots
# dcube InDetPhysValMonitoring "" "$dcubemon_postprocessing" "$dcubecfg_postprocessing" "$dcuberef_postprocessing" "$dcubeoutput_postprocessing"

# Generate test report with custom python script
python $artdata/InDetSLHC_Example/scripts/compareIDPVoutputs_ratio.py ./$dcuberef_postprocessing ./$dcubemon_postprocessing
