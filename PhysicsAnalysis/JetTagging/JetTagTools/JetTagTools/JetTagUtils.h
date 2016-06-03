/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_JETTAGUTILS
#define JETTAGTOOLS_JETTAGUTILS
#include "xAODJet/Jet.h"
#include <string>

namespace JetTagUtils{

  std::string getJetAuthor(xAOD::Jet& jetToTag);

}

#endif
