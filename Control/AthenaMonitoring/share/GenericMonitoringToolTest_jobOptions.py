## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["EXPERT DATAFILE='phi.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them


#Run Fake RoI
from AthenaCommon.AppMgr import ToolSvc
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
from AthenaMonitoring.AthenaMonitoringConf import GenericMonitoringTool

genericMonTool = GenericMonitoringTool('monitoringTool')
genericMonTool.Histograms += [defineHistogram('phi',
                                         type='TH1D',
                                         title="PHI",
                                         xbins = 200, xmin=0., xmax=200.)]
genericMonTool.Histograms += [defineHistogram('scalar',
                                         type='TH1D',
                                         title="SomeScalar",
                                         xbins = 200, xmin=0., xmax=10.)]
genericMonTool.Histograms += [defineHistogram('phi, eta',
                                           type='TH2F',
                                           title="phi vs eta",
                                           xbins = 1000, xmin=-100., xmax=100.,
                                           ybins = 1000, ymin=-100., ymax=100.)]
genericMonTool.Histograms += [defineHistogram('eta, scalar',
                                           type='TH2D',
                                           title="eta and scalar",
                                           xbins = 10, xmin=-1., xmax=1.,
                                           ybins = 10, ymin=7., ymax=8.)]

                                         
ToolSvc += genericMonTool

from AthenaMonitoring.AthenaMonitoringConf import GenericMonitoringToolTest
genericMonitoringToolTestObj = GenericMonitoringToolTest()
genericMonitoringToolTestObj.GenericMonitoringTool = genericMonTool
topSequence += genericMonitoringToolTestObj

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=True
print svcMgr

theApp.EvtMax = 1


from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.MaxEventsInFlight = 1
svcMgr.ForwardSchedulerSvc.MaxAlgosInFlight = 1
svcMgr.ForwardSchedulerSvc.ThreadPoolSize = 1
svcMgr.ForwardSchedulerSvc.CheckDependencies = True


print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################
