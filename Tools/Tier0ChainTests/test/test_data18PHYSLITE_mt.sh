#!/bin/sh

# art-description: DAOD building PHYSLITE data18 MT
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt
# art-athena-mt: 1

set -e

Reco_tf.py --athenaopts='--threads=1' --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00357750.physics_Main.merge.AOD.r12879_p4685/AOD.26343283._000019.pool.root.1 --outputDAODFile art.pool.root --reductionConf PHYSLITE --maxEvents -1 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff();'

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_PHYSLITEFile DAOD_PHYSLITE.art.pool.root --outputDAOD_PHYSLITE_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_PHYSLITE.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_PHYSLITE.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
