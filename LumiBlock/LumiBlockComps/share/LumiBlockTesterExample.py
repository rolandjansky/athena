from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
#    "/afs/cern.ch/user/j/jboyd/gencomm/ForEric/data11_7TeV.00186156.express_express.recon.AOD.f391._lb0535._SFO-ALL._0001.1"
#    "/afs/cern.ch/atlas/maxidisk/d36/lumiJuly2010/data10_7TeV.00152166.physics_MinBias.merge.AOD.r1239_p134_tid129143_00/AOD.129143._000120.pool.root.1"
    "root://eosatlas///eos/atlas/atlasdatadisk/data11_7TeV/AOD/f413_m1019/data11_7TeV.00191381.physics_JetTauEtmiss.merge.AOD.f413_m1019/data11_7TeV.00191381.physics_JetTauEtmiss.merge.AOD.f413_m1019._lb0502-lb0521._0001.1"
]

#af.EvtMax=-1 # number of event to process
af.EvtMax=100 # number of event to process

#======================================================================================

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

# Must do tool configuration here for DB access to be autoconfigured from RecExCommon
from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
lumiTool = LuminosityToolDefault()
lumiTool.OutputLevel = DEBUG
ToolSvc += lumiTool

from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
liveTool = TrigLivefractionToolDefault()
liveTool.OutputLevel = DEBUG
ToolSvc += liveTool

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




