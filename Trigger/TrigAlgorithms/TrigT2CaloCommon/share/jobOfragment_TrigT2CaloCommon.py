#----------------------------------------------------
#  T2CaloCommon options
#----------------------------------------------------
# Not to include this file more than once
include.block ("TrigT2CaloCommon/jobOfragment_TrigT2CaloCommon.py")

theApp.DLLs += [ "TrigT2CaloCommon" ]

T2CaloMon_g1_L2 = Algorithm( "T2CaloMon_g1_L2" )
T2CaloMon_g1_L2.TrigMonitorTools =["T2CaloMonTool/t2calomontool"]


