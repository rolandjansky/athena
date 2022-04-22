#include "../LArRawChannelBuilder.h"
#include "../ReadLArRaw.h"
#include "../ReadLArDigits.h"
#include "../LArCalibDigitPreProcessor.h"
#include "../LArRawChannelSimpleBuilder.h"
#include "../LArTimeChecker.h"
#include "../LArCalibDigitsAccumulator.h"
#include "../LArDigitsAccumulator.h"
#include "../LArRawChannelBuilderDriver.h"
#include "../LArRawChannelBuilderToolOFCIter.h"
#include "../LArRawChannelBuilderToolParabola.h"
#include "../LArRawChannelBuilderToolCubic.h"
#include "../LArRawChannelBuilderToolAverage.h"
#include "../LArRawChannelBuilderToolBadChannelTool.h"
#include "../LArRawChannelBuilderADC2EDataBase.h"
#include "../LArRawChannelBuilderADC2EConstants.h"
#include "../LArRawChannelBuilderPedestalDataBase.h"
#include "../LArRawChannelBuilderPedestalSampleZero.h"
#include "../LArRawChannelBuilderToolTileInfo.h"
#include "../LArDigitThinner.h"
#include "../LArFebErrorSummaryMaker.h"
#include "../LArCalibDigitsAccumulatorFreeGain.h"
#include "../LArRawChannelBuilderAlg.h"
#include "../LArRawChannelBuilderIterAlg.h"
#include "../LArNNRawChannelBuilder.h"
#include "../LArSCSimpleMaker.h"
#include "../LArSuperCellBCIDEmAlg.h"
#include "../LArSuperCellBCIDAlg.h"
#include "../LArHITtoCell.h"


typedef LArCalibDigitPreProcessor<TBLArCalibDigitContainer> TBLArCalDigProcessor;

DECLARE_COMPONENT( LArRawChannelBuilder )
DECLARE_COMPONENT( ReadLArRaw )
DECLARE_COMPONENT( ReadLArDigits )
DECLARE_COMPONENT( TBLArCalDigProcessor )
DECLARE_COMPONENT( LArRawChannelSimpleBuilder )
DECLARE_COMPONENT( LArTimeChecker )
DECLARE_COMPONENT( LArCalibDigitsAccumulator )
DECLARE_COMPONENT( LArDigitsAccumulator )
DECLARE_COMPONENT( LArRawChannelBuilderDriver )
DECLARE_COMPONENT( LArDigitThinner )
DECLARE_COMPONENT( LArFebErrorSummaryMaker )
DECLARE_COMPONENT( LArCalibDigitsAccumulatorFreeGain )
DECLARE_COMPONENT( LArRawChannelBuilderToolOFCIter )
DECLARE_COMPONENT( LArRawChannelBuilderToolParabola )
DECLARE_COMPONENT( LArRawChannelBuilderToolCubic )
DECLARE_COMPONENT( LArRawChannelBuilderToolAverage )
DECLARE_COMPONENT( LArRawChannelBuilderToolBadChannelTool )
DECLARE_COMPONENT( LArRawChannelBuilderADC2EDataBase )
DECLARE_COMPONENT( LArRawChannelBuilderADC2EConstants )
DECLARE_COMPONENT( LArRawChannelBuilderPedestalDataBase )
DECLARE_COMPONENT( LArRawChannelBuilderPedestalSampleZero )
DECLARE_COMPONENT( LArRawChannelBuilderToolTileInfo )
DECLARE_COMPONENT( LArRawChannelBuilderAlg )
DECLARE_COMPONENT( LArRawChannelBuilderIterAlg )
DECLARE_COMPONENT( LArNNRawChannelBuilder )
DECLARE_COMPONENT( LArSCSimpleMaker )
DECLARE_COMPONENT( LArSuperCellBCIDEmAlg )
DECLARE_COMPONENT( LArSuperCellBCIDAlg )
DECLARE_COMPONENT( LArHITtoCell )

#include "../tests/SuperCellVsCaloCellTestAlg.h"
DECLARE_COMPONENT( SuperCellVsCaloCellTestAlg )

