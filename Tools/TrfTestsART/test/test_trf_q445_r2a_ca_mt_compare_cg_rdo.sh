#!/bin/bash
#
# art-description: Reco_tf.py q445 RAWtoALL in MT and ComponentAccumulator mode with RDO input
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

mkdir ca
cd ca
Reco_tf.py --CA \
  --AMI q445  \
  --multithreaded="True" \
  --steering "doRAWtoALL" \
  --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/dev/x86_64-centos7-gcc11-opt/mc21a_ttbar.CG.RDO.pool.root \
  --outputAODFile myAOD_ca.pool.root \
  --outputESDFile myESD_ca.pool.root \
  --preExec "all:from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Jet.WriteToAOD=True; ConfigFlags.MET.WritetoAOD=True" \
  --imf="False" \
  --maxEvents 100

rc1=$?
echo "art-result: ${rc1} Reco_tf_q445_r2a_ca_mt_cg_1"

cd ..
mkdir def
cd def
Reco_tf.py \
  --AMI q445  \
  --multithreaded="True" \
  --steering "doRAWtoALL" \
  --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/dev/x86_64-centos7-gcc11-opt/mc21a_ttbar.CG.RDO.pool.root \
  --outputAODFile myAOD_def.pool.root \
  --outputESDFile myESD_def.pool.root \
  --preExec "all:from JetRec.JetRecFlags import jetFlags; jetFlags.writeJetsToAOD.set_Value_and_Lock(True); from METReconstruction.METRecoFlags import metFlags; metFlags.WriteMETAssocToOutput.set_Value_and_Lock(True);  metFlags.WriteMETToOutput.set_Value_and_Lock(True);" \
  --imf="False" \
  --maxEvents 100

rc2=$?
echo "art-result: ${rc2} Reco_tf_q445_r2a_mt_cg_2"

cd ..

# Check for FPEs in the logiles
# test_trf_check_fpe.sh      # currently disabled since FPEAuditor does not work in CA mode
#fpeStat=$?

#echo "art-result: ${fpeStat} FPEs in logfiles"

echo "============ checkxAOD myAOD_ca.pool.root"
checkxAOD ca/myAOD_ca.pool.root
rc3=$?
echo "art-result: ${rc3} checkxAOD myAOD_ca.pool.root"

echo "============ checkxAOD myAOD_def.pool.root"
checkxAOD def/myAOD_def.pool.root
rc4=$?
echo "art-result: ${rc4} checkxAOD myAOD_def.pool.root"

echo "============ xAODDigest.py --extravars myAOD_ca.pool.root"
xAODDigest.py --extravars ca/myAOD_ca.pool.root myAOD_ca.txt
rc5=$?
echo "art-result: ${rc5} xAODDigest.py --extravars myAOD_ca.pool.root"
echo "============ myAOD_ca.txt"
cat myAOD_ca.txt
echo "============ myAOD_ca.txt"

echo "============ xAODDigest.py --extravars myAOD_def.pool.root"
xAODDigest.py --extravars def/myAOD_def.pool.root myAOD_def.txt
rc6=$?
echo "art-result: ${rc6} xAODDigest.py --extravars myAOD_def.pool.root"
echo "============ myAOD_def.txt"
cat myAOD_def.txt
echo "============ myAOD_def.txt"

echo "============ comparexAODDigest.py myAOD_def.txt myAOD_ca.txt"
comparexAODDigest.py myAOD_def.txt myAOD_ca.txt
rc7=$?
echo "art-result: ${rc7} comparexAODDigest.py myAOD_def.txt myAOD_ca.txt"
