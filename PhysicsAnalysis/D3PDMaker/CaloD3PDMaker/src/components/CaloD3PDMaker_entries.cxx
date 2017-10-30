#include "../ClusterSamplingFillerTool.h"
#include "../ClusterEMSamplingFillerTool.h"
#include "../ClusterTimeFillerTool.h"
#include "../ClusterMomentFillerTool.h"
#include "../ClusterPositionInCaloFillerTool.h"
#include "../MBTSFillerTool.h"
#include "../MBTSTimeFillerTool.h"
#include "../CellFillerTool.h"
#include "../CaloCellDetailsFillerTool.h"
#include "../CaloCellRawFillerTool.h"
#include "../TileCellDetailsFillerTool.h"
#include "../TileCosmicMuonFillerTool.h"
#include "../TileMuFillerTool.h"
#include "../TileL2FillerTool.h"
#include "../TileTTL1FillerTool.h"
#include "../TileTriggerFillerTool.h"
#include "../TileHitFillerTool.h"
#include "../TileHitInfoFillerTool.h"
#include "../TileDigitFillerTool.h"
#include "../TileRawChannelFillerTool.h"
#include "../TileModuleBitsFillerTool.h"
#include "../TowerFillerTool.h"
#include "../CaloInfoFillerTool.h"
#include "../CaloCellFilterAlg.h"
#include "../ClusterFilter.h"
#include "../LArDigitFillerTool.h"
#include "../LArCollisionTimeFillerTool.h"
#include "../LArNoisyROFillerTool.h"
#include "../LArRawChannelFillerTool.h"
#include "../LArRawChannelContainerGetterTool.h"
#include "../CaloClusterCellAssociationTool.h"
#include "../CaloClusterExtendedCellAssociationTool.h"
#include "../LArHitFillerTool.h"
#include "../SGTileModuleBitsGetterTool.h"
#include "../SGTileDigitsGetterTool.h"
#include "../SGTileRawChannelGetterTool.h"
#include "../TileCellRawAssociationTool.h"
#include "../SGTileHitGetterTool.h"
#include "../TileCellDigitAssociationTool.h"
#include "../TileCosmicMuonTileCellAssociationTool.h"
#include "../JetTileAssociationTool.h"
#include "../CaloCalibHitFillerTool.h"

#include "../SCFillerTool.h"
#include "../LArSCHitFillerTool.h"



DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterSamplingFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterEMSamplingFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterTimeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterMomentFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterPositionInCaloFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, MBTSFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, MBTSTimeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CellFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloCellDetailsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloCellRawFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCellDetailsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileModuleBitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SGTileModuleBitsGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileDigitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SGTileDigitsGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileRawChannelFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, JetTileAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCellRawAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCellDigitAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCosmicMuonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCosmicMuonTileCellAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileMuFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileL2FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileTTL1FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileTriggerFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileHitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileHitInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SGTileHitGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SGTileRawChannelGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TowerFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArDigitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArCollisionTimeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArNoisyROFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArRawChannelFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArRawChannelContainerGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloClusterCellAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloClusterExtendedCellAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArHitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArHitContainerGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloCalibHitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CaloCalibrationHitContainerGetterTool)
DECLARE_ALGORITHM_FACTORY        (CaloCellFilterAlg)
DECLARE_ALGORITHM_FACTORY        (ClusterFilter)

DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LArSCHitFillerTool)

