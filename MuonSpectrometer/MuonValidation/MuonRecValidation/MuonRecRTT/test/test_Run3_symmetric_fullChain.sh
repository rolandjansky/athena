#!/bin/sh
#
# art-description: Test the symmetric Run3 layout in simulation/digitisation/reconstruction
# 
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 5
# art-output: OUT_HITS.root
# art-output: NSWPRDValAlg.sim.ntuple.root
# art-output: NSWPRDValAlg.dcube.root
# art-output: dcube*
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
# art-output: log.RAWtoESD_serial
# art-output: log.RAWtoESD_1thread
# art-output: log.RAWtoESD_5thread

#####################################################################
# run simulation on 100 events using the symmetric Run3 layout
# the postInclude adds a validation algorithm which writes out an ntuple for sim hit validation
# (without the postInclude, a standard simulation job would run)
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R3S-2021-01-00-02_VALIDATION' \
          --AMI=s3512 \
          --maxEvents 100 \
          --imf False \
          --postInclude MuonPRDTest/NSWPRDValAlg.sim.py \
          --outputHITSFile OUT_HITS.root
exit_code=$?
echo  "art-result: ${exit_code} Sim_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
# check the log file for WARNING/ERROR/FATAL
NWARNING="$(cat log.EVNTtoHITS | grep WARNING | wc -l)"
NERROR="$(cat log.EVNTtoHITS | grep ERROR | wc -l)"
NFATAL="$(cat log.EVNTtoHITS | grep FATAL | wc -l)"
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in log.EVNTtoHITS"
#####################################################################

#####################################################################
# create histograms for dcube
python $Athena_DIR/bin/createDCubeHistograms.py --doMM --doSTGC
exit_code=$?
echo  "art-result: ${exit_code} DCubeSimHist"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# download last nightly's ART results to compare against
echo "download latest result"
art.py download --user=artprod --dst=lastResults "$ArtPackage" "$ArtJobName"
ls -l lastResults
$ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py -r lastResults/NSWPRDValAlg.dcube.root -t KS chi2 -c $Athena_DIR/XML/MuonPRDTest/dcube_config_simulation_symRun3.xml -x dcubeSimulation -p NSWPRDValAlg.dcube.root
exit_code=$?
echo  "art-result: ${exit_code} DCubeSim"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
#####################################################################

#####################################################################
# now use the produced HITS file and run digitisation
# the postInclude adds a validation algorithm which writes out an ntuple for digit/RDO validation
# (without the postInclude, a standard digitisation job would run)
Digi_tf.py --inputHITSFile OUT_HITS.root \
           --imf False \
           --postInclude MuonPRDTest/NSWPRDValAlg.digi.py \
           --outputRDOFile OUT_RDO.root
exit_code=$?
echo  "art-result: ${exit_code} Digi_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
# check the log file for WARNING/ERROR/FATAL
NWARNING="$(cat log.HITtoRDO | grep WARNING | wc -l)"
NERROR="$(cat log.HITtoRDO | grep ERROR | wc -l)"
NFATAL="$(cat log.HITtoRDO | grep FATAL | wc -l)"
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in log.HITtoRDO"
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
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --imf False \
           --postInclude MuonPRDTest/NSWPRDValAlg.reco.py \
           --postExec 'conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3SYM-01");conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3SYM-01")' \
           --outputESDFile OUT_ESD.root
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
# check the log file for WARNING/ERROR/FATAL
NWARNING="$(cat log.RAWtoESD | grep WARNING | wc -l)"
NERROR="$(cat log.RAWtoESD | grep ERROR | wc -l)"
NFATAL="$(cat log.RAWtoESD | grep FATAL | wc -l)"
echo "Found ${NWARNING} WARNING, ${NERROR} ERROR and ${NFATAL} FATAL messages in log.RAWtoESD"
mv log.RAWtoESD log.RAWtoESD_serial
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
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --athenaopts="--threads=1" \
           --postExec 'conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3SYM-01");conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3SYM-01")' \
           --outputESDFile OUT_ESD_1thread.root
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py_1thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
mv log.RAWtoESD log.RAWtoESD_1thread
#####################################################################

#####################################################################
# now run reconstruction with AthenaMT with 5 threads
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --athenaopts="--threads=5" \
           --postExec 'conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3SYM-01");conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3SYM-01")' \
           --outputESDFile OUT_ESD_5thread.root
exit_code=$?
echo  "art-result: ${exit_code} Reco_tf.py_5thread"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi
mv log.RAWtoESD log.RAWtoESD_5thread
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
