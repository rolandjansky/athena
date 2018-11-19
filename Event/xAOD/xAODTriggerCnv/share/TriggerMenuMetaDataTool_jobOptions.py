# $Id: TriggerMenuMetaDataTool_jobOptions.py 651874 2015-03-05 14:16:19Z krasznaa $
#
# This is a simple test for xAODMaker::TriggerMenuMetaDataTool, testing if
# it does its job correctly during file processing.
#

# Set up the reading of the input files using RecExCommon:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/"
    "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD."
    "e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
    "AOD.01597980._000098.pool.root.1",
    "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/"
    "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD."
    "e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
    "AOD.01597980._000420.pool.root.1"
    ]
athenaCommonFlags.EvtMax = -1

# Include RecExCommon:
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
include( "RecExCommon/RecExCommon_topOptions.py" )

# Configure the metadata tool:
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool",
                                                      OutputLevel = 1 )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]

# Set up an output file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
output = MSMgr.NewPoolRootStream( "StreamAOD", "merged.pool.root" )

# Add some event and metadata items to the stream:
output.AddItem( [ "xAOD::EventInfo#EventInfo",
                   "xAOD::EventAuxInfo#EventInfoAux." ] )
output.AddMetaDataItem( [ "xAOD::TriggerMenuContainer#TriggerMenu",
                          "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ] )
