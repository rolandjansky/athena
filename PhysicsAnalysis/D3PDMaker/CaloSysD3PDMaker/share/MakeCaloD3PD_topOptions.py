from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags


if "readRaw" not in dir() :
    readRaw = True 

if readRaw :
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/afs/cern.ch/atlas/project/rig/data/data09_900GeV.00142193.physics_MinBias.merge.RAW._lb0098._0001.1'])
    globalflags.InputFormat.set_Value_and_Lock('bytestream')

    rec.doESD.set_Value_and_Lock(True)
    rec.readRDO.set_Value_and_Lock(True)
    rec.doWriteESD.set_Value_and_Lock(True)
    tuple_name = "caloD3PD_from_raw.root" 

else:
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock(['ESD.pool.root'])
    rec.readESD.set_Value_and_Lock(True)
    #rec.doESD.set_Value_and_Lock(False)
    #rec.readRDO.set_Value_and_Lock(False)
    #rec.doWriteESD.set_Value_and_Lock(False)
    tuple_name = "caloD3PD_from_esd.root" 

    

rec.AutoConfiguration=['everything']
athenaCommonFlags.EvtMax.set_Value_and_Lock(100)

rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)


# RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

# D3PDMaker calo block
from CaloSysD3PDMaker.CaloD3PD import CaloD3PD
alg = CaloD3PD(file = tuple_name, tuplename = 'caloD3PD')

if readRaw :
  # put OF iteration results on SG 
  ToolSvc.LArRawChannelBuilderToolOFCIter.StoreTiming=True
