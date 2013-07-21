include.block ( "AmdcMGM/AmdcMGM_jobOptions.py" )
#--------------------------------------------------------------
# AmdcMGM
#--------------------------------------------------------------

from AmdcMGM.AmdcMGMConf import AmdcDumpGeoModel
topSequence += AmdcDumpGeoModel( "AmdcDumpGeoModel00" )

theAmdcDumpGeoModel00 = topSequence.AmdcDumpGeoModel00

#if 1 do nothing 
theAmdcDumpGeoModel00.SwitchOff = 1


# WarningLevel:
#  if 1 print only warnings, 
#  if 2 print warnings only for precise coordinates, 
#  print all otherwise
theAmdcDumpGeoModel00.WarningLevel = 1


# CheckTEC if 1 perform the comparison for TEC 
# EpsLoTEC Min value on the position difference to output warning for TEC
theAmdcDumpGeoModel00.CheckMdt = 1     
theAmdcDumpGeoModel00.EpsLoMdt = 0.030 
theAmdcDumpGeoModel00.CheckRpc = 1     
theAmdcDumpGeoModel00.EpsLoRpc = 0.030 
theAmdcDumpGeoModel00.CheckTgc = 1     
theAmdcDumpGeoModel00.EpsLoTgc = 0.030 
theAmdcDumpGeoModel00.CheckCsc = 1     
theAmdcDumpGeoModel00.EpsLoCsc = 0.030 


# CenterCscMean:
#  if 1 use mean value of strip corners position instead of center for CSC
theAmdcDumpGeoModel00.CenterCscMean = 0 


# ChamberSelection:
#  if 1 set on Chamber selection by
#   AmdcDumpGeoModel.StationNameSelected        = [ "BIL" , "EIL" ]
#   AmdcDumpGeoModel.StationAbsAmdcJzzSelected  = [     2 ,     1 ]
#   AmdcDumpGeoModel.StationAmdcJffSelected     = [     1 ,     1 ]
theAmdcDumpGeoModel00.ChamberSelection = 0


# StationSelection:
#  if 1 set on Station selection by
#  AmdcDumpGeoModel.StationSelected  = [ "BIL" , "BML", "BMS", "BOL" ]
theAmdcDumpGeoModel00.StationSelection = 0


#if 1 initialize() return FAILURE to speed up analysis
theAmdcDumpGeoModel00.EmergencyOut = 1

