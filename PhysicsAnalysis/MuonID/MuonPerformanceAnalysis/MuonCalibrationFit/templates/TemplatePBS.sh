#!/bin/sh
#PBS -d __CURRENT_DIR__
#PBS -l cput=__TIME__:00:00
#PBS -o __OUT_FILE__
#PBS -e __ERR_FILE__
#cd /home/artoni/MuonCombinedPerformance/CalibrationFit
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
setupATLAS
#asetup 20.6.0 --testarea /home/artoni/MuonCombinedPerformance/CalibrationFit
COUNTER=1
until asetup 20.6.0 --testarea /home/artoni/MuonCombinedPerformance/CalibrationFit
do 
  echo "Try n."${COUNT} 
  let COUNTER+=1
  sleep 5
done
#cd -
athena.py __JOBOPTIONS_NAME__
