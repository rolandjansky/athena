## @file EventCheck.py
## @brief TopOptions: Using command line argument In, read 1 event from In
## and check that all the objects in the DataHeader are readable.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventCheck.py,v 1.10 2007-08-15 21:23:24 gemmeren Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
#import glob, os, re

theApp.EvtMax = 200000

theApp.TopAlg  = ["EventCount"]
theApp.TopAlg += ["EventCheck"]

theApp.Dlls += [ "AthenaPoolTools" ]   # Needed EventCount
theApp.Dlls += [ "AthenaPoolMultiTest" ]   # Needed EventCheck
theApp.Dlls += [ "CLIDSvc" ]               # Needed by EventCount

# EventCheck configuration ---------------------- BEGIN
# Configure the event checking algorithm
EventCheck = Algorithm("EventCheck")
EventCheck.CheckData = True  # if you want it to check data format
EventCheck.MaxCheck = 1 # how many events to check, default is 10
#EventCheck.ExcludeCLIDS = [4187,4190,1235574503] # crashers
#EventCheck.ExcludeCLIDS = [2534,77883132] # crashers
#EventCheck.OutputLevel = DEBUG

include("AthenaPoolTools/CopyFiles.py")

# EventCheck configuration ------------------------ END

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
#==============================================================
#
# End of job options file
#
###############################################################
