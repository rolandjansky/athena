#include "../L1TriggerResultByteStreamCnv.h"
#include "../RoIBResultByteStreamCnv.h"

#include "../MuCTPIByteStreamCnv.h"
#include "../CTPByteStreamCnv.h"
#include "../RecMuCTPIByteStreamCnv.h"
#include "../RecCTPByteStreamCnv.h"

#include "../CTPByteStreamTool.h"
#include "../MuCTPIByteStreamTool.h"
#include "../RecCTPByteStreamTool.h"
#include "../RecMuCTPIByteStreamTool.h"
#include "../RoIBResultByteStreamTool.h"

#include "../L1TriggerByteStreamDecoderAlg.h"
#include "../ExampleL1TriggerByteStreamTool.h"
#include "../MuonRoIByteStreamTool.h"

// ROBF for offline
typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF; 

// converter for offline 
typedef RoIBResultByteStreamCnv<ROBF> RoIBResultByteStreamCnvT ;

// declare 
DECLARE_CONVERTER( L1TriggerResultByteStreamCnv )
DECLARE_CONVERTER( RoIBResultByteStreamCnvT )
DECLARE_CONVERTER( MuCTPIByteStreamCnv )
DECLARE_CONVERTER( CTPByteStreamCnv )
DECLARE_CONVERTER( RecMuCTPIByteStreamCnv )
DECLARE_CONVERTER( RecCTPByteStreamCnv )

DECLARE_COMPONENT( CTPByteStreamTool )
DECLARE_COMPONENT( MuCTPIByteStreamTool )
DECLARE_COMPONENT( RecCTPByteStreamTool )
DECLARE_COMPONENT( RecMuCTPIByteStreamTool )
DECLARE_COMPONENT( RoIBResultByteStreamTool )

DECLARE_COMPONENT( L1TriggerByteStreamDecoderAlg )
DECLARE_COMPONENT( ExampleL1TriggerByteStreamTool )
DECLARE_COMPONENT( MuonRoIByteStreamTool )
