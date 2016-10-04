#**************************************************************
#
# TrigServicesTHistSvc configuration file
#
#==============================================================

# loading of this library (not a problem if done twice)
theApp.Dlls += [ "TrigServices" ]

# service creation
theApp.ExtSvc += [ "HltTHistSvc/THistSvc" ]
theApp.MultiThreadExtSvc += [ "HltTHistSvc/THistSvc" ]


# just a helper function
def defineTHistSvcOutputFile(dir, name, opt='NEW', service='THistSvc'):
    if numberOfThreads.value() == 0:
        THistSvc = Service(service)
        THistSvc.Output += [dir+" DATAFILE=\'"+name+"' OPT=\'"+opt+"\'" ]
        return
    for n in range(0,numberOfThreads.value()):
        threadID = '__%(#)d' % { '#': n}
        THistSvc = Service ( service+threadID )
        THistSvc.Output += [dir+" DATAFILE=\'"+name+threadID+"' OPT=\'"+opt+"\'" ]

# default settings 
defineTHistSvcOutputFile("SHIFT", "monitoring-shift.root")
defineTHistSvcOutputFile("EXPERT", "monitoring-expert.root")
defineTHistSvcOutputFile("DEBUG", "monitoring-debug.root") 
defineTHistSvcOutputFile("RUNSTAT", "monitoring-runstat.root") 



# user can also specify REGEX used to check Histograms Naming Convention
# THistSvc = Service("THistSvc")
#
# all sort of regex can be given
# THistSvc.ExcludeName = ".+LAr.+" # no LAr histograms any more
# THistSvc.IncludeName = "/SHIFT/.+"   # only monitoring allowed 
#
# the default is (implementation of the naming convention):
# see: https://edms.cern.ch/document/710911/1
# see: https://twiki.cern.ch/twiki/bin/view/Atlas/AthenaMTHistogramming
#
# THistSvc.ExcludeName = ".*\..*"
# THistSvc.IncludeName = "^/((SHIFT)|(EXPERT)|(DEBUG)|(RUNSTAT))/.+/.+"
# 
# Similar can be specified for histogram types (name of the ROOT class)
# example below can be used to exclude 2dim histograms from being registered
# THistSvc.ExcludeType = ".+2.+" 
# example below accepts only 1 dimensional integer histogram
# THistSvc.ExcludeType = ".+"
# THistSvc.IncludeType = "TH1I"

#==============================================================
#
# End of TrigServicesTHistSvc configuration file
#
#**************************************************************
