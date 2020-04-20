# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

include.block('EventOverlayJobTransforms/OverlayInput_jobOptions.py')

from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

#-------------------------
# Remapping Service
#-------------------------
from SGComps import AddressRemappingSvc
if overlayFlags.processLegacyEventInfo():
    AddressRemappingSvc.addInputRename('EventInfo', 'McEventInfo', 'Sig_McEventInfo')
else:
    AddressRemappingSvc.addInputRename('xAOD::EventAuxInfo', 'EventInfoAux.', 'Sig_EventInfoAux.')
    AddressRemappingSvc.addInputRename('xAOD::EventInfo', 'EventInfo', 'Sig_EventInfo')

# Timings
AddressRemappingSvc.addInputRename('RecoTimingObj', 'EVNTtoHITS_timings', 'Sig_EVNTtoHITS_timings')

# Truth
if DetFlags.overlay.Truth_on():
    AddressRemappingSvc.addInputRename('McEventCollection', 'TruthEvent', 'Sig_TruthEvent')

    if 'TrackRecordCollection' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['TrackRecordCollection']:
            AddressRemappingSvc.addInputRename('TrackRecordCollection', collection, 'Sig_' + collection)

    if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
        for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
            AddressRemappingSvc.addInputRename('CaloCalibrationHitContainer', collection, 'Sig_' + collection)
