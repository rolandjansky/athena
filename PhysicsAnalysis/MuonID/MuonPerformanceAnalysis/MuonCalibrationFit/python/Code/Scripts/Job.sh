#!/bin/sh
#PBS -d /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts
#PBS -l cput=04:00:00
#PBS -o Job.out 
#PBS -e Job.err
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
setupATLAS
COUNTER=1
until asetup 20.6.0 --testarea /home/artoni/MuonCombinedPerformance/CalibrationFit
do 
  echo "Try n."${COUNTER} 
  let COUNTER+=1
  sleep 5
done
python MakeToys.py --input /data/atlas/atlasdata/artoni/MuonCalibrationFit/25ns/Prod05/MonteCarlo_Z.root --output ToyDataFromZSimple.root --n-events 8000000
