#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LUCID_RawDataByteStreamCnv/LUCID_ByteStreamRawDataCnv.h"
DECLARE_ALGORITHM_FACTORY ( LUCID_ByteStreamRawDataCnv )

#include "LUCID_RawDataByteStreamCnv/LUCID_DigitRawDataCnv.h"
DECLARE_ALGORITHM_FACTORY ( LUCID_DigitRawDataCnv )

#include "LUCID_RawDataByteStreamCnv/LUCID_DigitByteStreamCnv.h"
DECLARE_CONVERTER_FACTORY ( LUCID_DigitByteStreamCnv )

DECLARE_FACTORY_ENTRIES( LUCID_RawDataByteStreamCnv ) {
  DECLARE_ALGORITHM ( LUCID_ByteStreamRawDataCnv )
  DECLARE_ALGORITHM ( LUCID_DigitRawDataCnv )
  DECLARE_CONVERTER ( LUCID_DigitByteStreamCnv )
}
