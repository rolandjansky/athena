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

DECLARE_COMPONENT( CaloCellNoiseAlg )
DECLARE_COMPONENT( CaloNoise2Ntuple )
DECLARE_COMPONENT( CaloFillCellPositionShift )
DECLARE_COMPONENT( CaloCellPosition2Ntuple )
DECLARE_COMPONENT( CaloRescaleNoise )
DECLARE_COMPONENT( CaloAddCellPedShift )
DECLARE_COMPONENT( FCAL_HV_Energy_Rescale )
DECLARE_COMPONENT( CaloCellCalcEnergyCorr )
DECLARE_COMPONENT( CaloCellEnergyCorr2Ntuple )
DECLARE_COMPONENT( LArMinBiasAlg )
  

