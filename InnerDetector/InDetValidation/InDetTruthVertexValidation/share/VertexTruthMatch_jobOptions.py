# Set up the file reading:
FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchTool
tool = InDetVertexTruthMatchTool()
tool.OutputLevel=DEBUG
ToolSvc += tool

# Add the test algorithm:
from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchAlgorithm
alg = InDetVertexTruthMatchAlgorithm()
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
