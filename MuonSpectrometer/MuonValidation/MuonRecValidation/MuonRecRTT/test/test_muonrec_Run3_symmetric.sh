#!/bin/sh
#
# art-description: Test the symmetric Run3 layout in reconstruction
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-output: trkPerformance_MuonSpectrometerTracks.txt
# art-output: trkPerformance_ExtrapolatedMuonTracks.txt
# art-output: trkPerformance_MSOnlyExtrapolatedMuonTracks.txt
# art-output: trkPerformance_CombinedMuonTracks.txt
# art-output: muonPerformance_segments.txt
# art-output: muonPerformance_xAOD.txt
# art-output: warningCount.txt
# art-output: NSWRecoCheck.txt
# art-output: NSWPRDValAlg.reco.ntuple.root

#####################################################################
# run reconstruction on 1000 di-muon events (1.3<|eta|<2.7) using the symmetric Run3 layout
LOG_RECO="log_Run3_symmetric_reco.log"
Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run3/RDO/SymmetricLayout_RDO_v1.root \
           --preExec "from MuonRecExample.MuonRecFlags import muonRecFlags;muonRecFlags.setDefaults();muonRecFlags.doFastDigitization=False;muonRecFlags.useLooseErrorTuning.set_Value_and_Lock(True);muonRecFlags.doTrackPerformance=True;muonRecFlags.TrackPerfSummaryLevel=2;muonRecFlags.TrackPerfDebugLevel=5;from RecExConfig.RecFlags import rec;rec.doTrigger=False;rec.doEgamma=True;rec.doLucid=True;rec.doZdc=True;rec.doJetMissingETTag=True;from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags;muonStandaloneFlags.printSummary=True;" \
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
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in ${LOG_RECO}" > warningCount.txt
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
