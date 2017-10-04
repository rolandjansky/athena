#include "../TrigHIJetBaseSubtraction.h"
#include "../TrigHIEFTrackHypo.h"
#include "../ETSumHypo.h"
#include "../TrigHIL2VtxMultHypo.h"
#include "../TrigEstimateUE.h"
#include "../VnHypo.h"
#include "../HFMuonHypo.h"
#include "../UltraCentralHypo.h"
#include "../CreateHIUEEstimate.h"
#include "../HICaloCellCorectionTool.h"
#include "../ElectronUEMonitoring.h"


DECLARE_ALGORITHM_FACTORY( TrigHIJetBaseSubtraction )
DECLARE_ALGORITHM_FACTORY( TrigHIEFTrackHypo )
DECLARE_ALGORITHM_FACTORY( ETSumHypo )
DECLARE_ALGORITHM_FACTORY( TrigHIL2VtxMultHypo )
DECLARE_ALGORITHM_FACTORY( TrigEstimateUE )
DECLARE_ALGORITHM_FACTORY( HFMuonHypo )
DECLARE_ALGORITHM_FACTORY( VnHypo )
DECLARE_ALGORITHM_FACTORY( UltraCentralHypo )
DECLARE_ALGORITHM_FACTORY( CreateHIUEEstimate )
DECLARE_ALGORITHM_FACTORY( ElectronUEMonitoring )
DECLARE_TOOL_FACTORY( HICaloCellCorectionTool )

