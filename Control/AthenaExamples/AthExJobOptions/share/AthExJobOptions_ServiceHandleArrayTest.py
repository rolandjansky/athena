# File: AthExJobOptions/AthExJobOptions_BasicJobOptions.py
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:

MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
assert( job == AlgSequence() )     # this is a singleton

# Get hold of the algorithms that we want to run
from AthExJobOptions.AthExJobOptionsConf import TestSvcHandleArrayAlg
#    ^^package name  ^^generated file           ^^Gaudi component

job += TestSvcHandleArrayAlg("MyAlg")

# register a service with the service manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += CfgMgr.ConcreteSvc(
   "QotdSvc",
   Qotd = "These are not the droids you are looking for."
   )
svcMgr += CfgMgr.AnotherConcreteSvc(
   "AnotherQotdSvc",
   Qotd = "Again, These are not the droids you are looking for."
   )
# and bind it to 'MyAlg'
job.MyAlg.SvcList = [svcMgr.QotdSvc,
                     svcMgr.AnotherQotdSvc]


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 1

# Print the final job for reference purposes
print job


