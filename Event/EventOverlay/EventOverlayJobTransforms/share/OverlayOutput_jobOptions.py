# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

include.block('EventOverlayJobTransforms/OverlayOutput_jobOptions.py')

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from OverlayCommonAlgs.OverlayFlags import overlayFlags

outStream = AthenaPoolOutputStream('StreamRDO', athenaCommonFlags.PoolRDOOutput(), asAlg=True)
outStream.ItemList = []

# Event info
outStream.ItemList += [ 'xAOD::EventInfo#EventInfo', 'xAOD::EventAuxInfo#EventInfoAux.' ]

# Timings
outStream.ItemList += ['RecoTimingObj#EVNTtoHITS_timings']
if not overlayFlags.isDataOverlay():
    outStream.ItemList += ['RecoTimingObj#HITStoRDO_timings']

# Truth
if DetFlags.overlay.Truth_on():
    outStream.ItemList += ['McEventCollection#TruthEvent']

    if 'TrackRecordCollection' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['TrackRecordCollection']:
            outStream.ItemList += ['TrackRecordCollection#' + collection]

    if not overlayFlags.isDataOverlay():
        outStream.ItemList += ['xAOD::JetContainer#InTimeAntiKt4TruthJets']
        outStream.ItemList += ['xAOD::JetAuxContainer#InTimeAntiKt4TruthJetsAux.']
        outStream.ItemList += ['xAOD::JetContainer#OutOfTimeAntiKt4TruthJets']
        outStream.ItemList += ['xAOD::JetAuxContainer#OutOfTimeAntiKt4TruthJetsAux.']

    if DetFlags.overlay.BCM_on():
        outStream.ItemList += [ 'InDetSimDataCollection#BCM_SDO_Map' ]

    if DetFlags.overlay.pixel_on():
        outStream.ItemList += [ 'InDetSimDataCollection#PixelSDO_Map' ]
        if overlayFlags.isDataOverlay():
            outStream.ItemList += [ 'InDetBSErrContainer#PixelByteStreamErrs' ]

    if DetFlags.overlay.SCT_on():
        outStream.ItemList += ['InDetSimDataCollection#SCT_SDO_Map']

    if DetFlags.overlay.TRT_on():
        outStream.ItemList += [ 'InDetSimDataCollection#TRT_SDO_Map' ]
        if overlayFlags.isDataOverlay():
            outStream.ItemList += [ 'TRT_BSErrContainer#TRT_ByteStreamErrs' ]

    if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
        if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
            for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
                outStream.ItemList += ['CaloCalibrationHitContainer#' + collection]

    if DetFlags.overlay.CSC_on():
        outStream.ItemList += [ 'CscSimDataCollection#CSC_SDO' ]

    if DetFlags.overlay.MDT_on():
        outStream.ItemList += [ 'MuonSimDataCollection#MDT_SDO' ]

    if DetFlags.overlay.RPC_on():
        outStream.ItemList += [ 'MuonSimDataCollection#RPC_SDO' ]

    if DetFlags.overlay.TGC_on():
        outStream.ItemList += [ 'MuonSimDataCollection#TGC_SDO' ]

    if DetFlags.overlay.sTGC_on():
        outStream.ItemList += [ 'MuonSimDataCollection#sTGC_SDO' ]

    if DetFlags.overlay.Micromegas_on():
        outStream.ItemList += [ 'MuonSimDataCollection#MM_SDO' ]

# Inner detector
if DetFlags.overlay.BCM_on():
    outStream.ItemList+=['BCM_RDO_Container#BCM_RDOs']

if DetFlags.overlay.pixel_on():
    outStream.ItemList += ['PixelRDO_Container#PixelRDOs']

if DetFlags.overlay.SCT_on():
    outStream.ItemList += ['SCT_RDO_Container#SCT_RDOs']

if DetFlags.overlay.TRT_on():
    outStream.ItemList += ['TRT_RDO_Container#TRT_RDOs']

if DetFlags.overlay.LAr_on():
    outStream.ItemList+=['LArRawChannelContainer#LArRawChannels']
    if 'AddCaloDigi' in digitizationFlags.experimentalDigi():
        outStream.ItemList+=['LArDigitContainer#LArDigitContainer_MC']
    else:
        outStream.ItemList+=['LArDigitContainer#LArDigitContainer_MC_Thinned']

if DetFlags.overlay.Tile_on():
    outStream.ItemList += [ 'TileRawChannelContainer#TileRawChannelCnt' ]
    if overlayFlags.isDataOverlay() or 'AddCaloDigi' in digitizationFlags.experimentalDigi():
        outStream.ItemList += [ 'TileDigitsContainer#TileDigitsCnt', 'TileDigitsContainer#TileDigitsFlt' ]
    else:
        outStream.ItemList += [ 'TileDigitsContainer#TileDigitsFlt' ]
    outStream.ItemList += [ 'TileL2Container#TileL2Cnt' ]

if DetFlags.overlay.CSC_on():
    outStream.ItemList += [ 'CscRawDataContainer#CSCRDO' ]

if DetFlags.overlay.MDT_on():
    outStream.ItemList += [ 'MdtCsmContainer#MDTCSM' ]

if DetFlags.overlay.RPC_on():
    outStream.ItemList += [ 'RpcPadContainer#RPCPAD' ]

if DetFlags.overlay.TGC_on():
    outStream.ItemList += [ 'TgcRdoContainer#TGCRDO' ]

if DetFlags.overlay.sTGC_on():
    outStream.ItemList += [ 'Muon::STGC_RawDataContainer#sTGCRDO' ]

if DetFlags.overlay.Micromegas_on():
    outStream.ItemList += [ 'Muon::MM_RawDataContainer#MMRDO' ]

if DetFlags.overlay.LVL1_on():
    if DetFlags.simulateLVL1.LAr_on():
        outStream.ItemList += [ 'LArTTL1Container#LArTTL1EM' ]
        outStream.ItemList += [ 'LArTTL1Container#LArTTL1HAD' ]
    if DetFlags.simulateLVL1.Tile_on():
        outStream.ItemList += [ 'TileTTL1Container#TileTTL1Cnt' ]
        outStream.ItemList += [ 'TileTTL1Container#TileTTL1MBTS' ]
        outStream.ItemList += [ 'TileDigitsContainer#MuRcvDigitsCnt' ]
        outStream.ItemList += [ 'TileRawChannelContainer#MuRcvRawChCnt' ]
        outStream.ItemList += [ 'TileMuonReceiverContainer#TileMuRcvCnt' ]

# Temporary to ensure the output is stored
outStream.TransientItems = outStream.ItemList
