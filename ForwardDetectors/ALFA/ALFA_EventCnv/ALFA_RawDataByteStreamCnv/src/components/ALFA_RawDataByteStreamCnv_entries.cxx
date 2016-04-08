#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProvider.h"
DECLARE_ALGORITHM_FACTORY( ALFA_RawDataProvider)  

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProvider_charge.h"
DECLARE_ALGORITHM_FACTORY( ALFA_RawDataProvider_charge)

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProviderTool.h"
DECLARE_TOOL_FACTORY( ALFA_RawDataProviderTool)  

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProviderTool_charge.h"
DECLARE_TOOL_FACTORY( ALFA_RawDataProviderTool_charge)

#include "ALFA_RawDataByteStreamCnv/ALFA_Decoder.h"
DECLARE_TOOL_FACTORY( ALFA_Decoder )

#include "ALFA_RawDataByteStreamCnv/ALFA_Decoder_charge.h"
DECLARE_TOOL_FACTORY( ALFA_Decoder_charge )

DECLARE_FACTORY_ENTRIES( ALFA_RawDataByteStreamCnv ) {
DECLARE_ALGORITHM ( ALFA_RawDataProvider )
DECLARE_ALGORITHM ( ALFA_RawDataProvider_charge )
DECLARE_TOOL      ( ALFA_RawDataProviderTool )
DECLARE_TOOL      ( ALFA_RawDataProviderTool_charge )
DECLARE_TOOL      ( ALFA_Decoder )
DECLARE_TOOL      ( ALFA_Decoder_charge )
}


