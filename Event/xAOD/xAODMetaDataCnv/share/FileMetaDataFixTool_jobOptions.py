# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Test jobO for xAODMaker::FileMetaDataFixTool
#
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

# Set up the reading of an input file:
INFILE = (
    "AOD.pool.root"
)

athenaCommonFlags.FilesInput = [INFILE]
athenaCommonFlags.EvtMax = 10

# Turn off most of what RecExCommon does:

rec.doCBNT.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(True)
rec.doFileMetaData.set_Value_and_Lock(True)
rec.doWritexAOD.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.AutoConfiguration = ["everything"]

# Read the file using RecExCommon:
include("RecExCommon/RecExCommon_topOptions.py")

# Set up the metadata tool:
ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool(
    "FileMetaDataCreatorTool", OutputLevel=2
)

ToolSvc += CfgMgr.xAODMaker__FileMetaDataFixTool(
    "FileMetaDataFixTool", OutputLevel=2, McChannelNumber=42,
)
svcMgr.MetaDataSvc.MetaDataTools += [
    ToolSvc.FileMetaDataCreatorTool,
    ToolSvc.FileMetaDataFixTool,
]

# Set up the writing of an output file:
stream = MSMgr.NewPoolRootStream("StreamxAOD", "DxAOD.pool.root")
stream.AddItem(
    ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
)
stream.AddMetaDataItem(
    [
        "xAOD::FileMetaData#FileMetaData",
        "xAOD::FileMetaDataAuxInfo#FileMetaDataAux.",
    ]
)
