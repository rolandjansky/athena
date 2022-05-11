# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

include.block ('EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py')

from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# TODO: some event info update will be needed

# The output - signal
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
signalStream = AthenaPoolOutputStream('StreamRDO_SGNL', overlayFlags.PoolSignalRDOOutput(), asAlg=True)
signalStream.ItemList = []

# Event info
signalStream.ItemList += [ 'xAOD::EventInfo#' + overlayFlags.sigPrefix() + 'EventInfo', 'xAOD::EventAuxInfo#' + overlayFlags.sigPrefix() + 'EventInfoAux.' ]

signalStream.ItemList += ['RecoTimingObj#' + overlayFlags.sigPrefix() + 'EVNTtoHITS_timings']

if DetFlags.overlay.Truth_on():
    signalStream.ItemList += ['McEventCollection#' + overlayFlags.sigPrefix() + 'TruthEvent']

    if 'TrackRecordCollection' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['TrackRecordCollection']:
            signalStream.ItemList += ['TrackRecordCollection#' + overlayFlags.sigPrefix() + collection]

    if DetFlags.overlay.BCM_on():
        signalStream.ItemList += [ 'InDetSimDataCollection#' + overlayFlags.sigPrefix() + 'BCM_SDO_Map' ]

    if DetFlags.overlay.pixel_on():
        signalStream.ItemList += [ 'InDetSimDataCollection#' + overlayFlags.sigPrefix() + 'PixelSDO_Map' ]

    if DetFlags.overlay.SCT_on():
        signalStream.ItemList += ['InDetSimDataCollection#' + overlayFlags.sigPrefix() + 'SCT_SDO_Map']

    if DetFlags.overlay.TRT_on():
        signalStream.ItemList += [ 'InDetSimDataCollection#' + overlayFlags.sigPrefix() + 'TRT_SDO_Map' ]

    if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
        if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
            for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
                signalStream.ItemList += ['CaloCalibrationHitContainer#' + overlayFlags.sigPrefix() + collection]

    if DetFlags.overlay.CSC_on():
        signalStream.ItemList += [ 'CscSimDataCollection#' + overlayFlags.sigPrefix() + 'CSC_SDO' ]

    if DetFlags.overlay.MDT_on():
        signalStream.ItemList += [ 'MuonSimDataCollection#' + overlayFlags.sigPrefix() + 'MDT_SDO' ]

    if DetFlags.overlay.RPC_on():
        signalStream.ItemList += [ 'MuonSimDataCollection#' + overlayFlags.sigPrefix() + 'RPC_SDO' ]

    if DetFlags.overlay.TGC_on():
        signalStream.ItemList += [ 'MuonSimDataCollection#' + overlayFlags.sigPrefix() + 'TGC_SDO' ]

    if DetFlags.overlay.sTGC_on():
        signalStream.ItemList += [ 'MuonSimDataCollection#' + overlayFlags.sigPrefix() + 'sTGC_SDO' ]

    if DetFlags.overlay.MM_on():
        signalStream.ItemList += [ 'MuonSimDataCollection#' + overlayFlags.sigPrefix() + 'MM_SDO' ]

if DetFlags.overlay.BCM_on():
    signalStream.ItemList+=['BCM_RDO_Container#' + overlayFlags.sigPrefix() + 'BCM_RDOs']

if DetFlags.overlay.pixel_on():
    signalStream.ItemList += ['PixelRDO_Container#' + overlayFlags.sigPrefix() + 'PixelRDOs']

if DetFlags.overlay.SCT_on():
    signalStream.ItemList += ['SCT_RDO_Container#' + overlayFlags.sigPrefix() + 'SCT_RDOs']

if DetFlags.overlay.TRT_on():
    signalStream.ItemList += ['TRT_RDO_Container#' + overlayFlags.sigPrefix() + 'TRT_RDOs']

if DetFlags.overlay.CSC_on():
    signalStream.ItemList += [ 'CscRawDataContainer#' + overlayFlags.sigPrefix() + 'CSCRDO' ]

if DetFlags.overlay.MDT_on():
    signalStream.ItemList += [ 'MdtCsmContainer#' + overlayFlags.sigPrefix() + 'MDTCSM' ]

if DetFlags.overlay.RPC_on():
    signalStream.ItemList += [ 'RpcPadContainer#' + overlayFlags.sigPrefix() + 'RPCPAD' ]

if DetFlags.overlay.TGC_on():
    signalStream.ItemList += [ 'TgcRdoContainer#' + overlayFlags.sigPrefix() + 'TGCRDO' ]

if DetFlags.overlay.sTGC_on():
    signalStream.ItemList += [ 'Muon::STGC_RawDataContainer#' + overlayFlags.sigPrefix() + 'sTGCRDO' ]

if DetFlags.overlay.MM_on():
    signalStream.ItemList += [ 'Muon::MM_RawDataContainer#' + overlayFlags.sigPrefix() + 'MMRDO' ]

# Temporary to ensure the output is stored
signalStream.TransientItems = signalStream.ItemList

# TODO: Write all IOV meta data containers
signalStream.MetadataItemList += ['IOVMetaDataContainer#*']
signalStream.MetadataItemList += ['LumiBlockCollection#*']
