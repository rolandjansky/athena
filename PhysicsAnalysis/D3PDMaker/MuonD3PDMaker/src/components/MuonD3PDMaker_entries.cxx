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


DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonTrkHitFillerTool) // Serhan
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonNumberOfSegmentsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonTrackParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonL1TriggerObjectAssociationTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonL2TriggerObjectAssociationTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonEFTriggerObjectAssociationTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonEFInfoTriggerObjectAssociationTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, L2MuonCB1FillerTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, L2MuonCB2FillerTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, EFMuonFillerTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonGenParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, EFInfoMuonKinematicsFiller) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonSegmentAuthorFillerTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonSegmentLocationFillerTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonSegmentFitQualityFillerTool) // Srivas
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonSegmentT0FillerTool) // Srivas, Verena
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonTruthClassificationFillerTool) // Max
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, TruthMuonsToSG) // Srivas
// DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonIDIsolTool) // Lashkar
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonSpShowerFillerTool) // Michiru, Haifeng
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MdtPrepDataFillerTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, RpcPrepDataFillerTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TgcPrepDataFillerTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, CscPrepDataFillerTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MdtPrepDataCollectionGetterTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, RpcPrepDataCollectionGetterTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TgcPrepDataCollectionGetterTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, CscPrepDataCollectionGetterTool) // David
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonPatternCombinationFillerTool) // Daniel
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonPatternCombinationMissedHitFillerTool) // Daniel
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonSegmentTruthFillerTool) // Daniel

DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MDTSimHitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TrackRecordFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, MuonTruthHitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TrackRecordCollectionGetterTool)

