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

DECLARE_COMPONENT( AthenaOutputStream )
DECLARE_COMPONENT( AthenaConditionStream )
DECLARE_COMPONENT( TestRandomSeqAlg )
DECLARE_COMPONENT( MultipleEventLoopMgr )
DECLARE_COMPONENT( AthenaEventLoopMgr )
DECLARE_COMPONENT( AthenaHiveEventLoopMgr )
DECLARE_COMPONENT( PyAthenaEventLoopMgr )
DECLARE_COMPONENT( SimplePOSIXTimeKeeperSvc )
DECLARE_COMPONENT( MixingEventSelector )
DECLARE_COMPONENT( ThinningSvc )
//DECLARE_COMPONENT( EventDumperSvc )
DECLARE_COMPONENT( MemoryRescueSvc )
DECLARE_COMPONENT( FPEControlSvc )
DECLARE_COMPONENT( JobIDSvc )
DECLARE_COMPONENT( CoreDumpSvc )
DECLARE_COMPONENT( PageAccessControlSvc )
DECLARE_COMPONENT( AthDictLoaderSvc )
DECLARE_COMPONENT( AthenaSealSvc )
DECLARE_COMPONENT( DecisionSvc )
DECLARE_COMPONENT( ItemListSvc )
DECLARE_COMPONENT( AthenaSummarySvc )
DECLARE_COMPONENT( LoggedMessageSvc )
DECLARE_COMPONENT( Athena::JobOptionsSvc )
DECLARE_COMPONENT( Athena::RCUSvc )
DECLARE_COMPONENT( AthTPCnvSvc )
DECLARE_COMPONENT( EvtIdModifierSvc )
DECLARE_COMPONENT( MetaDataSvc )
DECLARE_COMPONENT( OutputStreamSequencerSvc )
DECLARE_COMPONENT( AthenaOutputStreamTool )
DECLARE_COMPONENT( ThinningOutputTool )
DECLARE_COMPONENT( AthenaStopperAlg )
DECLARE_COMPONENT( AthIncFirerAlg )
