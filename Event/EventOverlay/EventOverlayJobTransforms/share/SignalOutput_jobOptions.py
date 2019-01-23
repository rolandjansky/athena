# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

include.block ('EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py')

from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# TODO: some event info update will be needed

# The output - signal
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
signalStream = AthenaPoolOutputStream('StreamRDO_SGNL', overlayFlags.PoolSignalRDOOutput(), asAlg=True)
signalStream.ItemList = []

signalStream.ItemList += ['RecoTimingObj#' + overlayFlags.sigPrefix() + 'EVNTtoHITS_timings']

if DetFlags.overlay.Truth_on():
    signalStream.ItemList += ['McEventCollection#' + overlayFlags.sigPrefix() + 'TruthEvent']

    if 'TrackRecordCollection' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['TrackRecordCollection']:
            signalStream.ItemList += ['TrackRecordCollection#' + overlayFlags.sigPrefix() + collection]

    if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
        if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
            for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
                signalStream.ItemList += ['CaloCalibrationHitContainer#' + overlayFlags.sigPrefix() + collection]

    if DetFlags.overlay.SCT_on():
        signalStream.ItemList += ['InDetSimDataCollection#' + overlayFlags.sigPrefix() + 'SCT_SDO_Map']

if DetFlags.overlay.SCT_on():
    signalStream.ItemList += ['SCT_RDO_Container#' + overlayFlags.sigPrefix() + 'SCT_RDOs']

# Temporary to ensure the output is stored
signalStream.TransientItems = signalStream.ItemList

# TODO: Write all IOV meta data containers
signalStream.MetadataItemList += ['IOVMetaDataContainer#*']
signalStream.MetadataItemList += ['LumiBlockCollection#*']
