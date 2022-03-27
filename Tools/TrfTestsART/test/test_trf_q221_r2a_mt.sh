#!/bin/bash
#
# art-description: Reco_tf.py q221 HITStoRDO and RAWtoALL/TRIGtoALL in MT mode, RDOtoRDOTrigger in 21.0
# art-type: grid
# art-athena-mt: 8

Reco_tf.py \
--multithreaded='True' \
--AMI=q221 \
--conditionsTag "default:OFLCOND-MC16-SDR-RUN2-09" "RDOtoRDOTrigger:OFLCOND-MC16-SDR-RUN2-08-02" \
--steering "doRDO_TRIG" "doTRIGtoALL" \
--triggerConfig "RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2233,87,314" \
--asetup "RDOtoRDOTrigger:Athena,21.0,latest" \
--imf="False" \
--postExec 'FPEAuditor.NStacktracesOnFPE=10' \
--maxEvents 1000

rc1=$?
echo "art-result: ${rc1} Reco_tf_q221_r2a_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"
