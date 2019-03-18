/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_JETTAGUTILS
#define JETTAGTOOLS_JETTAGUTILS
#include "xAODJet/Jet.h"
#include <string>

namespace JetTagUtils{

  std::string getJetAuthor(const xAOD::Jet * jetToTag);

}

#endif
