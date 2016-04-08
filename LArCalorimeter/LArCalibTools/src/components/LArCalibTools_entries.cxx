#include "LArCalibTools/LArAutoCorr2Ntuple.h"
#include "LArCalibTools/LArAverages2Ntuple.h"
#include "LArCalibTools/LArAccumulatedDigits2Ntuple.h"
#include "LArCalibTools/LArDigits2Ntuple.h"
#include "LArCalibTools/LArOFC2Ntuple.h"
#include "LArCalibTools/LArShape2Ntuple.h"
#include "LArCalibTools/LArPedestals2Ntuple.h"
#include "LArCalibTools/LArRamps2Ntuple.h"
#include "LArCalibTools/LArReadParamsFromFile.h"
#include "LArCalibTools/LAruA2MeV2Ntuple.h"
#include "LArCalibTools/LArCaliWaves2Ntuple.h"
#include "LArCalibTools/LArPhysWaves2Ntuple.h"
#include "LArCalibTools/LArWFParams2Ntuple.h"
#include "LArCalibTools/LArParams2Ntuple.h"
#include "LArCalibTools/LArPhysWaveFromTuple.h"
#include "LArCalibTools/LArPhysWaveFromStdNtuple.h"
#include "LArCalibTools/LArShapeFromStdNtuple.h"
#include "LArCalibTools/LArAutoCorrFromStdNtuple.h"
#include "LArCalibTools/LArParamsFromStdNtuple.h"
#include "LArCalibTools/LArOFCBin_PhysCaliTdiffFromStdNtuple.h"
#include "LArCalibTools/LArMphysOverMcalFromTuple.h"
#include "LArCalibTools/LArMphysOverMcal2Ntuple.h"
#include "LArCalibTools/LArHVScaleCorr2Ntuple.h"
#include "LArCalibTools/LArBlockCorrections.h"
#include "LArCalibTools/CaloCellGroup2Ntuple.h"
#include "LArCalibTools/LArFEBTemp2Ntuple.h"
#include "LArCalibTools/ForceLoadCondObj.h"
#include "LArCalibTools/LArFebTimeOffset2Ntuple.h"
#include "LArCalibTools/LArGainThresholds2Ntuple.h"
#include "LArCalibTools/LArPhysCaliTDiffAlg.h"
#include "LArCalibTools/LArOFCBinAlg.h"
#include "LArCalibTools/LArDSPThresholds2Ntuple.h"
#include "LArCalibTools/CaloCondBlob2Ntuple.h"
#include "LArCalibTools/LArCompleteToFlat.h"
#include "LArCalibTools/LArNoise2Ntuple.h"
#include "LArCalibTools/LArfSampl2Ntuple.h"
#include "LArCalibTools/LArFillDSPConfig.h"
#include "LArCalibTools/LArMinBias2Ntuple.h"
#include "LArCalibTools/LArDSPConfig2Ntuple.h"
//#include "LArCalibTools/LArSFcalFromFcalFromStdNtuple.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 
typedef LArReadParamsFromFile<LArCaliPulseParamsComplete>   LArReadCaliPulseParams ;
typedef LArReadParamsFromFile<LArDetCellParamsComplete>     LArReadDetCellParams ;
typedef LArReadParamsFromFile<LArPhysCaliTdiffComplete>     LArReadPhysCaliTdiff ;
typedef LArReadParamsFromFile<LArTdriftComplete>            LArReadTdrift ;
typedef LArReadParamsFromFile<LArMphysOverMcalComplete>     LArReadMphysOverMcal ;
typedef LArReadParamsFromFile<LArRinjComplete>              LArReadRinj ;
typedef LArReadParamsFromFile<LArTshaperComplete>           LArReadTshaper ;
typedef LArReadParamsFromFile<LArEMEC_CphiComplete>         LArReadEMEC_Cphi ;
typedef LArReadParamsFromFile<LArEMEC_HValphaComplete>      LArReadEMEC_HValpha ;
typedef LArReadParamsFromFile<LArEMEC_HVbetaComplete>       LArReadEMEC_HVbeta ;
typedef LArReadParamsFromFile<LArCableLengthComplete>       LArReadCableLength ;
typedef LArReadParamsFromFile<LArCableAttenuationComplete>  LArReadCableAttenuation ;
typedef LArReadParamsFromFile<LArCaliPulseParamsVsCalib>    LArReadCaliPulseParams_Calib ;

DECLARE_ALGORITHM_FACTORY( LArAutoCorr2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArAverages2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArAccumulatedDigits2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArDigits2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArOFC2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArShape2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArPedestals2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArRamps2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArReadCaliPulseParams ) 
DECLARE_ALGORITHM_FACTORY( LArReadCaliPulseParams_Calib ) 
DECLARE_ALGORITHM_FACTORY( LArReadDetCellParams )
DECLARE_ALGORITHM_FACTORY( LArReadPhysCaliTdiff )
DECLARE_ALGORITHM_FACTORY( LArReadTdrift )
DECLARE_ALGORITHM_FACTORY( LArReadMphysOverMcal )
DECLARE_ALGORITHM_FACTORY( LArReadRinj )
DECLARE_ALGORITHM_FACTORY( LArReadTshaper )
DECLARE_ALGORITHM_FACTORY( LArReadEMEC_Cphi )
DECLARE_ALGORITHM_FACTORY( LArReadEMEC_HValpha )
DECLARE_ALGORITHM_FACTORY( LArReadEMEC_HVbeta )
DECLARE_ALGORITHM_FACTORY( LArReadCableLength )
DECLARE_ALGORITHM_FACTORY( LArReadCableAttenuation )
DECLARE_ALGORITHM_FACTORY( LAruA2MeV2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArCaliWaves2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArPhysWaves2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArWFParams2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArParams2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArPhysWaveFromTuple )
DECLARE_ALGORITHM_FACTORY( LArPhysWaveFromStdNtuple )
DECLARE_ALGORITHM_FACTORY( LArShapeFromStdNtuple )
DECLARE_ALGORITHM_FACTORY( LArAutoCorrFromStdNtuple )
DECLARE_ALGORITHM_FACTORY( LArParamsFromStdNtuple )
DECLARE_ALGORITHM_FACTORY( LArOFCBin_PhysCaliTdiffFromStdNtuple )
DECLARE_ALGORITHM_FACTORY( LArMphysOverMcalFromTuple )
DECLARE_ALGORITHM_FACTORY( LArMphysOverMcal2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArHVScaleCorr2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArBlockCorrections )
DECLARE_ALGORITHM_FACTORY ( CaloCellGroup2Ntuple )
DECLARE_ALGORITHM_FACTORY ( LArFEBTemp2Ntuple )
DECLARE_ALGORITHM_FACTORY (ForceLoadCondObj )
DECLARE_ALGORITHM_FACTORY( LArFebTimeOffset2Ntuple )
DECLARE_ALGORITHM_FACTORY(LArGainThresholds2Ntuple)
DECLARE_ALGORITHM_FACTORY(LArDSPThresholds2Ntuple)
DECLARE_ALGORITHM_FACTORY( LArPhysCaliTDiffAlg )
DECLARE_ALGORITHM_FACTORY( LArOFCBinAlg )
DECLARE_ALGORITHM_FACTORY( CaloCondBlob2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArCompleteToFlat )
DECLARE_ALGORITHM_FACTORY( LArNoise2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArfSampl2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArFillDSPConfig )
DECLARE_ALGORITHM_FACTORY( LArDSPConfig2Ntuple )
DECLARE_ALGORITHM_FACTORY( LArMinBias2Ntuple )
//DECLARE_ALGORITHM_FACTORY( LArSFcalFromFcalFromStdNtuple )



DECLARE_FACTORY_ENTRIES( LArCalibTools ) {
  DECLARE_ALGORITHM( LArAutoCorr2Ntuple )
  DECLARE_ALGORITHM( LArAverages2Ntuple )
  DECLARE_ALGORITHM( LArAccumulatedDigits2Ntuple )
  DECLARE_ALGORITHM( LArDigits2Ntuple )
  DECLARE_ALGORITHM( LArAutoCorr2Ntuple )
  DECLARE_ALGORITHM( LArOFC2Ntuple )
  DECLARE_ALGORITHM( LArShape2Ntuple )
  DECLARE_ALGORITHM( LArPedestals2Ntuple )
  DECLARE_ALGORITHM( LAruA2MeV2Ntuple )
  DECLARE_ALGORITHM( LArRamps2Ntuple )
  DECLARE_ALGORITHM( LArReadCaliPulseParams ) 
  DECLARE_ALGORITHM( LArReadCaliPulseParams_Calib ) 
  DECLARE_ALGORITHM( LArReadDetCellParams )
  DECLARE_ALGORITHM( LArReadPhysCaliTdiff )
  DECLARE_ALGORITHM( LArReadTdrift )
  DECLARE_ALGORITHM( LArReadMphysOverMcal )
  DECLARE_ALGORITHM( LArReadRinj )
  DECLARE_ALGORITHM( LArReadTshaper )
  DECLARE_ALGORITHM( LArReadEMEC_Cphi )
  DECLARE_ALGORITHM( LArReadEMEC_HValpha )
  DECLARE_ALGORITHM( LArReadEMEC_HVbeta )
  DECLARE_ALGORITHM( LArReadCableLength )
  DECLARE_ALGORITHM( LArReadCableAttenuation )
  DECLARE_ALGORITHM( LAruA2MeV2Ntuple )
  DECLARE_ALGORITHM( LArCaliWaves2Ntuple )
  DECLARE_ALGORITHM( LArPhysWaves2Ntuple )
  DECLARE_ALGORITHM( LArWFParams2Ntuple )
  DECLARE_ALGORITHM( LArParams2Ntuple )
  DECLARE_ALGORITHM( LArPhysWaveFromTuple )
  DECLARE_ALGORITHM( LArPhysWaveFromStdNtuple )
  DECLARE_ALGORITHM( LArShapeFromStdNtuple )
  DECLARE_ALGORITHM( LArAutoCorrFromStdNtuple )
  DECLARE_ALGORITHM( LArParamsFromStdNtuple )
  DECLARE_ALGORITHM( LArOFCBin_PhysCaliTdiffFromStdNtuple )
  DECLARE_ALGORITHM( LArMphysOverMcalFromTuple )
  DECLARE_ALGORITHM( LArMphysOverMcal2Ntuple)
  DECLARE_ALGORITHM( LArHVScaleCorr2Ntuple)
  DECLARE_ALGORITHM( LArBlockCorrections )
  DECLARE_ALGORITHM (CaloCellGroup2Ntuple )
  DECLARE_ALGORITHM (LArFEBTemp2Ntuple )
  DECLARE_ALGORITHM (ForceLoadCondObj )
  DECLARE_ALGORITHM ( LArFebTimeOffset2Ntuple )
  DECLARE_ALGORITHM ( LArGainThresholds2Ntuple)  
  DECLARE_ALGORITHM ( LArDSPThresholds2Ntuple)  
  DECLARE_ALGORITHM ( LArPhysCaliTDiffAlg ) 
  DECLARE_ALGORITHM ( LArOFCBinAlg ) 
  DECLARE_ALGORITHM ( CaloCondBlob2Ntuple ) 
  DECLARE_ALGORITHM ( LArCompleteToFlat )
  DECLARE_ALGORITHM ( LArNoise2Ntuple)
  DECLARE_ALGORITHM ( LArfSampl2Ntuple)
  DECLARE_ALGORITHM ( LArFillDSPConfig )
  DECLARE_ALGORITHM ( LArDSPConfig2Ntuple )
  DECLARE_ALGORITHM( LArMinBias2Ntuple)
//  DECLARE_ALGORITHM( LArSFcalFromFcalFromStdNtuple )
}
