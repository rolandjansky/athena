include.block ( "AmdcAth/AmdcAth_jobOptions.py" )
#--------------------------------------------------------------
# AmdcAth
#--------------------------------------------------------------
from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
ServiceMgr += AmdcsimrecAthenaSvc()

#Debuging
ServiceMgr.AmdcsimrecAthenaSvc.CtrlDumpCo = 0 ;
ServiceMgr.AmdcsimrecAthenaSvc.CtrlFileStrings = 0 ;
ServiceMgr.AmdcsimrecAthenaSvc.EmergencyOut = 0 ;

#Control A line parameters
# ControlALine = I1 * 100000 + I2 * 10000 + I3 * 1000 + I4 * 100 + I5 * 10 + I6
# I1 = 0/1 zerofy or not the s translation 
# I2 = 0/1 zerofy or not the z translation 
# I3 = 0/1 zerofy or not the t translation 
# I4 = 0/1 zerofy or not the s rotation 
# I5 = 0/1 zerofy or not the z rotation 
# I6 = 0/1 zerofy or not the t rotation 
#ServiceMgr.AmdcsimrecAthenaSvc.ControlALine = 111111 ;

#Truncate A line rotation parameters
#1/0 truncate/ do not truncate
#ServiceMgr.AmdcsimrecAthenaSvc.TruncateALine = 0 ;

#Truncate P line rotation parameters
#1/0 truncate/ do not truncate
#ServiceMgr.AmdcsimrecAthenaSvc.TruncatePLine = 0 ;

#--------------------------------------------------------
if not 'doAmdcOracle' in dir():
     doAmdcOracle=False

if doAmdcOracle:
    include( "AmdcOracle/AmdcOracle_jobOptions.py" )

#--------------------------------------------------------
if not 'doAmdcMGM' in dir():
     doAmdcMGM=False

if doAmdcMGM:
    include( "AmdcMGM/AmdcMGM_jobOptions.py" )
