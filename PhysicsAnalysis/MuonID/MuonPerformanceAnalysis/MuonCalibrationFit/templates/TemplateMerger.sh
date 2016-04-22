#!/bin/sh
#PBS -d __CURRENT_DIR__
#PBS -l cput=02:00:00
#PBS -o __OUT_FILE__
#PBS -e __ERR_FILE__

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
setupATLAS
#lsetup root
COUNTER=1
until lsetup root 
do 
  echo "Try n."${COUNT} 
  let COUNTER+=1
  sleep 3
done

echo "python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/Merger.py --input __INPUTS__ --output temp_output_file.root --function __FUNCTION__ --job __JOBNAME__ --region __REGIONNAME__"
python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/Merger.py --input __INPUTS__ --output temp_output_file.root --function __FUNCTION__ --job __JOBNAME__ --region __REGIONNAME__ __CHECK_SCAN__
echo "hadd __OUTPUT__ temp_output_file.root __INPUTS_SPACE__"
hadd __OUTPUT__ temp_output_file.root __INPUTS_SPACE__
echo "python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/MergeRegions.py --certain __FIRST_REG_FILE__ --input __SECOND_REG_FILE__ --output __OUT_REG_FILE__"
python /home/artoni/MuonCombinedPerformance/CalibrationFit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Scripts/MergeRegions.py --certain __FIRST_REG_FILE__ --input __SECOND_REG_FILE__ --output __OUT_REG_FILE__ 
