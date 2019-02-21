from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg( "SGInputLoader" )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.ShowControlFlow( True )



from AthenaCommon.DetFlags import DetFlags
digiExtraInputs = []#[('McEventCollection', 'TruthEvent')]
digiExtraOutputs = []
StreamRDOExtraInputs = [('McEventCollection', 'TruthEvent')]
if DetFlags.digitize.BCM_on():
    StreamRDOExtraInputs += [( 'BCM_RDO_Container' , 'StoreGateSvc+BCM_RDOs' ),
                             ( 'InDetSimDataCollection' , 'StoreGateSvc+BCM_SDO_Map' )]

if DetFlags.digitize.pixel_on():
    StreamRDOExtraInputs += [( 'PixelRDO_Container' , 'StoreGateSvc+PixelRDOs' ),
                             ( 'InDetSimDataCollection' , 'StoreGateSvc+PixelSDO_Map' )]

if DetFlags.digitize.SCT_on():
    StreamRDOExtraInputs += [( 'SCT_RDO_Container' , 'StoreGateSvc+SCT_RDOs' ),
                             ( 'InDetSimDataCollection' , 'StoreGateSvc+SCT_SDO_Map' )]

if DetFlags.digitize.TRT_on():
    StreamRDOExtraInputs += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' ),
                             ( 'InDetSimDataCollection' , 'StoreGateSvc+TRT_SDO_Map' )]

if DetFlags.digitize.LAr_on():
    digiExtraOutputs += [('LArDigitContainer', 'LArDigitContainer_MC')]
    topSequence.LArRawChannelBuilder.ExtraInputs = [('LArDigitContainer', 'LArDigitContainer_MC')]
    topSequence.LArRawChannelBuilder.ExtraOutputs = [('LArRawChannelContainer', 'LArRawChannels')]
    topSequence.LArTTL1Maker.ExtraInputs = [('LArDigitContainer', 'LArDigitContainer_MC')] # Hack to force it to run after LArPileUpTool
    StreamRDOExtraInputs += [('LArRawChannelContainer', 'LArRawChannels')] #FIXME

if DetFlags.digitize.Tile_on():
    digiExtraOutputs += [('TileHitContainer', 'TileHitCnt')]
    StreamRDOExtraInputs += [('TileRawChannelContainer' , 'TileRawChannelCnt'),
                             ('TileRawChannelContainer' , 'TileRawChannelFit'),
                             ('TileL2Container' , 'TileL2Cnt')]

if DetFlags.digitize.CSC_on():
    StreamRDOExtraInputs += [( 'CscRawDataContainer' , 'StoreGateSvc+CSCRDO' ),
                             ( 'CscSimDataCollection' , 'StoreGateSvc+CSC_SDO' )]

if DetFlags.digitize.MDT_on():
    StreamRDOExtraInputs += [( 'MdtCsmContainer' , 'StoreGateSvc+MDTCSM' ),
                             ( 'MuonSimDataCollection' , 'StoreGateSvc+MDT_SDO' )]

if DetFlags.digitize.RPC_on():
    StreamRDOExtraInputs += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' ),
                             ( 'MuonSimDataCollection' , 'StoreGateSvc+RPC_SDO' )]

if DetFlags.digitize.TGC_on():
    StreamRDOExtraInputs += [( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO' ),
                             ( 'MuonSimDataCollection' , 'StoreGateSvc+TGC_SDO' )]

topSequence.StandardPileUpToolsAlg.ExtraInputs = digiExtraInputs
topSequence.StandardPileUpToolsAlg.ExtraOutputs = digiExtraOutputs

from AthenaCommon.AppMgr import theApp
StreamRDO = theApp.getOutputStream( "StreamRDO" )
StreamRDO.ExtraInputs += StreamRDOExtraInputs

