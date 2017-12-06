#include "LArByteStream/LArRawChannelContByteStreamCnv.h"
#include "LArByteStream/LArDigitContByteStreamCnv.h"
#include "LArByteStream/LArCalibDigitContByteStreamCnv.h"
#include "LArByteStream/LArAccumulatedCalibDigitContByteStreamCnv.h"
#include "LArByteStream/LArAccumulatedDigitContByteStreamCnv.h"
#include "LArByteStream/LArFebHeaderContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"
#include "LArByteStream/LArRodDecoder.h"
#include "ByteStreamCnvSvcBase/CollectionByteStreamCnv.h"
//#include "LArByteStream/LArRawChannelCollByteStreamTool.h"

// Containers 
DECLARE_COMPONENT( LArRawDataContByteStreamTool )
DECLARE_COMPONENT( LArRodDecoder )
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
//DECLARE_COMPONENT( LARCELLTOOL )  
//DECLARE_CONVERTER_FACTORY( LArCellCollByteStreamCnv )

