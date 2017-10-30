#include "../MuonTrkHitFillerTool.h" //Serhan
#include "../MuonNumberOfSegmentsFillerTool.h"
#include "../MuonTrackParticleAssociationTool.h"
#include "../MuonL1TriggerObjectAssociationTool.h" // Srivas
#include "../MuonL2TriggerObjectAssociationTool.h" // Srivas
#include "../MuonEFTriggerObjectAssociationTool.h" // Srivas
#include "../MuonEFInfoTriggerObjectAssociationTool.h" // Srivas
#include "../L2MuonCB1FillerTool.h" // Srivas
#include "../L2MuonCB2FillerTool.h" // Srivas
#include "../EFMuonFillerTool.h" // Srivas
#include "../MuonGenParticleAssociationTool.h"
#include "../EFInfoMuonKinematicsFiller.h" // Srivas
#include "../MuonSegmentAuthorFillerTool.h" // Srivas
#include "../MuonSegmentLocationFillerTool.h" // Srivas
#include "../MuonSegmentFitQualityFillerTool.h" // Srivas
#include "../MuonSegmentT0FillerTool.h" // Srivas, Verena
#include "../TruthMuonsToSG.h" // Srivas
#include "../MuonTruthClassificationFillerTool.h" // Max
//#include "../MuonIDIsolTool.h" // Lashkar
#include "../MuonSpShowerFillerTool.h" // Michiru, Haifeng
#include "../MdtPrepDataFillerTool.h" // David
#include "../RpcPrepDataFillerTool.h" // David
#include "../TgcPrepDataFillerTool.h" // David
#include "../CscPrepDataFillerTool.h" // David
#include "../MdtPrepDataCollectionGetterTool.h" // David
#include "../RpcPrepDataCollectionGetterTool.h" // David
#include "../TgcPrepDataCollectionGetterTool.h" // David
#include "../CscPrepDataCollectionGetterTool.h" // David
#include "../MuonPatternCombinationFillerTool.h" // Daniel
#include "../MuonPatternCombinationMissedHitFillerTool.h" // Daniel
#include "../MuonSegmentTruthFillerTool.h" // Daniel

#include "../MDTSimHitFillerTool.h"
#include "../TrackRecordFillerTool.h"
#include "../MuonTruthHitsFillerTool.h"
#include "../TrackRecordCollectionGetterTool.h"


DECLARE_COMPONENT( D3PD::MuonTrkHitFillerTool ) // Serhan
DECLARE_COMPONENT( D3PD::MuonNumberOfSegmentsFillerTool )
DECLARE_COMPONENT( D3PD::MuonTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::MuonL1TriggerObjectAssociationTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonL2TriggerObjectAssociationTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonEFTriggerObjectAssociationTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonEFInfoTriggerObjectAssociationTool ) // Srivas
DECLARE_COMPONENT( D3PD::L2MuonCB1FillerTool ) // Srivas
DECLARE_COMPONENT( D3PD::L2MuonCB2FillerTool ) // Srivas
DECLARE_COMPONENT( D3PD::EFMuonFillerTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonGenParticleAssociationTool )
DECLARE_COMPONENT( D3PD::EFInfoMuonKinematicsFiller ) // Srivas
DECLARE_COMPONENT( D3PD::MuonSegmentAuthorFillerTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonSegmentLocationFillerTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonSegmentFitQualityFillerTool ) // Srivas
DECLARE_COMPONENT( D3PD::MuonSegmentT0FillerTool ) // Srivas, Verena
DECLARE_COMPONENT( D3PD::MuonTruthClassificationFillerTool ) // Max
DECLARE_COMPONENT( D3PD::TruthMuonsToSG ) // Srivas
// DECLARE_COMPONENT( D3PD::MuonIDIsolTool ) // Lashkar
DECLARE_COMPONENT( D3PD::MuonSpShowerFillerTool ) // Michiru, Haifeng
DECLARE_COMPONENT( D3PD::MdtPrepDataFillerTool ) // David
DECLARE_COMPONENT( D3PD::RpcPrepDataFillerTool ) // David
DECLARE_COMPONENT( D3PD::TgcPrepDataFillerTool ) // David
DECLARE_COMPONENT( D3PD::CscPrepDataFillerTool ) // David
DECLARE_COMPONENT( D3PD::MdtPrepDataCollectionGetterTool ) // David
DECLARE_COMPONENT( D3PD::RpcPrepDataCollectionGetterTool ) // David
DECLARE_COMPONENT( D3PD::TgcPrepDataCollectionGetterTool ) // David
DECLARE_COMPONENT( D3PD::CscPrepDataCollectionGetterTool ) // David
DECLARE_COMPONENT( D3PD::MuonPatternCombinationFillerTool ) // Daniel
DECLARE_COMPONENT( D3PD::MuonPatternCombinationMissedHitFillerTool ) // Daniel
DECLARE_COMPONENT( D3PD::MuonSegmentTruthFillerTool ) // Daniel

DECLARE_COMPONENT( D3PD::MDTSimHitFillerTool )
DECLARE_COMPONENT( D3PD::TrackRecordFillerTool )
DECLARE_COMPONENT( D3PD::MuonTruthHitsFillerTool )
DECLARE_COMPONENT( D3PD::TrackRecordCollectionGetterTool )

