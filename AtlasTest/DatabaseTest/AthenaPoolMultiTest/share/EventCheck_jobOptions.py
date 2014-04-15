## @file EventCheck_jobOptions.py
## @brief JobOptions: Defaults.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventCheck_jobOptions.py,v 1.4 2007-08-15 21:23:24 gemmeren Exp $
#--------------------------------------------------------------
theApp.Dlls += [ "AthenaPoolMultiTest" ]   # Needed both EventCount,EventCheck
theApp.Dlls += [ "CLIDSvc" ]               # Needed by EventCount

# EventCheck configuration ---------------------- BEGIN
EventCheck = Algorithm("EventCheck")
EventCheck.CheckData = False  # if you want it to check data format
EventCheck.MaxCheck = 10 # how many events to check, default is 10
EventCheck.ExcludeCLIDS = [] # CLID's to exclude from check
#EventCheck.ExcludeCLIDS = [4187] # crashers
#==============================================================
#
# End of job options file
#
###############################################################
