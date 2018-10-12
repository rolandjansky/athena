#include "../CaloCellNoiseAlg.h"
#include "../CaloNoise2Ntuple.h"
#include "../CaloRescaleNoise.h"
#include "../CaloFillCellPositionShift.h"
#include "../CaloCellPosition2Ntuple.h"
#include "../CaloAddCellPedShift.h"
#include "../FCAL_HV_Energy_Rescale.h"
#include "../CaloCellCalcEnergyCorr.h"
#include "../CaloCellEnergyCorr2Ntuple.h"
#include "../LArMinBiasAlg.h"

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
  

