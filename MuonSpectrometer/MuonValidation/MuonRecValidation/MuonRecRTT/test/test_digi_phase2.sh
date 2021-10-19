#!/bin/sh
#
# art-description: Test muon digitisation on best knowledge Run4 muon layout
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-output: log.HITtoRDO

#####################################################################
# run digitisation on 2000 di-muon events (0.9<|eta|<2.8) using the best knowledge Run4 muon layout (MuonSpectrometer-R.10.01) on top of the symmetric Run3 setup (ATLAS-R3S-2021-01-00-02)
# the input HITS were produced from /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonGeomRTT/EVNT_DiMuon_10k__AbsEta_09_28__Pt_10_1000GeV.root
# simulation was executed in Athena,22.0.34
Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run4/HITS/HITS_DiMuon_2k__AbsEta_09_28__Pt_10_1000GeV.root \
           --imf False \
           --postExec 'database_layout="MuonSpectrometer-R.10.01";include("MuonGeoModel/InitGeoFromLocal_postIncl.py");conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-Run4-01");conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-Run4-01");svcMgr.MessageSvc.Format = "% F%67W%S%7W%R%T %0W%M"' \
           --outputRDOFile OUT_RDO.root \
           --athenaopts='--exctrace'
           
 exit_code=$?
echo "art-result: ${exit_code} Digi_tf.py"
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
echo "art-result: $?"
