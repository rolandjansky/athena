#!/bin/sh
#
# art-description: Test the symmetric Run3 layout in simulation/digitisation/reconstruction
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-output: OUT_HITS.root
# art-output: log_diff_HITS.log
# art-output: OUT_RDO.root
# art-output: NSWPRDValAlg.digi.ntuple.root
# art-output: NSWDigiCheck.txt
# art-output: OUT_ESD.root
# art-output: NSWPRDValAlg.reco.ntuple.root
# art-output: NSWRecoCheck.txt

#####################################################################
# run simulation on 25 events using the symmetric Run3 layout
LOG_SIM="log_Run3_symmetric_sim.log"
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R3S-2021-01-00-02_VALIDATION' \
          --AMI=s3512 \
          --maxEvents 25 \
          --imf False \
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
# check differences wrt reference HITS file
acmd.py diff-root --ignore-leaves timings --mode semi-detailed --error-mode resilient OUT_HITS.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run3/HITS/SymmetricLayout_HITS_v2.root &> log_diff_HITS.log
exit_code=$?
echo  "art-result: ${exit_code} diff-root_sim"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now use the produced HITS file and run digitisation
# the postInclude adds a validation algorithm which writes out an ntuple for digit/RDO validation
# (without the postInclude, a standard digitisation job would run)
LOG_DIGI="log_Run3_symmetric_digi.log"
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
LOG_RECO="log_Run3_symmetric_reco.log"
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --imf False \
           --postInclude MuonPRDTest/NSWPRDValAlg.reco.py \
           --postExec 'conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3SYM-01");conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3SYM-01")' \
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

echo "art-result: $?"
