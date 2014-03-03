# run.py
#
# David Adams
# January 2014
#
# Example job option for running demo tools from Athena.
#
# Run with
# > athena.py run.py
#

# Specify input file.
infile = "/afs/cern.ch/user/d/dadams/pubdata/xaod_jetclusters.root"

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax =  5

#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

# Allow messge service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 10000

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the algorithm. It runs the demo tools.
from AsgExampleTools.AsgExampleToolsConf import AsgExampleAlgorithm
job += AsgExampleAlgorithm("halg")
halg = job.halg
halg.OutputLevel = INFO

#--------------------------------------------------------------
# Configure tools.
#--------------------------------------------------------------

if 1:
  from AsgExampleTools.AsgExampleToolsConf import AsgHelloTool
  ToolSvc += AsgHelloTool("atool")
  atool = ToolSvc.atool
  atool.Message = "Hello from my Athena tool."
  atool.OutputLevel = VERBOSE
  halg.Tools += [atool]
