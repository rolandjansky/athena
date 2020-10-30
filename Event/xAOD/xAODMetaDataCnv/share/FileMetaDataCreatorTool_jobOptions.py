# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Test jobO for exercising xAODMaker::FileMetaDataCreatorTool to create
# the file-level metadata.
#
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

# Set up the reading of an input file:
INFILE = (
    "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/"
    "data16_13TeV.00311321.physics_Main.recon.AOD.r9264/"
    "AOD.11038520._000001.pool.root.1"
)

athenaCommonFlags.FilesInput = [INFILE]
athenaCommonFlags.EvtMax = 10

# Turn off most of what RecExCommon does:

rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.AutoConfiguration = ["everything"]

# Read the file using RecExCommon:
include("RecExCommon/RecExCommon_topOptions.py")

# Set up the metadata tool:
ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool(
    "FileMetaDataCreatorTool", OutputLevel=2
)
svcMgr.MetaDataSvc.MetaDataTools += [ToolSvc.FileMetaDataCreatorTool]

# Set up the writing of an output file:

stream = MSMgr.NewPoolRootStream("StreamDAOD", "DxAOD.pool.root")
stream.AddItem(
    ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
)
stream.AddMetaDataItem(
    [
        "xAOD::FileMetaData#FileMetaData",
        "xAOD::FileMetaDataAuxInfo#FileMetaDataAux.",
    ]
)
