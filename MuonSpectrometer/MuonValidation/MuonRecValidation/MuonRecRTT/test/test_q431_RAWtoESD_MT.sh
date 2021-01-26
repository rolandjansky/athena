#!/bin/sh
#
# art-description: run the RAWtoESD transform of plain q431 with different number of threads and compare the outputs
#
# art-type: grid
# art-include: master/Athena
# art-output: OUT_ESD.root
# art-output: OUT_ESD_1thread.root
# art-output: OUT_ESD_5thread.root
# art-output: OUT_ESD_8thread.root
# art-output: diff_1_vs_serial.txt
# art-output: diff_5_vs_1.txt
# art-output: diff_8_vs_1.txt

#####################################################################
LOG_RECO="log_q431_RAWtoESD_serial.log"
Reco_tf.py --AMI q431 \
           --imf False \
           --outputESDFile OUT_ESD.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run reconstruction with AthenaMT with 1 thread
LOG_RECO="log_q431_RAWtoESD_1thread.log"
Reco_tf.py --AMI q431 \
           --imf False \
           --athenaopts="--threads=1" \
           --outputESDFile OUT_ESD_1thread.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf_1thread.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run reconstruction with AthenaMT with 5 threads
LOG_RECO="log_q431_RAWtoESD_5thread.log"
Reco_tf.py --AMI q431 \
           --imf False \
           --athenaopts="--threads=5" \
           --outputESDFile OUT_ESD_5thread.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf_5thread.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run reconstruction with AthenaMT with 8 threads
LOG_RECO="log_q431_RAWtoESD_8thread.log"
Reco_tf.py --AMI q431 \
           --imf False \
           --athenaopts="--threads=8" \
           --outputESDFile OUT_ESD_8thread.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf_8thread.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run diff-root to compare the ESDs made with serial and 1thread
acmd.py diff-root --ignore-leaves InDet::PixelClusterContainer_p3_PixelClusters HLT::HLTResult_p1_HLTResult_HLT.m_navigationResult xAOD::BTaggingAuxContainer_v1_BTagging_AntiKt4EMTopoAuxDyn xAOD::TrigDecisionAuxInfo_v1_xTrigDecisionAux xAOD::TrigPassBitsAuxContainer_v1_HLT_xAOD__TrigPassBitsContainer_passbitsAux xAOD::TrigNavigationAuxInfo_v1_TrigNavigationAux InDet::SCT_ClusterContainer_p3_SCT_Clusters index_ref --order-trees OUT_ESD_1thread.root OUT_ESD.root &> diff_1_vs_serial.txt
exit_code=$?
echo  "art-result: ${exit_code} diff-root"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################
# now run diff-root to compare the ESDs made with 5threads and 1thread
acmd.py diff-root --ignore-leaves InDet::PixelClusterContainer_p3_PixelClusters HLT::HLTResult_p1_HLTResult_HLT.m_navigationResult xAOD::BTaggingAuxContainer_v1_BTagging_AntiKt4EMTopoAuxDyn xAOD::TrigDecisionAuxInfo_v1_xTrigDecisionAux xAOD::TrigPassBitsAuxContainer_v1_HLT_xAOD__TrigPassBitsContainer_passbitsAux xAOD::TrigNavigationAuxInfo_v1_TrigNavigationAux InDet::SCT_ClusterContainer_p3_SCT_Clusters index_ref --order-trees OUT_ESD_5thread.root OUT_ESD_1thread.root &> diff_5_vs_1.txt
exit_code=$?
echo  "art-result: ${exit_code} diff-root_5thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################
# now run diff-root to compare the ESDs made with 8threads and 1thread
acmd.py diff-root --ignore-leaves InDet::PixelClusterContainer_p3_PixelClusters HLT::HLTResult_p1_HLTResult_HLT.m_navigationResult xAOD::BTaggingAuxContainer_v1_BTagging_AntiKt4EMTopoAuxDyn xAOD::TrigDecisionAuxInfo_v1_xTrigDecisionAux xAOD::TrigPassBitsAuxContainer_v1_HLT_xAOD__TrigPassBitsContainer_passbitsAux xAOD::TrigNavigationAuxInfo_v1_TrigNavigationAux InDet::SCT_ClusterContainer_p3_SCT_Clusters index_ref --order-trees OUT_ESD_8thread.root OUT_ESD_1thread.root &> diff_8_vs_1.txt
exit_code=$?
echo  "art-result: ${exit_code} diff-root_8thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

echo "art-result: $?"
