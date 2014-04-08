from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [

    "/afs/cern.ch/atlas/maxidisk/d36/lumiJuly2010/data10_7TeV.00152166.physics_MinBias.merge.AOD.r1239_p134_tid129143_00/AOD.129143._000120.pool.root.1"
    
                 ]

af.EvtMax=-1 # number of event to process

#======================================================================================

# Adding a public version of our new tool which will read from EventInfo
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuTool
ToolSvc += LumiBlockMuTool("LumiBlockMuTool")

from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
rec.readRDO = False
rec.readESD = True
rec.readAOD = True
rec.doCBNT = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doDPD = True
rec.doFileMetaData = True

# Output log setting
OutputLevel = INFO

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

# add LumiCalcMuWriter, should only be done in RAW->ESD transform
from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault

lumiTool = LuminosityToolDefault()
lumiTool.OutputLevel = DEBUG
ToolSvc += lumiTool

from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuWriter
muWriter = LumiBlockMuWriter("LumiBlockMuWriter")
muWriter.OutputLevel = DEBUG
#muWriter.LuminosityTool = ToolSvc.LuminosityTool
        
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += muWriter

# this switches off performance monitoring ntuples creation
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False




