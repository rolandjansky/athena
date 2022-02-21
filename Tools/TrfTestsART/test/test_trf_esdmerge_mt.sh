#!/bin/bash
#
# art-description: ESDMerge_tf.py mt
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

ESDMerge_tf.py \
    --multithreaded="True" \
    --inputESDFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/DESDM_MCP.26614755._001203.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/DESDM_MCP.26614755._001208.pool.root.1 \
    --postInclude="all:PyJobTransforms/UseFrontier.py" \
    --autoConfiguration="everything" \
    --conditionsTag="all:CONDBR2-BLKPA-RUN2-09" \
    --geometryVersion="all:ATLAS-R2-2016-01-00-01" \
    --runNumber="358031" \
    --outputESD_MRGFile="DESDM_MCP.pool.root" \
    --postExec 'FPEAuditor.NStacktracesOnFPE=10' \
    --AMITag="p4756" 

rc1=$?
echo "art-result: ${rc1} ESDMerge_tf_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"
