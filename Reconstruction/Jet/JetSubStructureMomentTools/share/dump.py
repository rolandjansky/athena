# run.py
#
# David Adams
# January 2014
#
# Example job options for running jet reconstruction.
#
# Run with
# > athena.py run.py
#

# Specify input file.
infile = "xAOD.root"
clname = "MyCopiedJets"

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed. Default is end [-1].
theApp.EvtMax =  1

#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

# Allow messge service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 10000

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the algorithm. It runs the demo tools.
from JetRec.JetRecConf import JetAlgorithm
job += JetAlgorithm("jetalg")
jetalg = job.jetalg
jetalg.OutputLevel = INFO

#--------------------------------------------------------------
# Configure tools.
#--------------------------------------------------------------

# Dump before reco.
if 1:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("inpdmp")
  inpdmp = ToolSvc.inpdmp
  inpdmp.ContainerName = clname
  inpdmp.Detail = 2
  inpdmp.MaxObject = 20
  inpdmp.OutputLevel = INFO
  inpdmp.FloatMoments = ["tau1"]
  jetalg.Tools += [inpdmp]

