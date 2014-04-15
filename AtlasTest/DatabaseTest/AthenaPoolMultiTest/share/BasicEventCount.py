# * @file BasicEventCount.py
# * @brief TopOptions: Uses EventCount with dump flag NOT set -> nevents, runs
# * @author Jack Cranshaw (Jack.Cranshaw@cern.ch
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

theApp.EvtMax = 200000
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# Take the file name form the command line -c options
EventSelector = Service( "EventSelector" )
try:
  EventSelector.InputCollections = In
except:
  print "Usage: -c 'In=['file']'"

theApp.TopAlg  = ["EventCount"]
EC = Algorithm("EventCount")
EC.OutputLevel = INFO
EC.Dump = True

theApp.Dlls += [ "AthenaPoolMultiTest" ]   # Needed both EventCount,EventCheck
theApp.Dlls += [ "CLIDSvc" ]               # Needed by EventCount

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = FATAL
#==============================================================
#
# End of job options file
#
###############################################################
