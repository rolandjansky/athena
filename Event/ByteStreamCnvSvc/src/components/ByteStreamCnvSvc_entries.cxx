#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "../EventSelectorByteStream.h"
#include "../ByteStreamMergeOutputSvc.h"
#include "../EventInfoByteStreamCnv.h"
#include "../ByteStreamRDP_OutputSvc.h"
#include "../ByteStreamEventStorageInputSvc.h"
#include "../ByteStreamEventStorageOutputSvc.h"
#include "../ByteStreamSingleEventInputSvc.h"
#include "../ByteStreamNavigationProviderSvc.h"
#include "../ByteStreamOutputStreamCopyTool.h"
#include "../ByteStreamMetadataTool.h"
#include "../ByteStreamAttListMetadataSvc.h"

DECLARE_CONVERTER_FACTORY( EventInfoByteStreamCnv )
DECLARE_COMPONENT( ByteStreamCnvSvc )
DECLARE_COMPONENT( EventSelectorByteStream )
DECLARE_COMPONENT( ByteStreamMergeOutputSvc )
DECLARE_COMPONENT( ByteStreamRDP_OutputSvc )
DECLARE_COMPONENT( ByteStreamEventStorageInputSvc )
DECLARE_COMPONENT( ByteStreamEventStorageOutputSvc )
DECLARE_COMPONENT( ByteStreamSingleEventInputSvc )
DECLARE_COMPONENT( ByteStreamNavigationProviderSvc )
DECLARE_COMPONENT( ByteStreamAttListMetadataSvc )

DECLARE_COMPONENT( ByteStreamOutputStreamCopyTool )
DECLARE_COMPONENT( ByteStreamMetadataTool )

