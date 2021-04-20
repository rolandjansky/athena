include.block ( "EventOverlayJobTransforms/TruthOverlay_jobOptions.py" )

from AthenaCommon import CfgGetter
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# Copy McEventCollection
job += CfgGetter.getAlgorithm("CopyMcEventCollection")

# Copy CaloCalibrationHitContainer(s) and TrackRecordCollection(s)
if 'TrackRecordCollection' in overlayFlags.optionalContainerMap():
    for collection in overlayFlags.optionalContainerMap()['TrackRecordCollection']:
        job += CfgGetter.getAlgorithmClone('CopyTrackRecordCollection' + collection, 'CopyTrackRecordCollection', collectionName = collection)

if 'CaloCalibrationHitContainer' in overlayFlags.optionalContainerMap():
    for collection in overlayFlags.optionalContainerMap()['CaloCalibrationHitContainer']:
        job += CfgGetter.getAlgorithmClone('CopyCaloCalibrationHitContainer' + collection, 'CopyCaloCalibrationHitContainer', collectionName = collection)

# Copy jet truth
if not overlayFlags.isDataOverlay():
    prefix = overlayFlags.bkgPrefix() if overlayFlags.isOverlayMT() else ""
    if 'xAOD::JetContainer' in overlayFlags.optionalContainerMap():
        inputContainer = prefix + "InTimeAntiKt4TruthJets"
        if inputContainer in overlayFlags.optionalContainerMap()['xAOD::JetContainer']:
            job += CfgGetter.getAlgorithm("CopyInTimeJetTruthInfo")
        inputContainer = prefix + "OutOfTimeAntiKt4TruthJets"
        if inputContainer in overlayFlags.optionalContainerMap()['xAOD::JetContainer']:
            job += CfgGetter.getAlgorithm("CopyOutOfTimeJetTruthInfo")
    if 'xAOD::TruthParticleContainer' in overlayFlags.optionalContainerMap():
        inputContainer = prefix + "TruthPileupParticles"
        if inputContainer in overlayFlags.optionalContainerMap()['xAOD::TruthParticleContainer']:
            job += CfgGetter.getAlgorithm("CopyPileupParticleTruthInfo")
