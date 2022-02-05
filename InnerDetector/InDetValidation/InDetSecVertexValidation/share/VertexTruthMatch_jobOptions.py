# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Set up the file reading:
FNAME = "/eos/atlas/atlascerngroupdisk/phys-exotics/ueh/VH4b/R22/valid1.313415.PowhegPythia8EvtGen_ZH_H125_a16a16_4b_ctau100.recon.AOD.e8323_e5984_s3227_r12627_tid26024619_00/AOD.26024619.merge.VSI.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from InDetSecVertexValidation.InDetSecVertexValidationConf import InDetSecVertexTruthMatchTool
tool = InDetSecVertexTruthMatchTool()
tool.OutputLevel=INFO
ToolSvc += tool

# Add the test algorithm:
from InDetSecVertexValidation.InDetSecVertexValidationConf import InDetSecVertexTruthMatchAlgorithm
alg = InDetSecVertexTruthMatchAlgorithm( SecVertexSGKey = "VrtSecInclusive_SecondaryVertices",
                                         TruthVertexSGKey = "TruthVertices" )
theJob += alg

outputFile = "VSI.root"

from GaudiSvc.GaudiSvcConf  import THistSvc
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = ["VTXPLOTS DATAFILE='"+outputFile+"' OPT='RECREATE'"]


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 500
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
