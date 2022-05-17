#!/bin/bash
#
# art-description: Testing AODMerge_tf.py in the old/new-style job configurations 
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena

# Run the old-style merge
AODMerge_tf.py \
  --maxEvents 25 \
  --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc20_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3681_r13167/AOD.27312826._000061.pool.root.1 \
  --outputAOD_MRGFile std.aod.pool.root
rc1=$?

# Run the CA-based merge
AODMerge_tf.py \
  --CA \
  --maxEvents 25 \
  --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc20_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3681_r13167/AOD.27312826._000061.pool.root.1 \
  --outputAOD_MRGFile ca.aod.pool.root
rc2=$?

# Compare both MetaData and EventData in the merged files
rc3=1
rc4=1
if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq 0 ]]; then
  # MetaData
  meta-diff -x diff -s std.aod.pool.root ca.aod.pool.root -d file_guid file_size
  rc3=$?
  # EventData
  acmd diff-root --nan-equal std.aod.pool.root ca.aod.pool.root
  rc4=$?
fi

# Print the results
echo "art-result: ${rc1} AODMerge:STD"
echo "art-result: ${rc2} AODMerge:CA"
echo "art-result: ${rc3} Comparison:MetaData"
echo "art-result: ${rc4} Comparison:EventData"
