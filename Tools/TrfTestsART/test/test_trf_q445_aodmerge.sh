#!/bin/bash
#
# art-description: Reco_tf.py q445, AODMerge and output comparison
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena
# art-athena-mt: 8

export ATHENA_CORE_NUMBER=8
Reco_tf.py \
    --AMI q445 \
    --maxEvents 100 \
    --multithreaded="True" \
    --outputAODFile myAOD.MT.pool.root
rc1=$?
AODMerge_tf.py \
    --autoConfiguration 'everything' \
    --fastPoolMerge 'False' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --inputAODFile myAOD.MT.pool.root \
    --outputAODFile Merged.MT.AOD.pool.root \
    --multithreaded="True"
rc2=$?

# Compare EventData in the merged files
rc3=1
rc4=1
if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq 0 ]]; then
  # EventData
  acmd diff-root --nan-equal --order-trees myAOD.MT.pool.root Merged.MT.AOD.pool.root
  rc3=$?
  meta-diff -x diff -s myAOD.MT.pool.root Merged.MT.AOD.pool.root -d file_guid file_size
  rc4=$?
fi

# Print the results
echo "art-result: ${rc1} q445"
echo "art-result: ${rc2} AODMerge"
echo "art-result: ${rc3} Comparison:EventData"
echo "art-result: ${rc4} Comparison:MetaData"