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

DECLARE_COMPONENT( LArAutoCorr2Ntuple )
DECLARE_COMPONENT( LArAverages2Ntuple )
DECLARE_COMPONENT( LArAccumulatedDigits2Ntuple )
DECLARE_COMPONENT( LArDigits2Ntuple )
DECLARE_COMPONENT( LArOFC2Ntuple )
DECLARE_COMPONENT( LArShape2Ntuple )
DECLARE_COMPONENT( LArPedestals2Ntuple )
DECLARE_COMPONENT( LArRamps2Ntuple )
DECLARE_COMPONENT( LArReadCaliPulseParams ) 
DECLARE_COMPONENT( LArReadCaliPulseParams_Calib ) 
DECLARE_COMPONENT( LArReadDetCellParams )
DECLARE_COMPONENT( LArReadPhysCaliTdiff )
DECLARE_COMPONENT( LArReadTdrift )
DECLARE_COMPONENT( LArReadMphysOverMcal )
DECLARE_COMPONENT( LArReadRinj )
DECLARE_COMPONENT( LArReadTshaper )
DECLARE_COMPONENT( LArReadEMEC_Cphi )
DECLARE_COMPONENT( LArReadEMEC_HValpha )
DECLARE_COMPONENT( LArReadEMEC_HVbeta )
DECLARE_COMPONENT( LArReadCableLength )
DECLARE_COMPONENT( LArReadCableAttenuation )
DECLARE_COMPONENT( LAruA2MeV2Ntuple )
DECLARE_COMPONENT( LArCaliWaves2Ntuple )
DECLARE_COMPONENT( LArPhysWaves2Ntuple )
DECLARE_COMPONENT( LArWFParams2Ntuple )
DECLARE_COMPONENT( LArParams2Ntuple )
DECLARE_COMPONENT( LArPhysWaveFromTuple )
DECLARE_COMPONENT( LArPhysWaveFromStdNtuple )
DECLARE_COMPONENT( LArShapeFromStdNtuple )
DECLARE_COMPONENT( LArAutoCorrFromStdNtuple )
DECLARE_COMPONENT( LArParamsFromStdNtuple )
DECLARE_COMPONENT( LArOFCBin_PhysCaliTdiffFromStdNtuple )
DECLARE_COMPONENT( LArMphysOverMcalFromTuple )
DECLARE_COMPONENT( LArMphysOverMcal2Ntuple )
DECLARE_COMPONENT( LArHVScaleCorr2Ntuple )
DECLARE_COMPONENT( LArBlockCorrections )
DECLARE_COMPONENT( CaloCellGroup2Ntuple )
DECLARE_COMPONENT( LArFEBTemp2Ntuple )
DECLARE_COMPONENT( ForceLoadCondObj )
DECLARE_COMPONENT( LArFebTimeOffset2Ntuple )
DECLARE_COMPONENT( LArGainThresholds2Ntuple )
DECLARE_COMPONENT( LArDSPThresholds2Ntuple )
DECLARE_COMPONENT( LArPhysCaliTDiffAlg )
DECLARE_COMPONENT( LArOFCBinAlg )
DECLARE_COMPONENT( CaloCondBlob2Ntuple )
DECLARE_COMPONENT( LArCompleteToFlat )
DECLARE_COMPONENT( LArNoise2Ntuple )
DECLARE_COMPONENT( LArfSampl2Ntuple )
DECLARE_COMPONENT( LArFillDSPConfig )
DECLARE_COMPONENT( LArDSPConfig2Ntuple )
DECLARE_COMPONENT( LArMinBias2Ntuple )
//DECLARE_COMPONENT( LArSFcalFromFcalFromStdNtuple )

