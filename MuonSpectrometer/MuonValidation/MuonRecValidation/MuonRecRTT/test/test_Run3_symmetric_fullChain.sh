#!/bin/sh
#
# art-description: Test the symmetric Run3 layout in simulation/digitisation/reconstruction
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-output: OUT_HITS.root
# art-output: OUT_RDO.root
# art-output: OUT_ESD.root

#####################################################################
# run simulation on 25 events using the symmetric Run3 layout
LOG_SIM="log_Run3_symmetric_sim.log"
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R3S-2021-01-00-00_VALIDATION' \
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
acmd.py diff-root OUT_HITS.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run3/HITS/SymmetricLayout_HITS_v1.root --ignore-leaves timings &> log_diff_HITS.log
exit_code=$?
echo  "art-result: ${exit_code} diff-root_sim"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now use the produced HITS file and run digitisation
LOG_DIGI="log_Run3_symmetric_digi.log"
Digi_tf.py --inputHITSFile OUT_HITS.root \
           --imf False \
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

#####################################################################
# now use the produced RDO file and run reconstruction
LOG_RECO="log_Run3_symmetric_reco.log"
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --preExec "from MuonRecExample.MuonRecFlags import muonRecFlags;muonRecFlags.setDefaults();muonRecFlags.doFastDigitization=False;muonRecFlags.useLooseErrorTuning.set_Value_and_Lock(True);from RecExConfig.RecFlags import rec;rec.doTrigger=False;rec.doEgamma=True;rec.doLucid=True;rec.doZdc=True;rec.doJetMissingETTag=True" \
           --autoConfiguration everything \
           --imf False \
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

echo "art-result: $?"
