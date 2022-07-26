# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Set up the file reading:
FNAME = "/eos/atlas/atlascerngroupdisk/phys-exotics/ueh/VH4b/R22/valid1.313415.PowhegPythia8EvtGen_ZH_H125_a16a16_4b_ctau100.recon.AOD.e8323_e5984_s3227_r12627_tid26024619_00/AOD.26024619.merge.VSI.pool.root.1"

m_CustomArgs = None

from AthenaCommon.AthArgumentParser import AthArgumentParser
MyParser = AthArgumentParser(description='')
MyParser.add_argument("--inputFile", help='Input File to run the validation on', default=FNAME)
MyParser.add_argument("--AugString", help="Augmentation string for the containers and variables" , default='')
MyParser.add_argument("--OutFile", help="Path to the output file", default='VSI.root')
MyParser.add_argument("--truthCont", help="Truth container name", default='TruthVertices')
MyParser.add_argument("--pdgIds", help="List of pdg Ids to match to", default='36')

m_CustomArgs = MyParser.parse_known_args()[0]

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ m_CustomArgs.inputFile ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from InDetSecVertexValidation.InDetSecVertexValidationConf import InDetSecVertexTruthMatchTool
tool = InDetSecVertexTruthMatchTool()
tool.OutputLevel=INFO
tool.pdgIds= m_CustomArgs.pdgIds
tool.AugString = m_CustomArgs.AugString
ToolSvc += tool

# Add the test algorithm:
from InDetSecVertexValidation.InDetSecVertexValidationConf import InDetSecVertexTruthMatchAlgorithm
alg = InDetSecVertexTruthMatchAlgorithm( SecVertexSGKey = "VrtSecInclusive_SecondaryVertices"+m_CustomArgs.AugString,
                                         TruthVertexSGKey = m_CustomArgs.truthCont )
theJob += alg

outputFile = m_CustomArgs.OutFile

from GaudiSvc.GaudiSvcConf  import THistSvc
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = ["VTXPLOTS DATAFILE='"+outputFile+"' OPT='RECREATE'"]


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
