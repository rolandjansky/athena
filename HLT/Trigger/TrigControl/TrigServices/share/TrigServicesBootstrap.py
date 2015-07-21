#**************************************************************
#
# HLT bootstrap options file
#
#==============================================================

#--------------------------------------------------------------
# HLT specific event loop and output stream
#--------------------------------------------------------------
theApp.EventLoop         = "HltEventLoopMgr"
#theApp.OutStreamType     = "AthenaOutputStream"

#--------------------------------------------------------------
# in Hlt the EventLoop is controlled by the HLT Processing Unit
# so the default is no Event Selector
#--------------------------------------------------------------
theApp.EvtSel            = "NONE"

#--------------------------------------------------------------
# take care of job options legacy
#--------------------------------------------------------------
theApp.JobOptionsPath    = ""
theApp.JobOptionsType    = "NONE"

#==============================================================
#
# End of HLT bootstrap options file
#
#**************************************************************
