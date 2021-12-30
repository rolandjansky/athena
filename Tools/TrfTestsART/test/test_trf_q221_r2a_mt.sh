#!/bin/bash
#
# art-description: Reco_tf.py q431 HITStoRDO and RAWtoALL/TRIGtoALL in MT mode, RDOtoRDOTrigger in 21.0
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

Reco_tf.py \
--multithreaded='True' \
--AMI=q221 \
--conditionsTag "default:OFLCOND-MC16-SDR-RUN2-09" "RDOtoRDOTrigger:OFLCOND-MC16-SDR-RUN2-08-02" \
--steering "doRDO_TRIG" "doTRIGtoALL" \
--triggerConfig "RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2233,87,314" \
--asetup "RDOtoRDOTrigger:Athena,21.0,latest" \
--imf="False" \
--maxEvents 1000

rc1=$?
echo "art-result: ${rc1} Reco_tf_q221_r2a_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc2=$?
fi
echo  "art-result: ${rc2} Comparison with the latest result"

rc3=-9999
if [ ${rc1} -eq 0 ]
then
# The comparison to fixed reference will be updated. Currently testing the machinery
#  art-diff.py . /eos/atlas/atlascerngroupdisk/data-art/grid-output/21.0/Athena/x86_64-slc6-gcc62-opt/2018-03-06T2154/Tier0ChainTests --entries 50 --mode=semi-detailed --order-trees  --diff-type=diff-root
  rc3=$?
fi
echo  "art-result: ${rc3} Comparison with fixed reference"
