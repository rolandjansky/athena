#include "../AthenaEventLoopMgr.h"
#include "../PyAthenaEventLoopMgr.h"
#include "../AthenaOutputStream.h"
#include "../AthenaOutputStreamTool.h"
#include "../AthenaConditionStream.h"
#include "../MultipleEventLoopMgr.h"
#include "../SimplePOSIXTimeKeeperSvc.h"
#include "../MixingEventSelector.h"
#include "../ThinningSvc.h"
#include "../ThinningOutputTool.h"
//#include "../EventDumperSvc.h"
#include "../MemoryRescueSvc.h"
#include "../FPEControlSvc.h"
#include "../JobIDSvc.h"
#include "../UserDataSvc.h"
#include "../CoreDumpSvc.h"
#include "../AthDictLoaderSvc.h"
#include "../AthenaSealSvc.h"
#include "../PageAccessControlSvc.h"
#include "../DecisionSvc.h"
#include "../ItemListSvc.h"
#include "../AthenaSummarySvc.h"
#include "../LoggedMessageSvc.h"
#include "../AthenaJobOptionsSvc.h"
#include "../RCUSvc.h"
#include "../AthTPCnvSvc.h"
#include "../EvtIdModifierSvc.h"
#include "../TestRandomSeqAlg.h"
#include "../MetaDataSvc.h"
#include "../OutputStreamSequencerSvc.h"
#include "../AthenaStopperAlg.h"
#include "../AthenaHiveEventLoopMgr.h"
#include "../AthIncFirerAlg.h"

DECLARE_ALGORITHM_FACTORY( AthenaOutputStream )
DECLARE_ALGORITHM_FACTORY( AthenaConditionStream )
DECLARE_ALGORITHM_FACTORY( TestRandomSeqAlg )
DECLARE_SERVICE_FACTORY( MultipleEventLoopMgr )
DECLARE_SERVICE_FACTORY( AthenaEventLoopMgr )
DECLARE_SERVICE_FACTORY( AthenaHiveEventLoopMgr )
DECLARE_SERVICE_FACTORY( PyAthenaEventLoopMgr )
DECLARE_SERVICE_FACTORY( SimplePOSIXTimeKeeperSvc )
DECLARE_SERVICE_FACTORY( MixingEventSelector )
DECLARE_SERVICE_FACTORY( ThinningSvc )
//DECLARE_SERVICE_FACTORY( EventDumperSvc )
DECLARE_SERVICE_FACTORY( MemoryRescueSvc )
DECLARE_SERVICE_FACTORY( FPEControlSvc )
DECLARE_SERVICE_FACTORY( JobIDSvc )
DECLARE_SERVICE_FACTORY( UserDataSvc )
DECLARE_SERVICE_FACTORY( CoreDumpSvc )
DECLARE_SERVICE_FACTORY( PageAccessControlSvc )
DECLARE_SERVICE_FACTORY( AthDictLoaderSvc )
DECLARE_SERVICE_FACTORY( AthenaSealSvc )
DECLARE_SERVICE_FACTORY( DecisionSvc )
DECLARE_SERVICE_FACTORY( ItemListSvc )
DECLARE_SERVICE_FACTORY( AthenaSummarySvc )
DECLARE_SERVICE_FACTORY( LoggedMessageSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( Athena, JobOptionsSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( Athena, RCUSvc )
DECLARE_SERVICE_FACTORY( AthTPCnvSvc )
DECLARE_SERVICE_FACTORY( EvtIdModifierSvc )
DECLARE_SERVICE_FACTORY( MetaDataSvc )
DECLARE_SERVICE_FACTORY( OutputStreamSequencerSvc )
DECLARE_TOOL_FACTORY( AthenaOutputStreamTool )
DECLARE_TOOL_FACTORY( ThinningOutputTool )
DECLARE_ALGORITHM_FACTORY( AthenaStopperAlg )
DECLARE_ALGORITHM_FACTORY( AthIncFirerAlg )

