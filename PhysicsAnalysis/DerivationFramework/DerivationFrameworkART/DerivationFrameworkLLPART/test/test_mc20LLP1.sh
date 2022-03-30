#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building LLP1 mc20
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt
# art-output: checkIndexRefs.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc20_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3681_r13167/AOD.27312826._000061.pool.root.1 --outputDAODFile art.pool.root --reductionConf LLP1 --maxEvents -1 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); DetFlags.detdescr.pixel_setOn(); DetFlags.detdescr.SCT_setOn(); InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doR3LargeD0.set_Value_and_Lock(True);' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_LLP1File DAOD_LLP1.art.pool.root --outputDAOD_LLP1_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_LLP1.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_LLP1.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"

checkIndexRefs.py DAOD_LLP1.art.pool.root > checkIndexRefs.txt

echo "art-result: $?  checkIndexRefs"
