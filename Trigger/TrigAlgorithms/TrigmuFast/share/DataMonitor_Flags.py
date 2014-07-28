# Set flags for running the muFast algo in the online setup

# Use of Calibration Service
muFastUseCalib=True

# RPC testbeam 2004 configuration
#TestBeam2004=True
CombinedRun=True
RoISaleve=False
Hodo=False

# offline data processing
Local=False
LocalRunDirllectory="/pcatr01/data1/dimattia/8.7.x/Trigger/TriggerRelease/TriggerRelease-00-01-71/run"
LocalFilePrefix="daq_ROS-71_prova_RPC"

# RunNumber and events
RunNumber=2101754
#run for calibration: not CombinedRun
#RunNumber=600724
EventNumber=2000
 
include ( "TrigmuFast/TBdataMonitor_jobOptions.py" )
