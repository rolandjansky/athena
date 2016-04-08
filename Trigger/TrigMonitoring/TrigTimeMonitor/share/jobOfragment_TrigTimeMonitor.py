#**************************************************************
#
# jopOptions file for Benchmarking using TrigTimeMonitor
#
#==============================================================
theApp.Dlls +=["TrigTimeMonitor" ]
theApp.Dlls += [ "AthenaMonitoring" ]

# In order to run the tools they must be inserted in the
# algorithms AthenaMon is one of those which run
# just tools inserted in it
#theApp.topAlg += [ "AthenaMon/AthenaMonForTime" ]
#AthenaMonForTime = Algorithm( "AthenaMonForTime" )



##############################################################
# Save timers in the N-tuple
#
#AthenaMonForTime.AthenaMonTools += ["TrigTimeTreeTool" ]
#TrigTimeTreeTool = Algorithm('ToolSvc.TrigTimeTreeTool')
# Here you change booking path
#
# TrigTimeTreeTool.BookingDir = "/EXMON/TrigTimeTree/"
#
# use this to save certain time marks
#
#TrigTimeTreeTool.Key = "sometimername"
#
# To record absolute time marks
# TrigTimeTreeTool.DoStamps = True
#

##############################################################
# Save timers in form of histograms
#
#AthenaMonForTime.AthenaMonTools += ["TrigTimeHistTool" ]
#TrigTimeHistTool = Algorithm('ToolSvc.TrigTimeHistTool')
# Here you change booking path
#
# TrigTimeHistTool.BookingDir = "/EXMON/AlgorithmsPerformanceHist/"
#
#
#TrigTimeTreeTool.Key = ""
#
# number of bins and limits of the histogram
#
#TrigTimeHistTool.NumberOfHistBins = 50
#TrigTimeHistTool.TimerHistLimits = [0, 200]
# for histograms made from timer/proprty value
# sort of "per object" timing
#
# it also can be used to histogram under certain condition.
# Namely enabling it and setting property value to 1
# for interesting events will yield the histogram with all
# events in (default one, defined above) and
# histogram with interesting events in timing per object histogram
#
#TrigTimeHistTool.DoPerObjHist = True
#TrigTimeHistTool.TimerPerObjHistLimits = [0, 200]
