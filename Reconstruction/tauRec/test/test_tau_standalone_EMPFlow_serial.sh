#!/bin/bash
#
# art-description: standalone tau reconstruction on MC in serial mode
# art-type: grid
# art-include: master/Athena
# art-output: *.root
# art-output: *.log
# art-output: *.ps
# art-output: dcube
# art-html: dcube

NEVENTS=1000
REF_DIR="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/tauRec/reference/standalone"

# run the reconstruction
athena.py --evtMax ${NEVENTS}  tauRec/tau_standalone_EMPFlow_ESDtoAOD.py >> tau_standalone.log 2>&1
echo "art-result: $? Reconstrution"

# compare the AOD file
art.py compare ref --entries ${NEVENTS} --mode detailed --order-trees --diff-root EMPFlow_AOD.pool.root ${REF_DIR}/EMPFlow_AOD.pool.root >> AOD_diff_root.log 2>&1
echo "art-result: $? diff-root"

# run the physics validation
Reco_tf.py --maxEvents ${NEVENTS} --validationFlags 'noExample,doTau' --inputAODFile EMPFlow_AOD.pool.root  --outputNTUP_PHYSVALFile EMPFlow_NTUP_PHYSVAL.root
echo "art-result: $? PhysVal"

# compare the histograms
rootcomp.py EMPFlow_NTUP_PHYSVAL.root ${REF_DIR}/EMPFlow_NTUP_PHYSVAL.root >> rootcomp.log 2>&1
echo "art-result: $? rootcomp"

# run dcube
INPUT_DIR="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/tauRec/input"
$ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    --plot --output dcube \
    --config ${INPUT_DIR}/config_mc.xml \
    --reference ${REF_DIR}/EMPFlow_NTUP_PHYSVAL.root \
    EMPFlow_NTUP_PHYSVAL.root
echo "art-result: $? dcube"
