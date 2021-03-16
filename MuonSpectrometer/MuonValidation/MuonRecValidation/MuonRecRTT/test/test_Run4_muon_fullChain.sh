#!/bin/sh
#
# art-description: Test the Run4 muon layout in simulation/digitisation/reconstruction
# 
# art-type: grid
# art-include: master/Athena
# art-output: OUT_HITS.root
# art-output: OUT_RDO.root
# art-output: OUT_ESD.root

#####################################################################
# run simulation on 50 Zmumu events using the Run4 muon layout on top of the latest Run3 ATLAS layout
# the postExec overrides the muon geometry to use the Run4 muon layout
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R3S-2021-01-00-02_VALIDATION' \
          --AMI=s3512 \
          --maxEvents 50 \
          --imf False \
          --postExec 'database_layout="MuonSpectrometer-R.10.01";include("MuonGeoModel/InitGeoFromLocal_postIncl.py");' \
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
# now use the produced HITS file and run digitisation
# the postExec overrides the muon geometry to use the Run4 muon layout
Digi_tf.py --inputHITSFile OUT_HITS.root \
           --imf False \
           --postExec 'database_layout="MuonSpectrometer-R.10.01";include("MuonGeoModel/InitGeoFromLocal_postIncl.py");' \
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

#####################################################################
# now use the produced RDO file and run reconstruction
# the first part of the postExec overrides the muon geometry to use the Run4 muon layout
# the second part of the postExec overrides the MDT calibration setup to match the phase2 layout
Reco_tf.py --inputRDOFile OUT_RDO.root \
           --autoConfiguration everything \
           --imf False \
           --postExec 'database_layout="MuonSpectrometer-R.10.01";include("MuonGeoModel/InitGeoFromLocal_postIncl.py");conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-Run4-01");conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-Run4-01")' \
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
#####################################################################

echo "art-result: $?"
