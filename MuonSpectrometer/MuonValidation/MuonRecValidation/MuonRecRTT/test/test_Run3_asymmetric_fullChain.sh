#!/bin/sh
#
# art-description: Test the asymmetric Run3 layout in simulation/digitisation/reconstruction
#
# art-type: grid
# art-include: master/Athena
# art-output: OUT_HITS.root
# art-output: NSWPRDValAlg.sim.ntuple.root
# art-output: OUT_RDO.root
# art-output: NSWPRDValAlg.digi.ntuple.root
# art-output: NSWDigiCheck.txt
# art-output: OUT_ESD.root
# art-output: OUT_ESD_1thread.root
# art-output: OUT_ESD_5thread.root
# art-output: NSWPRDValAlg.reco.ntuple.root
# art-output: NSWRecoCheck.txt
# art-output: diff_1_vs_serial.txt
# art-output: diff_5_vs_1.txt

#####################################################################
# run simulation on 100 events using the asymmetric Run3 layout
# the postInclude adds a validation algorithm which writes out an ntuple for sim hit validation
# (without the postInclude, a standard simulation job would run)
LOG_SIM="log_Run3_asymmetric_sim.log"
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R3-2021-01-00-01_VALIDATION' \
          --AMI=s3512 \
          --maxEvents 100 \
          --imf False \
          --postInclude MuonPRDTest/NSWPRDValAlg.sim.py \
          --outputHITSFile OUT_HITS.root &> ${LOG_SIM}
exit_code=$?
echo  "art-result: ${exit_code} Sim_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
# check the log file for WARNING/ERROR/FATAL
NWARNING="$(cat ${LOG_SIM} | grep WARNING | wc -l)"
NERROR="$(cat ${LOG_SIM} | grep ERROR | wc -l)"
NFATAL="$(cat ${LOG_SIM} | grep FATAL | wc -l)"
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in ${LOG_SIM}"
#####################################################################

#####################################################################
# now use the produced HITS file and run digitisation
# the postInclude adds a validation algorithm which writes out an ntuple for digit/RDO validation
# (without the postInclude, a standard digitisation job would run)
LOG_DIGI="log_Run3_asymmetric_digi.log"
Digi_tf.py --inputHITSFile OUT_HITS.root \
           --imf False \
           --postInclude MuonPRDTest/NSWPRDValAlg.digi.py \
           --outputRDOFile OUT_RDO.root &> ${LOG_DIGI}
exit_code=$?
echo  "art-result: ${exit_code} Digi_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
# check the log file for WARNING/ERROR/FATAL
NWARNING="$(cat ${LOG_DIGI} | grep WARNING | wc -l)"
NERROR="$(cat ${LOG_DIGI} | grep ERROR | wc -l)"
NFATAL="$(cat ${LOG_DIGI} | grep FATAL | wc -l)"
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in ${LOG_DIGI}"
#####################################################################
# check the NSW validation ntuple
python $Athena_DIR/bin/checkNSWValTree.py -i NSWPRDValAlg.digi.ntuple.root &> NSWDigiCheck.txt
exit_code=$?
echo  "art-result: ${exit_code} NSWDigiCheck"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now use the produced RDO file and run reconstruction
# the postInclude adds a validation algorithm which writes out an ntuple for digit/RDO/PRD validation
# (without the postInclude, a standard reconstruction job would run)
LOG_RECO="log_Run3_asymmetric_reco.log"
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --imf False \
           --postInclude MuonPRDTest/NSWPRDValAlg.reco.py \
           --outputESDFile OUT_ESD.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
# check the log file for WARNING/ERROR/FATAL
NWARNING="$(cat ${LOG_RECO} | grep WARNING | wc -l)"
NERROR="$(cat ${LOG_RECO} | grep ERROR | wc -l)"
NFATAL="$(cat ${LOG_RECO} | grep FATAL | wc -l)"
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in ${LOG_RECO}"
#####################################################################
# check the NSW validation ntuple
python $Athena_DIR/bin/checkNSWValTree.py -i NSWPRDValAlg.reco.ntuple.root --checkPRD &> NSWRecoCheck.txt
exit_code=$?
echo  "art-result: ${exit_code} NSWRecoCheck"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run reconstruction with AthenaMT with 1 thread
LOG_RECO="log_Run3_asymmetric_reco_1thread.log"
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --athenaopts="--threads=1" \
           --outputESDFile OUT_ESD_1thread.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py_1thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run reconstruction with AthenaMT with 5 threads
LOG_RECO="log_Run3_asymmetric_reco_5thread.log"
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --athenaopts="--threads=5" \
           --outputESDFile OUT_ESD_5thread.root &> ${LOG_RECO}
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py_5thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run diff-root to compare the ESDs made with serial and 1thread
acmd.py diff-root --ignore-leaves index_ref xAOD::BTaggingAuxContainer_v1_BTagging_AntiKt4EMTopoAuxDyn xAOD::CaloClusterAuxContainer_v2_ForwardElectronClustersAuxDyn --entries 25 --order-trees OUT_ESD_1thread.root OUT_ESD.root &> diff_1_vs_serial.txt
exit_code=$?
echo  "art-result: ${exit_code} diff-root"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now run diff-root to compare the ESDs made with 5threads and 1thread
acmd.py diff-root --ignore-leaves index_ref xAOD::BTaggingAuxContainer_v1_BTagging_AntiKt4EMTopoAuxDyn xAOD::CaloClusterAuxContainer_v2_ForwardElectronClustersAuxDyn --entries 25 --order-trees OUT_ESD_5thread.root OUT_ESD_1thread.root &> diff_5_vs_1.txt
exit_code=$?
echo  "art-result: ${exit_code} diff-root_5thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

echo "art-result: $?"
