
#include "TrigT1ResultByteStream/RoIBResultByteStreamCnv.h"
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamCnv.h"

#include "TrigT1ResultByteStream/MuCTPIByteStreamCnv.h"
#include "TrigT1ResultByteStream/CTPByteStreamCnv.h"
#include "TrigT1ResultByteStream/RecMuCTPIByteStreamCnv.h"
#include "TrigT1ResultByteStream/RecCTPByteStreamCnv.h"

#include "TrigT1ResultByteStream/CTPByteStreamTool.h"
#include "TrigT1ResultByteStream/MuCTPIByteStreamTool.h"
#include "TrigT1ResultByteStream/RecCTPByteStreamTool.h"
#include "TrigT1ResultByteStream/RecMuCTPIByteStreamTool.h"
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamTool.h"
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"

// ROBF for offline
typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF; 

// converter for offline 
typedef RoIBResultByteStreamCnv<ROBF> RoIBResultByteStreamCnvT ;
typedef RecRoIBResultByteStreamCnv<ROBF> RecRoIBResultByteStreamCnvT ;

// NOTE TO L2: need to define a ROBFragment for L2, and instantiate the
//             converter. 

// declare 
DECLARE_CONVERTER_FACTORY( RoIBResultByteStreamCnvT )
DECLARE_CONVERTER_FACTORY( RecRoIBResultByteStreamCnvT )
DECLARE_CONVERTER_FACTORY( MuCTPIByteStreamCnv )
DECLARE_CONVERTER_FACTORY( CTPByteStreamCnv )
DECLARE_CONVERTER_FACTORY( RecMuCTPIByteStreamCnv )
DECLARE_CONVERTER_FACTORY( RecCTPByteStreamCnv )

DECLARE_COMPONENT( CTPByteStreamTool )
DECLARE_COMPONENT( MuCTPIByteStreamTool )
DECLARE_COMPONENT( RecCTPByteStreamTool )
DECLARE_COMPONENT( RecMuCTPIByteStreamTool )
DECLARE_COMPONENT( RecRoIBResultByteStreamTool )
DECLARE_COMPONENT( RoIBResultByteStreamTool )

