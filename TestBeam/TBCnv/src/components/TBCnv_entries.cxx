#include "TBCnv/TBDataCnv.h"
#include "TBCnv/TBByteStreamCnvTool.h"
#include "TBCnv/ReadTBLArDigits.h"
#include "TBCnv/ReadTBLArCalibDigits.h"
#include "TBCnv/TBCheckBCIDs.h"

#include "TBEvent/TBTDC.h"
#include "TBEvent/TBBPCRawCont.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBScintillatorRawCont.h"
#include "TBEvent/TBTailCatcherRaw.h"
#include "TBEvent/TBADCRawCont.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBLArDigitContainer.h"
#include "TBEvent/TBLArCalibDigitContainer.h"
#include "TBEvent/TBEventInfo.h"


typedef TBDataCnv<TBTDC> TBTDCCnv;
typedef TBDataCnv<TBTDCRawCont> TBTDCRawContCnv;
typedef TBDataCnv<TBADCRawCont> TBADCRawContCnv;
typedef TBDataCnv<TBLArDigitContainer> TBLArDigitContainerCnv;
typedef TBDataCnv<TBLArCalibDigitContainer> TBLArCalibDigitContainerCnv;
typedef TBDataCnv<TBBPCRawCont> TBBPCRawContCnv;
typedef TBDataCnv<TBScintillatorRawCont> TBScintillatorRawContCnv;
typedef TBDataCnv<TBTriggerPatternUnit> TBTriggerPatternUnitCnv;
typedef TBDataCnv<TBTailCatcherRaw> TBTailCatcherRawCnv;
typedef TBDataCnv<TBEventInfo> TBEventInfoCnv;

DECLARE_TOOL_FACTORY( TBByteStreamCnvTool )
DECLARE_CONVERTER_FACTORY( TBTDCCnv )
DECLARE_CONVERTER_FACTORY( TBTDCRawContCnv )
DECLARE_CONVERTER_FACTORY( TBLArDigitContainerCnv )
DECLARE_CONVERTER_FACTORY( TBLArCalibDigitContainerCnv )
DECLARE_CONVERTER_FACTORY( TBADCRawContCnv )
DECLARE_CONVERTER_FACTORY( TBBPCRawContCnv )
DECLARE_CONVERTER_FACTORY( TBScintillatorRawContCnv )
DECLARE_CONVERTER_FACTORY( TBTriggerPatternUnitCnv )
DECLARE_CONVERTER_FACTORY( TBTailCatcherRawCnv )
DECLARE_CONVERTER_FACTORY( TBEventInfoCnv )
DECLARE_ALGORITHM_FACTORY( ReadTBLArDigits )
DECLARE_ALGORITHM_FACTORY( ReadTBLArCalibDigits )
DECLARE_ALGORITHM_FACTORY( TBCheckBCIDs )
 

