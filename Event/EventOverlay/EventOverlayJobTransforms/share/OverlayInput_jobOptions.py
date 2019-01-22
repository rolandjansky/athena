# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

include.block('EventOverlayJobTransforms/OverlayInput_jobOptions.py')

from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

#-------------------------
# Remapping Service
#-------------------------
from SGComps import AddressRemappingSvc	
AddressRemappingSvc.addInputRename('EventInfo', 'McEventInfo', 'Sig_McEventInfo')

# Timings
AddressRemappingSvc.addInputRename('RecoTimingObj', 'EVNTtoHITS_timings', 'Sig_EVNTtoHITS_timings')
if not overlayFlags.isDataOverlay():
    AddressRemappingSvc.addInputRename('RecoTimingObj', 'HITStoRDO_timings', 'Bkg_HITStoRDO_timings')

# Truth
if DetFlags.overlay.Truth_on():
    AddressRemappingSvc.addInputRename('McEventCollection', 'TruthEvent', 'Sig_TruthEvent')

    if 'TrackRecordCollection' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['TrackRecordCollection']:
            AddressRemappingSvc.addInputRename('TrackRecordCollection', collection, 'Sig_' + collection)

    if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
            AddressRemappingSvc.addInputRename('CaloCalibrationHitContainer', collection, 'Sig_' + collection)

    if not overlayFlags.isDataOverlay():
        AddressRemappingSvc.addInputRename('xAOD::JetContainer', 'InTimeAntiKt4TruthJets', 'Bkg_InTimeAntiKt4TruthJets')
        AddressRemappingSvc.addInputRename('xAOD::JetAuxContainer', 'InTimeAntiKt4TruthJetsAux.', 'Bkg_InTimeAntiKt4TruthJetsAux.')
        AddressRemappingSvc.addInputRename('xAOD::JetContainer', 'OutOfTimeAntiKt4TruthJets', 'Bkg_OutOfTimeAntiKt4TruthJets')
        AddressRemappingSvc.addInputRename('xAOD::JetAuxContainer', 'OutOfTimeAntiKt4TruthJetsAux.', 'Bkg_OutOfTimeAntiKt4TruthJetsAux.')
