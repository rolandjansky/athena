#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloCondPhysAlgs/CaloCellNoiseAlg.h"
#include "CaloCondPhysAlgs/CaloNoise2Ntuple.h"
#include "CaloCondPhysAlgs/CaloRescaleNoise.h"
#include "CaloCondPhysAlgs/CaloFillCellPositionShift.h"
#include "CaloCondPhysAlgs/CaloCellPosition2Ntuple.h"
#include "CaloCondPhysAlgs/CaloAddCellPedShift.h"
#include "CaloCondPhysAlgs/FCAL_HV_Energy_Rescale.h"
#include "CaloCondPhysAlgs/CaloCellCalcEnergyCorr.h"
#include "CaloCondPhysAlgs/CaloCellEnergyCorr2Ntuple.h"
#include "CaloCondPhysAlgs/LArMinBiasAlg.h"
#include "CaloCondPhysAlgs/LArHVMapTool.h"

DECLARE_ALGORITHM_FACTORY( CaloCellNoiseAlg )
DECLARE_ALGORITHM_FACTORY( CaloNoise2Ntuple )
DECLARE_ALGORITHM_FACTORY( CaloFillCellPositionShift )
DECLARE_ALGORITHM_FACTORY( CaloCellPosition2Ntuple )
DECLARE_ALGORITHM_FACTORY( CaloRescaleNoise )
DECLARE_ALGORITHM_FACTORY( CaloAddCellPedShift )
DECLARE_ALGORITHM_FACTORY( FCAL_HV_Energy_Rescale )
DECLARE_ALGORITHM_FACTORY( CaloCellCalcEnergyCorr )
DECLARE_ALGORITHM_FACTORY( CaloCellEnergyCorr2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArMinBiasAlg)
DECLARE_TOOL_FACTORY(LArHVMapTool)
  
DECLARE_FACTORY_ENTRIES(CaloCondPhysAlgs) {
  DECLARE_ALGORITHM( CaloCellNoiseAlg )
  DECLARE_ALGORITHM( CaloNoise2Ntuple )
  DECLARE_ALGORITHM( CaloRescaleNoise )
  DECLARE_ALGORITHM( CaloFillCellPositionShift )
  DECLARE_ALGORITHM( CaloCellPosition2Ntuple )
  DECLARE_ALGORITHM( CaloAddCellPedShift )
  DECLARE_ALGORITHM( FCAL_HV_Energy_Rescale )
  DECLARE_ALGORITHM( CaloCellCalcEnergyCorr )
  DECLARE_ALGORITHM( CaloCellEnergyCorr2Ntuple )
  DECLARE_ALGORITHM( LArMinBiasAlg)
  DECLARE_ALGTOOL( LArHVMapTool)
}

