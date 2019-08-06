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
#include "../LArRawChannelBSReadAlg.h"
//#include "LArByteStream/LArRawChannelCollByteStreamTool.h"

// Containers 
DECLARE_COMPONENT( LArRawDataContByteStreamTool )
DECLARE_COMPONENT( LArRodDecoder )
DECLARE_COMPONENT( LArABBADecoder )
DECLARE_COMPONENT( LArRawChannelBSReadAlg )

DECLARE_CONVERTER( LArRawChannelContByteStreamCnv )
DECLARE_CONVERTER( LArDigitContByteStreamCnv )
DECLARE_CONVERTER( LArCalibDigitContByteStreamCnv )
DECLARE_CONVERTER( LArAccumulatedCalibDigitContByteStreamCnv )
DECLARE_CONVERTER( LArAccumulatedDigitContByteStreamCnv )
DECLARE_CONVERTER( LArFebHeaderContByteStreamCnv )

// Collections

// for LArCell 
//typedef LArRawChannelCollByteStreamTool<ROBData,LArCellCollection>
//							 LARCELLTOOL;
//typedef CollectionByteStreamCnv<LARCELLTOOL> LArCellCollByteStreamCnv ;
//DECLARE_COMPONENT( LARCELLTOOL )  
//DECLARE_CONVERTER( LArCellCollByteStreamCnv )

