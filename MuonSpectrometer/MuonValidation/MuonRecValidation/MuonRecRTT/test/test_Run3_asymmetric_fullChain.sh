#!/bin/sh
#
# art-description: Test the asymmetric Run3 layout in simulation/digitisation/reconstruction
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-output: OUT_HITS.root
# art-output: OUT_RDO.root
# art-output: NSWPRDValAlg.digi.ntuple.root
# art-output: NSWDigiCheck.txt
# art-output: OUT_ESD.root
# art-output: NSWPRDValAlg.reco.ntuple.root
# art-output: NSWRecoCheck.txt

#####################################################################
# run simulation on 25 events using the asymmetric Run3 layout
LOG_SIM="log_Run3_asymmetric_sim.log"
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R3-2021-01-00-00_VALIDATION' \
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
#####################################################################

#####################################################################
# now use the produced HITS file and run digitisation
# (since the 21.X and master branches use a different Geant4 version, we use the HITS file produced in 21.X
# to avoid tiny differences in the number of secondary particles and hit positions and start from the same HITS file)
# the postInclude adds a validation algorithm which writes out an ntuple for digit/RDO validation
# (without the postInclude, a standard digitisation job would run)
LOG_DIGI="log_Run3_asymmetric_digi.log"
Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run3/HITS/AsymmetricLayout_HITS_v1.root \
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
           --preExec "from MuonRecExample.MuonRecFlags import muonRecFlags;muonRecFlags.setDefaults();muonRecFlags.doFastDigitization=False;muonRecFlags.useLooseErrorTuning.set_Value_and_Lock(True);from RecExConfig.RecFlags import rec;rec.doTrigger=False;rec.doEgamma=True;rec.doLucid=True;rec.doZdc=True;rec.doJetMissingETTag=True" \
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

echo "art-result: $?"
