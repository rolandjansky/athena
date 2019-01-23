# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

include.block('EventOverlayJobTransforms/OverlayOutput_jobOptions.py')

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from OverlayCommonAlgs.OverlayFlags import overlayFlags

outStream = AthenaPoolOutputStream('StreamRDO', athenaCommonFlags.PoolRDOOutput(), asAlg=True)
outStream.ItemList = []

# Event info
outStream.ItemList += [ 'xAOD::EventInfo#*', 'xAOD::EventAuxInfo#*' ]
outStream.ItemList += [ 'xAOD::EventInfoContainer#*', 'xAOD::EventInfoAuxContainer#*' ]

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

    if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
        if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
            for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
                outStream.ItemList += ['CaloCalibrationHitContainer#' + collection]

    if DetFlags.overlay.SCT_on():
        outStream.ItemList += ['InDetSimDataCollection#SCT_SDO_Map']

# Inner detector
if DetFlags.overlay.SCT_on():
    outStream.ItemList += ['SCT_RDO_Container#SCT_RDOs']

# Temporary to ensure the output is stored
outStream.TransientItems = outStream.ItemList
