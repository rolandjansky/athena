#include "../T2CaloTauHypo.h"
#include "../T2IDTauHypo.h"
#include "../T2IDCoreTauHypo.h"
#include "../T2IDIsoTauHypo.h"
#include "../T2TauHypo.h"
#include "../EFTauInvHypo.h"
#include "../EFTauMVHypo.h"
#include "../EFTauDiKaonHypo.h"
#include "../EFHadCalibHypo.h"
#include "../T2TauTauCombHypo.h"
#include "../T2TauTauCombFexAlgo.h"
#include "../EFTauTauCombHypo.h"
#include "../EFTauTauCombFexAlgo.h"
#include "../HLTCaloPreSelHypo.h"
#include "../HLTTrackPreSelHypo.h"
#include "../HLTTauGenericHypo.h"
#include "../HLTTauTrackRoiUpdater.h"
#include "../HLTTauCaloRoiUpdater.h"
#include "../HLTVertexPreSelHypo.h"
#include "../HLTVertexCombo.h"
#include "../L2TauTopoFex.h"
#include "../L2TauTopoHypo.h"
#include "../EFTauTopoFex.h"
#include "../EFTauTopoHypo.h"
#include "../EFPhotonTauFex.h"
#include "../EFPhotonTauHypo.h"
#include "../TrigTauCaloRoiUpdaterMT.h"
#include "../TrigTauGenericHypoMT.h"
#include "../TrigTauCaloHypoAlgMT.h"
#include "../TrigTauTrackRoiUpdaterMT.h"
#include "../TrigTrackPreSelHypoAlgMT.h"
#include "../TrigTrackPreSelHypoTool.h"
#include "../TrigEFTauMVHypoTool.h"
#include "../TrigEFTauMVHypoAlgMT.h"
#include "../TrigTrkPrecHypoAlgMT.h"
#include "../TrigTrkPrecHypoTool.h"
#include "../TrigTauXComboHypoTool.h"

DECLARE_COMPONENT( T2CaloTauHypo )
DECLARE_COMPONENT( T2IDTauHypo )
DECLARE_COMPONENT( T2IDCoreTauHypo )
DECLARE_COMPONENT( T2IDIsoTauHypo )
DECLARE_COMPONENT( T2TauHypo )
DECLARE_COMPONENT( EFTauInvHypo )
DECLARE_COMPONENT( EFTauMVHypo )
DECLARE_COMPONENT( EFTauDiKaonHypo )
DECLARE_COMPONENT( EFHadCalibHypo )
DECLARE_COMPONENT( T2TauTauCombHypo )
DECLARE_COMPONENT( T2TauTauCombFexAlgo )
DECLARE_COMPONENT( EFTauTauCombHypo )
DECLARE_COMPONENT( EFTauTauCombFexAlgo )
DECLARE_COMPONENT( HLTCaloPreSelHypo )
DECLARE_COMPONENT( HLTTrackPreSelHypo )
DECLARE_COMPONENT( HLTTauGenericHypo )
DECLARE_COMPONENT( HLTTauTrackRoiUpdater )
DECLARE_COMPONENT( HLTTauCaloRoiUpdater )
DECLARE_COMPONENT( HLTVertexPreSelHypo )
DECLARE_COMPONENT( HLTVertexCombo )
DECLARE_COMPONENT( L2TauTopoFex )
DECLARE_COMPONENT( L2TauTopoHypo )
DECLARE_COMPONENT( EFTauTopoFex )
DECLARE_COMPONENT( EFTauTopoHypo )
DECLARE_COMPONENT( EFPhotonTauFex )
DECLARE_COMPONENT( EFPhotonTauHypo )
DECLARE_COMPONENT( TrigTauCaloRoiUpdaterMT )
DECLARE_COMPONENT( TrigTauGenericHypoMT )
DECLARE_COMPONENT( TrigTauCaloHypoAlgMT )
DECLARE_COMPONENT( TrigTauTrackRoiUpdaterMT )
DECLARE_COMPONENT( TrigTrackPreSelHypoAlgMT )
DECLARE_COMPONENT( TrigTrackPreSelHypoTool )
DECLARE_COMPONENT( TrigTrkPrecHypoTool )
DECLARE_COMPONENT( TrigTrkPrecHypoAlgMT )
DECLARE_COMPONENT( TrigEFTauMVHypoAlgMT )
DECLARE_COMPONENT( TrigEFTauMVHypoTool )
DECLARE_COMPONENT( TrigTauXComboHypoTool )
