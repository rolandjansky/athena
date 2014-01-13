
CASTOR_PATH="/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data10_cos/calibration_L2CostMonitoring"
TARGET_HTML="/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/online"

for run in `nsls $CASTOR_PATH`
do
  dir=$CASTOR_PATH/$run/data10_cos.0${run}.calibration_L2CostMonitoring.Reproc.NTUP_COSTMON.c65
  web=$TARGET_HTML/$run
  sub="$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py $dir -o $web --data -n 100000"
  bat="bsub -q atlastrig $TestArea/Trigger/TrigCost/TrigCostRate/scripts/run_cost.sh $TestArea/setup_pro.sh $dir $web 100000"

  echo run=$run
  echo dir=$dir
  echo sub=$sub
  echo bat=$bat
  break
done
