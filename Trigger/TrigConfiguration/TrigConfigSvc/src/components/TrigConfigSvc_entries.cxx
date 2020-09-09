#include "../L1TopoConfigSvc.h"
#include "../LVL1ConfigSvc.h"
#include "../DSConfigSvc.h"
#include "../HLTConfigSvc.h"
#include "../TrigConfJobOptionsSvc.h"
#include "../TrigConfigSvc.h"

#include "../L1PrescaleCondAlg.h"
#include "../HLTPrescaleCondAlg.h"

#include "../AlgToChainTool.h"

DECLARE_COMPONENT( TrigConf::L1TopoConfigSvc )
DECLARE_COMPONENT( TrigConf::LVL1ConfigSvc )
DECLARE_COMPONENT( TrigConf::HLTConfigSvc )
DECLARE_COMPONENT( TrigConf::JobOptionsSvc )
DECLARE_COMPONENT( TrigConf::DSConfigSvc )
DECLARE_COMPONENT( TrigConf::TrigConfigSvc )

DECLARE_COMPONENT( TrigConf::L1PrescaleCondAlg )
DECLARE_COMPONENT( TrigConf::HLTPrescaleCondAlg )

DECLARE_COMPONENT( TrigConf::AlgToChainTool )