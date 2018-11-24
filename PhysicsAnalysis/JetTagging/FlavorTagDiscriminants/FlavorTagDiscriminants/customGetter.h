/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes
#include "xAODJet/Jet.h"

#include <functional>
#include <string>

#ifndef CUSTOM_GETTER_H
#define CUSTOM_GETTER_H

namespace FlavorTagDiscriminants {
  std::function<std::pair<std::string, double>(const xAOD::Jet&)>
  customGetterAndName(const std::string&);

  std::function<std::pair<std::string, std::vector<double>>(
    const xAOD::Jet&,
    const std::vector<const xAOD::TrackParticle*>&)>
  customNamedSeqGetter(const std::string&);
}

#endif
