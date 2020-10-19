#!/bin/sh
#
# art-description: Run simulation from existing layout of geometry database and from local file (for muon geometry MuonSpectrometer-R.08.01)
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
# first, run simulation with ATLAS layout ATLAS-R2-2016-01-00-01 (official Run2) based on MuonSpectrometer-R.08.01
# but from local file (in this case: amdb_simrec.r.08.01)
#######################################

# download amdb file
wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
# run simulation from local amdb file
Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
          --postExec 'input_amdb_simrec="amdb_simrec.r.08.01";include("MuonGeoModel/InitGeoFromLocal_postIncl.py")' \
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
# second, run simulation with ATLAS layout ATLAS-R2-2016-01-00-01 (official Run2) based on MuonSpectrometer-R.08.01
#######################################

Sim_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
          --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
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

acmd.py diff-root --ignore-leaves timings --mode semi-detailed --error-mode resilient OUT_HITS_fromLocal.root OUT_HITS.root &> log_diff_HITS.log
exit_code=$?
echo  "art-result: ${exit_code} diff-root_sim"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

echo "art-result: $?"



