# Author: Ketevi A. Assamagan
# BNL, february 2008

# get a handle on the service manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle on the sequence of top-level algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [ "ESD.pool.root" ]

from AnalysisExamples.AnalysisExamplesConf import PileUpTruthExample
topSequence += PileUpTruthExample(
    "PileUpTruthExample",
    McEventKey = "TruthEvent",
    OutputLevel = INFO
    )

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100


