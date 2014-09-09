#include "GaudiKernel/DeclareFactoryEntries.h"

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
#include "../AthTPCnvSvc.h"
#include "../EvtIdModifierSvc.h"
#include "../TestRandomSeqAlg.h"
#include "../MetaDataSvc.h"
#include "../OutputStreamSequencerSvc.h"
#include "../AthenaSharedMemoryTool.h"
#include "../AthenaYamplTool.h"

DECLARE_FACTORY_ENTRIES(AthenaServices) {
    DECLARE_ALGORITHM( AthenaOutputStream )
    DECLARE_ALGORITHM( AthenaConditionStream )
    DECLARE_ALGORITHM( TestRandomSeqAlg )
    DECLARE_SERVICE( AthenaEventLoopMgr )
    DECLARE_SERVICE( MultipleEventLoopMgr )
    DECLARE_SERVICE( PyAthenaEventLoopMgr )
    DECLARE_SERVICE( SimplePOSIXTimeKeeperSvc )
    DECLARE_SERVICE( MixingEventSelector )
    DECLARE_SERVICE( ThinningSvc )
//       DECLARE_SERVICE( EventDumperSvc )
    DECLARE_SERVICE( MemoryRescueSvc )
    DECLARE_SERVICE( FPEControlSvc )
    DECLARE_SERVICE( JobIDSvc )
    DECLARE_SERVICE( UserDataSvc )
    DECLARE_SERVICE( CoreDumpSvc )
    DECLARE_SERVICE( PageAccessControlSvc )
    DECLARE_SERVICE( AthDictLoaderSvc )
    DECLARE_SERVICE( AthenaSealSvc )
    DECLARE_SERVICE( DecisionSvc )
    DECLARE_SERVICE( ItemListSvc )
    DECLARE_SERVICE( AthenaSummarySvc )
    DECLARE_SERVICE( LoggedMessageSvc )
    DECLARE_NAMESPACE_SERVICE(Athena, JobOptionsSvc)
    DECLARE_SERVICE( AthTPCnvSvc )
    DECLARE_SERVICE( EvtIdModifierSvc )
    DECLARE_SERVICE( MetaDataSvc )
    DECLARE_SERVICE( OutputStreamSequencerSvc )

    DECLARE_TOOL( AthenaOutputStreamTool )
    DECLARE_TOOL( ThinningOutputTool )
    DECLARE_TOOL( AthenaSharedMemoryTool )
    DECLARE_TOOL( AthenaYamplTool )

}
DECLARE_ALGORITHM_FACTORY( AthenaOutputStream )
DECLARE_ALGORITHM_FACTORY( AthenaConditionStream )
DECLARE_ALGORITHM_FACTORY( TestRandomSeqAlg )
DECLARE_SERVICE_FACTORY( AthenaEventLoopMgr )
DECLARE_SERVICE_FACTORY( MultipleEventLoopMgr )
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
DECLARE_SERVICE_FACTORY( AthTPCnvSvc )
DECLARE_SERVICE_FACTORY( EvtIdModifierSvc )
DECLARE_SERVICE_FACTORY( MetaDataSvc )
DECLARE_SERVICE_FACTORY( OutputStreamSequencerSvc )
DECLARE_TOOL_FACTORY( AthenaOutputStreamTool )
DECLARE_TOOL_FACTORY( ThinningOutputTool )
DECLARE_TOOL_FACTORY( AthenaSharedMemoryTool )
DECLARE_TOOL_FACTORY( AthenaYamplTool )


