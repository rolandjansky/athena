include.block ( "AmdcOracle/AmdcOracle_jobOptions.py" )
#--------------------------------------------------------------
# AmdcOracle
#--------------------------------------------------------------
#load AmdcDb
include( "AmdcDb/AmdcDb_jobOptions.py" )

from AmdcOracle.AmdcOracleConf import AmdcDumpOracle
topSequence += AmdcDumpOracle()

theAmdcDumpOracle = topSequence.AmdcDumpOracle

#if 1 do nothing 
theAmdcDumpOracle.SwitchOff = 1

#Print Level
theAmdcDumpOracle.WarningLevel = 1

#if 1 initialize() return FAILURE to speed up analysis
theAmdcDumpOracle.EmergencyOut = 1

#Set sql prod
theAmdcDumpOracle.AsNewTableOn    = 0 ; 
theAmdcDumpOracle.UseKeysOn       = 1 ; 
