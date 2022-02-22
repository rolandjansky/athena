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
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock(['data11_7TeV.00180124.physics_JetTauEtmiss.recon.ESD.f368._lb0334._SFO-12._0002.1'])
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

from D3PDMakerCoreComps.MakerAlg import *
from CaloD3PDMaker.ClusterD3PDObject import *
from CaloD3PDMaker.CaloCellD3PDObject import *
from CaloD3PDMaker.CaloInfoD3PDObject import *
from CaloD3PDMaker.LArDigitD3PDObject import *
from CaloD3PDMaker.TowerD3PDObject import *
from EventCommonD3PDMaker.EventInfoD3PDObject import *

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw

def CaloD3PD (file,
             level = 4,
             tuplename = 'MyTuple',
             seq = topSequence,
             D3PDSvc = 'D3PD::RootD3PDSvc',
             streamNameRoot = None,
             **kw):
                 
    alg = MakerAlg(tuplename, seq, file = file ,D3PDSvc =D3PDSvc,streamNameRoot =streamNameRoot)
    alg += EventInfoD3PDObject (**_args (level, 'EventInfo', kw ))

    # CaloTower
    alg += TowerD3PDObject (**_args(0, 'CaloTower',kw, sgkey='CombinedTower', prefix='tw_calo_'))
    # TopoTower
    alg += TowerD3PDObject (**_args(1, 'TopoTower',kw, sgkey='TopoTower', prefix='tw_topo_'))

    return alg
#TowerD3PDObject.OutputLevel= DEBUG
theAlg=CaloD3PD(tuple_name)

if readRaw :
  # put OF iteration results on SG 
  ToolSvc.LArRawChannelBuilderToolOFCIter.TimingContainerKey="LArOFIterResult"
