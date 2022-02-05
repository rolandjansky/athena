# Calibration runs

#### to be run like this (from run/ directory):
# athena.py -s -c 'RunNumber='$rn';InputDir="'$folderIn'";OutputDir="'$outDataFolder'";OutName="'$aname'_'$gain'";Gain="'$gain'";Partition="ALL";EvtNo='$evts';FEBSforStatTest=["endcapAft04slot02","barrelCft01slot03"]' ../build/x86_64-slc6-gcc62-opt/jobOptions/LArMonTools/LArCalibRunMonitoringOffline.py 
#
# OutName - if you want to modify the output name
# Gain=HIGH/MEDIUM/LOW
# EvtNo set if you want to run on a limited number of events, do not set otherwise
# FEBSforStatTest tellse the coherent noise what to run on, if not set there's a default 
###

from AthenaCommon.ConcurrencyFlags import jobproperties as jps  

online = False

#
# Common header to all LArMon jobs
#

coherent_noise_febs = FEBSforStatTest #list of FEBs to be monitored, if empty, runs on them all
print(coherent_noise_febs)
#coherent_noise_PublishPartialSums = True #if true, publish the partial sums histograms, can be useful for debugging. False by default
coherent_noise_calibration_run = True


from LArMonTools.LArMonFlags import LArMonFlags
include("LArMonTools/LArMonJobOptionsOffline.py") # contains the setting up of athena for the monitoring + rootfile naming  + LArBuilders
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2018-03"
# needed at P1
#Service("PoolSvc").SortReplicas = False

svcMgr.MessageSvc.OutputLevel  = 4 #2 to set DEBUG mode, 4 to remove debug


