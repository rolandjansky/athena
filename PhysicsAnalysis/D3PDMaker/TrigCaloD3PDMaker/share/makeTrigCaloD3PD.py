from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    # ttbar AOD
    # '/rscratch/pcatr18/tkohno/data/test/ttbar/AOD.092344._000274.pool.root.1',

    # Zee AOD
    '/rscratch/pcatr18/tkohno/data/test/zeeAod.root', 
    ]
athenaCommonFlags.EvtMax = -1

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

from TrigCaloD3PDMaker.TrigCaloD3PD import TrigCaloD3PD
alg = TrigCaloD3PD('d3pd.root')

