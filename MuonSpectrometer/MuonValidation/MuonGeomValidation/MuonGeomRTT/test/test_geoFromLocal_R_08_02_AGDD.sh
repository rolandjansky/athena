#!/bin/sh
#
# art-description: Run simulation from existing layout of geometry database and from local AGDD file (for muon geometry MuonSpectrometer-R.08.02)
#
# art-type: grid
# art-include: master/Athena
#
# art-output: log.EVNTtoHITS_fromLocal
# art-output: log.EVNTtoHITS
# art-output: log_diff_HITS.log

art.py createpoolfile

set -x

#######################################
# run simulation with ATLAS layout ATLAS-R2-2016-01-02-01 (best knowledge Run2) based on MuonSpectrometer-R.08.02
# but from local file (in this case: passiveMat_r.08.02.xml)
#######################################

# download AGDD file
get_files passiveMat_r.08.02.xml
# run simulation from local AGDD file
# NOTE: the simFlags.G4Commands+=["/process/em/applyCuts true"] is added by hand, since it is part of the nominal s3512 job, but apparently overwritten when giving a custom postExec
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R2-2016-01-02-01_VALIDATION' \
          --postExec 'input_agdd_xml="passiveMat_r.08.02.xml";include("MuonGeoModel/InitGeoFromLocal_postIncl.py");simFlags.G4Commands+=["/process/em/applyCuts true"]' \
          --AMI=s3512 \
          --maxEvents 25 \
          --imf False \
          --outputHITSFile OUT_HITS_fromLocal.root

exit_code=$?
echo  "art-result: ${exit_code} Sim_tf_fromLocal"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

mv log.EVNTtoHITS log.EVNTtoHITS_fromLocal

#######################################
# run simulation with ATLAS layout ATLAS-R2-2016-01-02-01 (best knowledge Run2) based on MuonSpectrometer-R.08.02
#######################################

Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R2-2016-01-02-01_VALIDATION' \
          --AMI=s3512 \
          --maxEvents 25 \
          --imf False \
          --outputHITSFile OUT_HITS.root

exit_code=$?
echo  "art-result: ${exit_code} Sim_tf"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

#######################################
# then diff the output files
#######################################

acmd.py diff-root OUT_HITS_fromLocal.root OUT_HITS.root --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref &> log_diff_HITS.log
exit_code=$?
echo  "art-result: ${exit_code} diff-root_sim"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

echo "art-result: $?"



