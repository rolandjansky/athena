#include "CaloLocalHadCalib/GetLCClassification.h"
#include "CaloLocalHadCalib/GetLCWeights.h"
#include "CaloLocalHadCalib/GetLCOutOfCluster.h"
#include "CaloLocalHadCalib/GetLCDeadMaterial.h"
#include "CaloLocalHadCalib/GetLCDeadMaterialTree.h"
#include "CaloLocalHadCalib/GetLCSinglePionsPerf.h"
#include "CaloLocalHadCalib/CaloReadLCWeightsFile.h"
#include "CaloLocalHadCalib/CaloReadLCClassificationFile.h"
#include "CaloLocalHadCalib/CaloReadLCOutOfClusterFile.h"
#include "CaloLocalHadCalib/CaloReadLCJetEnergyScaleFile.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_ALGORITHM_FACTORY( GetLCClassification )
DECLARE_ALGORITHM_FACTORY( GetLCWeights )
DECLARE_ALGORITHM_FACTORY( GetLCOutOfCluster )
DECLARE_ALGORITHM_FACTORY( GetLCDeadMaterial )
DECLARE_ALGORITHM_FACTORY( GetLCDeadMaterialTree )
DECLARE_ALGORITHM_FACTORY( GetLCSinglePionsPerf )
DECLARE_ALGORITHM_FACTORY( CaloReadLCWeightsFile )
DECLARE_ALGORITHM_FACTORY( CaloReadLCClassificationFile )
DECLARE_ALGORITHM_FACTORY( CaloReadLCOutOfClusterFile )
DECLARE_ALGORITHM_FACTORY( CaloReadLCJetEnergyScaleFile )
DECLARE_FACTORY_ENTRIES(CaloLocalHadCalib) {
    DECLARE_ALGORITHM( GetLCClassification );
    DECLARE_ALGORITHM( GetLCWeights );
    DECLARE_ALGORITHM( GetLCOutOfCluster );
    DECLARE_ALGORITHM( GetLCDeadMaterial );
    DECLARE_ALGORITHM( GetLCDeadMaterialTree );
    DECLARE_ALGORITHM( GetLCSinglePionsPerf );
    DECLARE_ALGORITHM( CaloReadLCWeightsFile );
    DECLARE_ALGORITHM( CaloReadLCClassificationFile );
    DECLARE_ALGORITHM( CaloReadLCOutOfClusterFile );
    DECLARE_ALGORITHM( CaloReadLCJetEnergyScaleFile );
}
