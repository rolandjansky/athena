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



DECLARE_COMPONENT( D3PD::ClusterSamplingFillerTool )
DECLARE_COMPONENT( D3PD::ClusterEMSamplingFillerTool )
DECLARE_COMPONENT( D3PD::ClusterTimeFillerTool )
DECLARE_COMPONENT( D3PD::ClusterMomentFillerTool )
DECLARE_COMPONENT( D3PD::ClusterPositionInCaloFillerTool )
DECLARE_COMPONENT( D3PD::MBTSFillerTool )
DECLARE_COMPONENT( D3PD::MBTSTimeFillerTool )
DECLARE_COMPONENT( D3PD::CellFillerTool )
DECLARE_COMPONENT( D3PD::CaloInfoFillerTool )
DECLARE_COMPONENT( D3PD::CaloCellDetailsFillerTool )
DECLARE_COMPONENT( D3PD::CaloCellRawFillerTool )
DECLARE_COMPONENT( D3PD::TileCellDetailsFillerTool )
DECLARE_COMPONENT( D3PD::TileModuleBitsFillerTool )
DECLARE_COMPONENT( D3PD::SGTileModuleBitsGetterTool )
DECLARE_COMPONENT( D3PD::TileDigitFillerTool )
DECLARE_COMPONENT( D3PD::SGTileDigitsGetterTool )
DECLARE_COMPONENT( D3PD::TileRawChannelFillerTool )
DECLARE_COMPONENT( D3PD::JetTileAssociationTool )
DECLARE_COMPONENT( D3PD::TileCellRawAssociationTool )
DECLARE_COMPONENT( D3PD::TileCellDigitAssociationTool )
DECLARE_COMPONENT( D3PD::TileCosmicMuonFillerTool )
DECLARE_COMPONENT( D3PD::TileCosmicMuonTileCellAssociationTool )
DECLARE_COMPONENT( D3PD::TileMuFillerTool )
DECLARE_COMPONENT( D3PD::TileL2FillerTool )
DECLARE_COMPONENT( D3PD::TileTTL1FillerTool )
DECLARE_COMPONENT( D3PD::TileTriggerFillerTool )
DECLARE_COMPONENT( D3PD::TileHitFillerTool )
DECLARE_COMPONENT( D3PD::TileHitInfoFillerTool )
DECLARE_COMPONENT( D3PD::SGTileHitGetterTool )
DECLARE_COMPONENT( D3PD::SGTileRawChannelGetterTool )
DECLARE_COMPONENT( D3PD::TowerFillerTool )
DECLARE_COMPONENT( D3PD::LArDigitFillerTool )
DECLARE_COMPONENT( D3PD::LArCollisionTimeFillerTool )
DECLARE_COMPONENT( D3PD::LArNoisyROFillerTool )
DECLARE_COMPONENT( D3PD::LArRawChannelFillerTool )
DECLARE_COMPONENT( D3PD::LArRawChannelContainerGetterTool )
DECLARE_COMPONENT( D3PD::CaloClusterCellAssociationTool )
DECLARE_COMPONENT( D3PD::CaloClusterExtendedCellAssociationTool )
DECLARE_COMPONENT( D3PD::LArHitFillerTool )
DECLARE_COMPONENT( D3PD::LArHitContainerGetterTool )
DECLARE_COMPONENT( D3PD::CaloCalibHitFillerTool )
DECLARE_COMPONENT( D3PD::CaloCalibrationHitContainerGetterTool )
DECLARE_COMPONENT( CaloCellFilterAlg )
DECLARE_COMPONENT( ClusterFilter )

DECLARE_COMPONENT( D3PD::SCFillerTool )
DECLARE_COMPONENT( D3PD::LArSCHitFillerTool )

