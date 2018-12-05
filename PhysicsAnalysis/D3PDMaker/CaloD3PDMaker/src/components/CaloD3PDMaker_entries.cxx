#include "../CaloCellDetailsFillerTool.cxx"
#include "../CaloCellFilterAlg.cxx"
#include "../CaloCellRawFillerTool.cxx"
#include "../CaloClusterCellAssociationTool.cxx"
#include "../ClusterEMSamplingFillerTool.cxx"
#include "../ClusterPositionInCaloFillerTool.cxx"
#include "../ClusterSamplingFillerTool.cxx"
#include "../LArCollisionTimeFillerTool.cxx"
#include "../MBTSFillerTool.cxx"
#include "../MBTSTimeFillerTool.cxx"
#include "../caloInheritance.cxx"



// CaloCellD3PDObject, make[Calo|Tile]CellFilterAlg
DECLARE_COMPONENT( CaloCellFilterAlg )

// egammaCluster, ClusterD3PDObject, CaloCellD3PDObject, xAODClusterD3PDObject
DECLARE_COMPONENT( D3PD::CaloCellDetailsFillerTool )
DECLARE_COMPONENT( D3PD::CaloClusterCellAssociationTool ) // xAOD

// egammaCluster, CaloCellD3PDObject
DECLARE_COMPONENT( D3PD::CaloCellRawFillerTool )

// egammaCluster
DECLARE_COMPONENT( D3PD::ClusterEMSamplingFillerTool ) // xAOD
DECLARE_COMPONENT( D3PD::ClusterPositionInCaloFillerTool ) // xAOD

// BeamBackgroundD3PDObject, CorrectionClusterD3PDObject, ClusterD3PDObject, xAODClusterD3PDObject, EMClusterD3PDObject; TileRecEx
DECLARE_COMPONENT( D3PD::ClusterSamplingFillerTool ) // xAOD

// LArCollisionTimeD3PDObject
DECLARE_COMPONENT( D3PD::LArCollisionTimeFillerTool )

// MBTSD3PDObject; used from TileRecEx
DECLARE_COMPONENT( D3PD::MBTSFillerTool )

// MBTSTimeD3PDObject
DECLARE_COMPONENT( D3PD::MBTSTimeFillerTool )
