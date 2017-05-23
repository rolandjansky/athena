#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArByteStream/LArRawChannelContByteStreamCnv.h"
#include "LArByteStream/LArDigitContByteStreamCnv.h"
#include "LArByteStream/LArCalibDigitContByteStreamCnv.h"
#include "LArByteStream/LArAccumulatedCalibDigitContByteStreamCnv.h"
#include "LArByteStream/LArAccumulatedDigitContByteStreamCnv.h"
#include "LArByteStream/LArFebHeaderContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"
#include "LArByteStream/LArRodDecoder.h"
#include "LArByteStream/LArABBADecoder.h"
#include "ByteStreamCnvSvcBase/CollectionByteStreamCnv.h"
//#include "LArByteStream/LArRawChannelCollByteStreamTool.h"

// Containers 
DECLARE_TOOL_FACTORY( LArRawDataContByteStreamTool )
DECLARE_TOOL_FACTORY( LArRodDecoder )
DECLARE_TOOL_FACTORY( LArABBADecoder )
DECLARE_CONVERTER_FACTORY( LArRawChannelContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LArDigitContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LArCalibDigitContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LArAccumulatedCalibDigitContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LArAccumulatedDigitContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LArFebHeaderContByteStreamCnv )

// Collections

// for LArCell 
//typedef LArRawChannelCollByteStreamTool<ROBData,LArCellCollection>
//							 LARCELLTOOL;
//typedef CollectionByteStreamCnv<LARCELLTOOL> LArCellCollByteStreamCnv ;
//DECLARE_TOOL_FACTORY( LARCELLTOOL)  
//DECLARE_CONVERTER_FACTORY( LArCellCollByteStreamCnv )

DECLARE_FACTORY_ENTRIES(LArByteStream) {
    DECLARE_CONVERTER(  LArRawChannelContByteStreamCnv )
    DECLARE_CONVERTER(  LArDigitContByteStreamCnv )
    DECLARE_CONVERTER(  LArCalibDigitContByteStreamCnv )
    DECLARE_CONVERTER(  LArAccumulatedCalibDigitContByteStreamCnv )
    DECLARE_CONVERTER(  LArAccumulatedDigitContByteStreamCnv )
    DECLARE_CONVERTER( LArFebHeaderContByteStreamCnv )
    DECLARE_TOOL ( LArRawDataContByteStreamTool )
    DECLARE_TOOL ( LArRodDecoder)    

//    DECLARE_CONVERTER( LArRawChannelCollByteStreamCnv )
//    DECLARE_CONVERTER( LArCellCollByteStreamCnv )
//    DECLARE_TOOL( LARRAWTOOL) 
//    DECLARE_TOOL( LARCELLTOOL)  
}
