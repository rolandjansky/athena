
EvtMax=10
doRawData = False


from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from D3PDMakerCoreComps.D3PDObject import D3PDObject

bdir = "/afs/cern.ch/user/h/hma/work/public/data/"
PoolESDFile =bdir + "ESD.01204497._005853.pool.root.1"


athenaCommonFlags.PoolESDInput.set_Value_and_Lock([PoolESDFile])
rec.readESD.set_Value_and_Lock(True)

tuple_name = "caloD3PD_from_esd.root" 

#
#from MuonCnvExample.MuonCnvFlags import muonCnvFlags
#muonCnvFlags.RpcCablingMode="new"
#

rec.AutoConfiguration=['everything']
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)

rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
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
from CaloD3PDMaker.TileDigitD3PDObject import *
from EventCommonD3PDMaker.EventInfoD3PDObject import *
#from CaloD3PDMaker.TileRawChannelD3PDObject import *

from LArL1Sim.LArL1SimConf import LArSCSimpleMaker
topSequence+=LArSCSimpleMaker()

print 'CHECK topSequence'
print topSequence

include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "CaloConditions/CaloConditions_jobOptions.py" )

alg = MakerAlg("caloD3PD", topSequence, file = tuple_name , D3PDSvc = 'D3PD::RootD3PDSvc')
alg += EventInfoD3PDObject (10)

alg += AllCaloCellD3PDObject (10)
#alg += SelCaloCellD3PDObject (10)
# alg += EMCellD3PDObject (10)
# alg += HECCellD3PDObject (10)
# alg += FCALCellD3PDObject (10)
# alg += TileCellD3PDObject (10)

#alg += CaloInfoD3PDObject (10)

#alg += ClusterD3PDObject (10)

#alg += LArDigitD3PDObject (2)

alg += AllCaloCellD3PDObject (1,sgkey='SCellContainer',prefix='scells_')
alg.scells_Filler.scells_Filler_Detail1.SaveId=True 

import CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

if doRawData : 

  LArDigitD3PDObject = make_SGDataVector_D3PDObject( "LArDigitContainer",
                                                   "LArDigitContainer_MC",
                                                   "lardigit_", "LArDigitD3PDObject" )

  LArDigitD3PDObject.defineBlock( 0, 'Digits',
                                CaloD3PDMaker.LArDigitFillerTool,
                                SaveDigit= True,
                                SaveId = True,
                                SaveSCAAddress= False,
                                DumpIterResults= False )
  alg+=LArDigitD3PDObject(0)

  
  TileDigitD3PDObject = D3PDObject (makeTileDigitD3PDObject, 'tiledigit_', 'TileDigitD3PDObject')

  TileDigitD3PDObject.defineBlock (0, 'Digits',
                                CaloD3PDMaker.TileDigitFillerTool,
                                SaveOfflineInfo= True,
                                SaveHardwareInfo=True,
                                )
  alg+=TileDigitD3PDObject(0)


  import CaloD3PDMaker
  import D3PDMakerCoreComps
  from D3PDMakerCoreComps.D3PDObject import D3PDObject
  
  TileRawChannelSGKey='TileRawChannelCnt'

  def makeTileRawChannelD3PDObject (name, prefix, object_name='TileRawChannelD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = TileRawChannelSGKey
    if label == None: label = prefix

    if not getter:
        getter = CaloD3PDMaker.SGTileRawChannelGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileRawChannelContainer',
                  SGKey = sgkey,
                  Label = label)

    # create the selected cells
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())

  TileRawChannelD3PDObject = D3PDObject( makeTileRawChannelD3PDObject, 'tileraw_',
                                       'TileRawChannelD3PDObject' )

  TileRawChannelD3PDObject.defineBlock (0, 'RawChannel',
                                CaloD3PDMaker.TileRawChannelFillerTool,
                                SaveHardwareInfo=True,
                                SaveRawChannel= True,
                                )


  TileRawChannelD3PDObject.defineBlock (1, 'Hardware',
                                CaloD3PDMaker.TileRawChannelFillerTool,
                                SaveHardwareInfo=True,
                                SaveRawChannel= False,
                                )

  alg+=TileRawChannelD3PDObject(0)



#from CaloD3PDMaker.LArRawChannelD3PDObject import *
#alg+=LArRawChannelD3PDObject(0)

from TrigCaloD3PDMaker.TrigCaloD3PDMakerConf \
     import D3PD__TriggerTowerFillerTool, D3PD__TriggerTowerFillerTool

TriggerTowerCollection_sgkey = 'TriggerTowers'
TriggerTowerD3PDObject = make_SGDataVector_D3PDObject(
                                    'DataVector<LVL1::TriggerTower>',
                                    TriggerTowerCollection_sgkey,
                                    'tt_',
                                    'TriggerTowerD3PDObject')

# Level 0
TriggerTowerD3PDObject.defineBlock(0, 'Basics',
                                    D3PD__TriggerTowerFillerTool)
alg+=TriggerTowerD3PDObject(0)



import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject



#if readRaw :
#  # put OF iteration results on SG 
#  ToolSvc.LArRawChannelBuilderToolOFCIter.StoreTiming=True

print 'CHECK : svcMgr.DetDescrCnvSvc : '
print svcMgr.DetDescrCnvSvc
#svcMgr.StoreGateSvc.Dump=True
svcMgr.MessageSvc.OutputLevel = INFO

