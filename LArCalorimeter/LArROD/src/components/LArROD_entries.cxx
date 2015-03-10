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

#include "GaudiKernel/DeclareFactoryEntries.h"

typedef LArDigitPreProcessor<LArDigitContainer> LArDigProcessor;
typedef LArDigitPreProcessor<TBLArDigitContainer> TBLArDigProcessor;

typedef LArCalibDigitPreProcessor<TBLArCalibDigitContainer> TBLArCalDigProcessor;

DECLARE_ALGORITHM_FACTORY(LArRawChannelBuilder )
DECLARE_ALGORITHM_FACTORY(ReadLArRaw )
DECLARE_ALGORITHM_FACTORY(ReadLArDigits )
DECLARE_ALGORITHM_FACTORY(LArDigProcessor )
DECLARE_ALGORITHM_FACTORY(TBLArDigProcessor )
DECLARE_ALGORITHM_FACTORY(TBLArCalDigProcessor )
DECLARE_ALGORITHM_FACTORY(LArRawChannelSimpleBuilder )
DECLARE_ALGORITHM_FACTORY(LArTimeChecker )
DECLARE_ALGORITHM_FACTORY(LArCalibDigitsAccumulator)
DECLARE_ALGORITHM_FACTORY(LArDigitsAccumulator)
DECLARE_ALGORITHM_FACTORY(LArRawChannelBuilderDriver)
DECLARE_ALGORITHM_FACTORY(LArDigitThinner)
DECLARE_ALGORITHM_FACTORY(LArFebErrorSummaryMaker)
DECLARE_ALGORITHM_FACTORY(LArCalibDigitsAccumulatorFreeGain)
DECLARE_ALGORITHM_FACTORY( LArSuperCellBuilderDriver)
DECLARE_TOOL_FACTORY( LArCellBuilderDriver)
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolOFC )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolOFCIter )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolParabola )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolCubic )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolAverage )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolBadChannelTool )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderADC2EDataBase )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderADC2EConstants )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderPedestalDataBase )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderPedestalSampleZero )
DECLARE_TOOL_FACTORY( LArRawChannelBuilderToolTileInfo )


#include "../tests/SuperCellVsCaloCellTestAlg.h"
DECLARE_ALGORITHM_FACTORY( SuperCellVsCaloCellTestAlg )

DECLARE_FACTORY_ENTRIES(LArROD) {
  DECLARE_ALGORITHM( SuperCellVsCaloCellTestAlg );
    DECLARE_ALGORITHM(LArRawChannelBuilder )
    DECLARE_ALGORITHM(ReadLArRaw )
    DECLARE_ALGORITHM(ReadLArDigits )
    DECLARE_ALGORITHM(LArDigProcessor )
    DECLARE_ALGORITHM(TBLArDigProcessor )
    DECLARE_ALGORITHM(TBLArCalDigProcessor )
    DECLARE_ALGORITHM(LArRawChannelSimpleBuilder )
    DECLARE_ALGORITHM(LArTimeChecker )
    DECLARE_ALGORITHM(LArCalibDigitsAccumulator )
    DECLARE_ALGORITHM(LArDigitsAccumulator )
    DECLARE_ALGORITHM(LArRawChannelBuilderDriver)
    DECLARE_ALGORITHM(LArDigitThinner)
    DECLARE_ALGORITHM(LArFebErrorSummaryMaker)
    DECLARE_ALGORITHM(LArCalibDigitsAccumulatorFreeGain)
    DECLARE_ALGORITHM( LArSuperCellBuilderDriver)
    DECLARE_TOOL( LArCellBuilderDriver)
    DECLARE_TOOL( LArRawChannelBuilderToolOFC )
    DECLARE_TOOL( LArRawChannelBuilderToolOFCIter )
    DECLARE_TOOL( LArRawChannelBuilderToolParabola )
    DECLARE_TOOL( LArRawChannelBuilderToolCubic )
    DECLARE_TOOL( LArRawChannelBuilderToolAverage )
    DECLARE_TOOL( LArRawChannelBuilderToolBadChannelTool )
    DECLARE_TOOL( LArRawChannelBuilderADC2EDataBase )
    DECLARE_TOOL( LArRawChannelBuilderADC2EConstants )
    DECLARE_TOOL( LArRawChannelBuilderPedestalDataBase )
    DECLARE_TOOL( LArRawChannelBuilderPedestalSampleZero )
    DECLARE_TOOL( LArRawChannelBuilderToolTileInfo )  
}

