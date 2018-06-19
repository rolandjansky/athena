/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8Plugins/PowhegHooks.h"

namespace Pythia8{
  Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::PowhegHooks> PowhegHooksCreator("PowhegMain31");
}


