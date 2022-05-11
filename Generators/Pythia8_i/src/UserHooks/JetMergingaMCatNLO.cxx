/*
     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8Plugins/aMCatNLOHooks.h"

namespace Pythia8{
  Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::amcnlo_unitarised_interface> amcnlo_unitarised_interface_creator("JetMergingaMCatNLO");
}
