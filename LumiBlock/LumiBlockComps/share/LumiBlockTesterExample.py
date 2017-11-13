from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
#    "/afs/cern.ch/user/j/jboyd/gencomm/ForEric/data11_7TeV.00186156.express_express.recon.AOD.f391._lb0535._SFO-ALL._0001.1"
#    "/afs/cern.ch/atlas/maxidisk/d36/lumiJuly2010/data10_7TeV.00152166.physics_MinBias.merge.AOD.r1239_p134_tid129143_00/AOD.129143._000120.pool.root.1"
#    "root://eosatlas.cern.ch///eos/atlas/atlasdatadisk/data11_7TeV/AOD/f413_m1019/data11_7TeV.00191381.physics_JetTauEtmiss.merge.AOD.f413_m1019/data11_7TeV.00191381.physics_JetTauEtmiss.merge.AOD.f413_m1019._lb0502-lb0521._0001.1"
#"root://eosatlas.cern.ch///eos/atlas/atlasdatadisk/data12_8TeV/AOD/r4768/data12_8TeV.00200805.physics_MinBias.recon.AOD.r4768_tid01304347_00/AOD.01304347._003283.pool.root.1"
"root://eosatlas.cern.ch///eos/atlas/atlasproddisk/data12_8TeV/AOD/r4065/data12_8TeV.00213900.physics_Bphysics.recon.AOD.r4065_tid01228278_00/AOD.01228278._003644.pool.root.1"
]

#af.EvtMax=-1 # number of event to process
af.EvtMax=100 # number of event to process

#======================================================================================

from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
rec.readRDO = False
rec.readESD = False
rec.readAOD = True
rec.doCBNT = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doDPD = False
rec.doFileMetaData = False

# Output log setting
OutputLevel = INFO

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

# Get tools from toolSvc or create
from AthenaCommon.AppMgr import ToolSvc

# Must do tool configuration here for DB access to be autoconfigured from RecExCommon
name = "LuminosityTool"
if not hasattr(ToolSvc, name):
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
    ToolSvc += LuminosityToolDefault(name)

ToolSvc.LuminosityTool.OutputLevel = DEBUG
# ToolSvc.LuminosityTool.FillParamsTool = ""  # Try turning something off

if not hasattr(ToolSvc, "TrigLivefractionTool"):
    from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
    ToolSvc += TrigLivefractionToolDefault("TrigLivefractionTool")

#liveTool = getattr(ToolSvc, "TrigLivefractionTool")
ToolSvc.TrigLivefractionTool.OutputLevel = DEBUG

# Make some other things debug
ToolSvc.OnlineLumiCalibrationTool.OutputLevel = DEBUG
ToolSvc.BunchLumisTool.OutputLevel = DEBUG
ToolSvc.FillParamsTool.OutputLevel = DEBUG
        
# add LumiCalcMuWriter, should only be done in RAW->ESD transform
from LumiBlockComps.LumiBlockCompsConf import LumiBlockTester
muTester = LumiBlockTester("LumiBlockTester")
muTester.OutputLevel = DEBUG

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += muTester

# this switches off performance monitoring ntuples creation
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False




