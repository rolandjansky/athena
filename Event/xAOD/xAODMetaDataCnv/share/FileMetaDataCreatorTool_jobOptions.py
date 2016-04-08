# $Id: FileMetaDataCreatorTool_jobOptions.py 670279 2015-05-27 15:18:27Z krasznaa $
#
# Test jobO for exercising xAODMaker::FileMetaDataCreatorTool to create
# the file-level metadata.
#

# Set up the reading of an input file:
INFILE = "AOD.pool.root"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ INFILE ]
athenaCommonFlags.EvtMax = 10

# Turn off most of what RecExCommon does:
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.AutoConfiguration = [ "everything" ]

# Read the file using RecExCommon:
include( "RecExCommon/RecExCommon_topOptions.py" )

# Set up the metadata tool:
ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool( "FileMetaDataCreatorTool",
                                                      OutputLevel = 2 )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.FileMetaDataCreatorTool ]

# Set up the writing of an output file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
stream = MSMgr.NewPoolRootStream( "StreamDAOD", "DxAOD.pool.root" )
stream.AddItem( [ "xAOD::EventInfo#EventInfo",
                  "xAOD::EventAuxInfo#EventInfoAux." ] )
stream.AddMetaDataItem( [ "xAOD::FileMetaData#FileMetaData",
                          "xAOD::FileMetaDataAuxInfo#FileMetaDataAux." ] )
