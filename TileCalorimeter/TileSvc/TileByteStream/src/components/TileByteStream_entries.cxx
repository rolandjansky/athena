#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileByteStream/TileRawChannelContByteStreamCnv.h"
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileByteStream/TileDigitsContByteStreamCnv.h"
#include "TileByteStream/TileL2ContByteStreamCnv.h"
#include "TileByteStream/TileLaserObjByteStreamCnv.h"
#include "TileByteStream/TileMuRcvContByteStreamCnv.h"
#include "TileByteStream/TileROD_Decoder.h"

#include "TileByteStream/TileDigitsContByteStreamTool.h"
#include "TileByteStream/TileRawChannelContByteStreamTool.h"
#include "TileByteStream/TileL2ContByteStreamTool.h"
#include "TileByteStream/TileLaserObjByteStreamTool.h"
#include "TileByteStream/TileMuRcvContByteStreamTool.h"

DECLARE_TOOL_FACTORY( TileDigitsContByteStreamTool )
DECLARE_TOOL_FACTORY( TileRawChannelContByteStreamTool )
DECLARE_TOOL_FACTORY( TileL2ContByteStreamTool )
DECLARE_TOOL_FACTORY( TileLaserObjByteStreamTool )
DECLARE_TOOL_FACTORY( TileMuRcvContByteStreamTool )
DECLARE_TOOL_FACTORY( TileROD_Decoder )
DECLARE_CONVERTER_FACTORY( TileRawChannelContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( TileBeamElemContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( TileDigitsContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( TileL2ContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( TileLaserObjByteStreamCnv )
DECLARE_CONVERTER_FACTORY( TileMuRcvContByteStreamCnv )

DECLARE_FACTORY_ENTRIES(TileByteStream) {
    DECLARE_CONVERTER(  TileRawChannelContByteStreamCnv )
    DECLARE_CONVERTER(  TileBeamElemContByteStreamCnv )
    DECLARE_CONVERTER(  TileDigitsContByteStreamCnv )
    DECLARE_CONVERTER(  TileL2ContByteStreamCnv )
    DECLARE_CONVERTER(  TileLaserObjByteStreamCnv )
    DECLARE_CONVERTER(  TileMuRcvContByteStreamCnv )
    DECLARE_TOOL( TileDigitsContByteStreamTool )
    DECLARE_TOOL( TileRawChannelContByteStreamTool )
    DECLARE_TOOL( TileL2ContByteStreamTool )
    DECLARE_TOOL( TileLaserObjByteStreamTool )
    DECLARE_TOOL( TileMuRcvContByteStreamTool )
    DECLARE_TOOL( TileROD_Decoder )
}
