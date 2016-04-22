#!/bin/sh
#PBS -d __CURRENT_DIR__
#PBS -l cput=02:00:00
#PBS -o __OUT_FILE__
#PBS -e __ERR_FILE__

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
setupATLAS
COUNTER=1
until lsetup root 
do 
  echo "Try n."${COUNT} 
  let COUNTER+=1
  sleep 3
done

echo "python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/Terminator.py --input __OUT_INPUTS__"
python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/Terminator.py --input __OUT_INPUTS__
echo "python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/Merger.py --input __INPUTS__ --output temp_output_file.root --function __FUNCTION__ --job __JOBNAME__ --region __REGIONNAME__"
python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/Merger.py --input __INPUTS__ --output __OUTPUT__ --function __FUNCTION__ --job __JOBNAME__ --region __REGIONNAME__ __CHECK_SCAN__
