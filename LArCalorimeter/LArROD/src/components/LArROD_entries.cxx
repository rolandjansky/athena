#include "LArROD/LArRawChannelBuilder.h"
#include "LArROD/ReadLArRaw.h"
#include "LArROD/ReadLArDigits.h"
#include "LArROD/LArDigitPreprocessor.h"
#include "LArROD/LArCalibDigitPreProcessor.h"
#include "LArROD/LArRawChannelSimpleBuilder.h"
#include "LArROD/LArTimeChecker.h"
#include "LArROD/LArCalibDigitsAccumulator.h"
#include "LArROD/LArDigitsAccumulator.h"
#include "LArROD/LArRawChannelBuilderDriver.h"
#include "LArROD/LArCellBuilderDriver.h"
#include "LArROD/LArSuperCellBuilderDriver.h"
#include "LArROD/LArRawChannelBuilderToolOFC.h"
#include "LArROD/LArRawChannelBuilderToolOFCIter.h"
#include "LArROD/LArRawChannelBuilderToolParabola.h"
#include "LArROD/LArRawChannelBuilderToolCubic.h"
#include "LArROD/LArRawChannelBuilderToolAverage.h"
#include "LArROD/LArRawChannelBuilderToolBadChannelTool.h"
#include "LArROD/LArRawChannelBuilderADC2EDataBase.h"
#include "LArROD/LArRawChannelBuilderADC2EConstants.h"
#include "LArROD/LArRawChannelBuilderPedestalDataBase.h"
#include "LArROD/LArRawChannelBuilderPedestalSampleZero.h"
#include "LArROD/LArRawChannelBuilderToolTileInfo.h"
#include "LArROD/LArDigitThinner.h"
#include "LArROD/LArFebErrorSummaryMaker.h"
#include "LArROD/LArCalibDigitsAccumulatorFreeGain.h"


typedef LArDigitPreProcessor<LArDigitContainer> LArDigProcessor;
typedef LArDigitPreProcessor<TBLArDigitContainer> TBLArDigProcessor;

typedef LArCalibDigitPreProcessor<TBLArCalibDigitContainer> TBLArCalDigProcessor;

DECLARE_COMPONENT( LArRawChannelBuilder )
DECLARE_COMPONENT( ReadLArRaw )
DECLARE_COMPONENT( ReadLArDigits )
DECLARE_COMPONENT( LArDigProcessor )
DECLARE_COMPONENT( TBLArDigProcessor )
DECLARE_COMPONENT( TBLArCalDigProcessor )
DECLARE_COMPONENT( LArRawChannelSimpleBuilder )
DECLARE_COMPONENT( LArTimeChecker )
DECLARE_COMPONENT( LArCalibDigitsAccumulator )
DECLARE_COMPONENT( LArDigitsAccumulator )
DECLARE_COMPONENT( LArRawChannelBuilderDriver )
DECLARE_COMPONENT( LArDigitThinner )
DECLARE_COMPONENT( LArFebErrorSummaryMaker )
DECLARE_COMPONENT( LArCalibDigitsAccumulatorFreeGain )
DECLARE_COMPONENT( LArSuperCellBuilderDriver )
DECLARE_COMPONENT( LArCellBuilderDriver )
DECLARE_COMPONENT( LArRawChannelBuilderToolOFC )
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


#include "../tests/SuperCellVsCaloCellTestAlg.h"
DECLARE_COMPONENT( SuperCellVsCaloCellTestAlg )

