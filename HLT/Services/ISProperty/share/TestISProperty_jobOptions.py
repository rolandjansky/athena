# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:

MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel=WARNING

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get hold of the algorithms that we want to run
from ISProperty.ISPropertyConf import ISPropTestAlg
job += ISPropTestAlg("MyTestAlg", OutputLevel=VERBOSE)

from ISProperty.ISPropertyConf import ISPropertySvc
from GaudiKernel.GaudiHandles import ServiceHandle
ServiceMgr += ISPropertySvc()
ServiceMgr.ISPropertySvc.OutputLevel=VERBOSE
ServiceMgr.ISPropertySvc.PartitionName="MyPartition"
ServiceMgr.ISPropertySvc.ServerName="MyServer"
ServiceMgr.ISPropertySvc.ForcePublish=True

# Modify some job options parameters, by accessing the algorithms
# through their instance name:
job.MyTestAlg.MyInt   = 12
job.MyTestAlg.Delay   = 1

#run for a while
theApp.EvtMax = 1000

# Print the final job for reference purposes
print svcMgr
print job

