from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags


athenaCommonFlags.PoolESDInput.set_Value_and_Lock(['ESD.pool.root'])
rec.readESD.set_Value_and_Lock(True)
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
from CaloD3PDMaker.TileDetailsD3PDObject import *
from CaloD3PDMaker.TileDigitD3PDObject import *
from CaloD3PDMaker.TileRawChannelD3PDObject import *
from CaloD3PDMaker.CaloInfoD3PDObject import *
from CaloD3PDMaker.LArDigitD3PDObject import *
from EventCommonD3PDMaker.EventInfoD3PDObject import *
from CaloD3PDMaker.CaloD3PDMakerConf import CaloCellFilterAlg

CaloCellFilterAlg.OutputLevel=DEBUG
CaloD3PDMaker.TileRawChannelD3PDObject.TileRawChannelSGKey='TileRawChannelFlt'
CaloD3PDMaker.TileDigitD3PDObject.TileDigitSGKey='TileDigitsFlt'

alg = MakerAlg("caloD3PD", topSequence, file = tuple_name , D3PDSvc = 'D3PD::RootD3PDSvc')
alg += EventInfoD3PDObject (10)

# alg += AllCaloCellD3PDObject (10)
# alg += SelCaloCellD3PDObject (10)
# alg += EMCellD3PDObject (10)
# alg += HECCellD3PDObject (10)
# alg += FCALCellD3PDObject (10)
# alg += TileCellD3PDObject (10)
alg += TileDetailsD3PDObject (10)
alg += TileDigitD3PDObject (2)
alg += TileRawChannelD3PDObject(2)

#alg += CaloInfoD3PDObject (10)
#
#alg += ClusterD3PDObject (10)
#
#alg += LArDigitD3PDObject (2)

