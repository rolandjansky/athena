#include "../egammaDeltaEmax2Alg.h"
#include "../egammaShowerDepthAlg.h"
#include "../PhotonTruthAlg.h"
#include "../ElectronRedoOQAlg.h"
#include "../PhotonRedoOQAlg.h"
#include "../egammaCalcOQAlg.h"
#include "../egammaTruthAlg.h"
#include "../TileGapSelectionAlg.h"
#include "../TileGapSumAlg.h"
#include "../PhotonTruthTool.h"

#include "../egammaMaxECellAlg.h"
#include "../egammaSumCellsGainAlg.h" 
#include "../egammaNbCellsGainAlg.h"
#include "../egammaTimeCorrAlg.h"




DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaDeltaEmax2Alg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaShowerDepthAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, PhotonTruthAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, ElectronRedoOQAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, PhotonRedoOQAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaCalcOQAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaTruthAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, TileGapSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, TileGapSumAlg)

DECLARE_NAMESPACE_TOOL_FACTORY           (D3PD, PhotonTruthTool)

DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaMaxECellAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaSumCellsGainAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaNbCellsGainAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, egammaTimeCorrAlg)

