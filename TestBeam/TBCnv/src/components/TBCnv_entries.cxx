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

DECLARE_COMPONENT( TBByteStreamCnvTool )

DECLARE_CONVERTER( TBTDCCnv )
DECLARE_CONVERTER( TBTDCRawContCnv )
DECLARE_CONVERTER( TBLArDigitContainerCnv )
DECLARE_CONVERTER( TBLArCalibDigitContainerCnv )
DECLARE_CONVERTER( TBADCRawContCnv )
DECLARE_CONVERTER( TBBPCRawContCnv )
DECLARE_CONVERTER( TBScintillatorRawContCnv )
DECLARE_CONVERTER( TBTriggerPatternUnitCnv )
DECLARE_CONVERTER( TBTailCatcherRawCnv )
DECLARE_CONVERTER( TBEventInfoCnv )

DECLARE_COMPONENT( ReadTBLArDigits )
DECLARE_COMPONENT( ReadTBLArCalibDigits )
DECLARE_COMPONENT( TBCheckBCIDs )
 

