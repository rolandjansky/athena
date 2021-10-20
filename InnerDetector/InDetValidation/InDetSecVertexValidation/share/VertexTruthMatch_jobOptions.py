# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Set up the file reading:
FNAME = "/afs/cern.ch/work/j/jburzyns/WorkArea/runDerivation/DAOD_PHYSLLP.test.pool.root"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from InDetSecVertexValidation.InDetSecVertexValidationConf import InDetSecVertexTruthMatchTool
tool = InDetSecVertexTruthMatchTool()
tool.OutputLevel=DEBUG
ToolSvc += tool

# Add the test algorithm:
from InDetSecVertexValidation.InDetSecVertexValidationConf import InDetSecVertexTruthMatchAlgorithm
alg = InDetSecVertexTruthMatchAlgorithm()
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
