from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    # Zee AOD
    '/rscratch/pcatr18/tkohno/data/test/zeeAod.root', 
    ]
#athenaCommonFlags.EvtMax = -1
athenaCommonFlags.EvtMax = 100

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.AutoConfiguration = ['everything']
# rec.AutoConfiguration.set_Value_and_Lock(['ConditionsTag'])

include ("RecExCommon/RecExCommon_topOptions.py")

#-------------------------------------------------------------------
# Add extra algorithms
#-------------------------------------------------------------------
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

from AthenaCommon.AlgSequence import AlgSequence
from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
job = AlgSequence()
job += FlatHltNaviMaker()
include('TrigEgammaD3PDMaker/Setup_RoILinksCnvTool_IBv2.py')

if not 'level' in dir():
    level = 99
else:
    level = int(level)

d3pd_output_name = 'd3pd_level%d.root' % level
from TrigEgammaD3PDMaker.TrigEgammaD3PD import TrigEgammaD3PD
alg = TrigEgammaD3PD(d3pd_output_name, level)

# from AthenaCommon.AppMgr import ServiceMgr
# ServiceMgr.StoreGateSvc.Dump = True

print 'TopAlg'
print job

