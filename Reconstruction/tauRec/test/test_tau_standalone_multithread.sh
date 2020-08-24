#!/bin/bash
#
# art-description: standalone tau reconstruction ART on MC in multithread mode
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena
# art-output: *.root
# art-output: *.log
# art-output: *.ps
# art-output: dcube
# art-html: dcube

NEVENTS=1000
REF_DIR="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/tauRec/reference/standalone"

# run the reconstruction
athena.py --threads 4 --evtMax ${NEVENTS}  tauRec/tau_standalone_ESDtoAOD.py >> tau_standalone.log 2>&1
echo "art-result: $? Reconstrution"

# compare the AOD file
art.py compare ref --entries ${NEVENTS} --mode semi-detailed --excluded-vars=mc_event_number --order-trees --diff-root AOD.pool.root ${REF_DIR}/AOD.pool.root >> AOD_diff_root.log 2>&1
echo "art-result: $? diff-root"

# run the physics validation
Reco_tf.py --maxEvents ${NEVENTS} --validationFlags 'noExample,doTau' --inputAODFile AOD.pool.root  --outputNTUP_PHYSVALFile NTUP_PHYSVAL.root
echo "art-result: $? PhysVal"

# compare the histograms
rootcomp.py NTUP_PHYSVAL.root ${REF_DIR}/NTUP_PHYSVAL.root >> rootcomp.log 2>&1
echo "art-result: $? rootcomp"

# run dcube
INPUT_DIR="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/tauRec/input"
$ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    --plot --output dcube \
    --config ${INPUT_DIR}/config_mc.xml \
    --reference ${REF_DIR}/NTUP_PHYSVAL.root \
    NTUP_PHYSVAL.root
echo "art-result: $? dcube"
