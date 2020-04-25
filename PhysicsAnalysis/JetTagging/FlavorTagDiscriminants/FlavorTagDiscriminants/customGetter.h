/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// The customGetter file is a catch-all for various getter functinos
// that need to be hard coded for whatever reason. Some of these are
// accessing methods like `pt` which have no name in the EDM, others
// can't be stored in the edm directly for various reasons.


// EDM includes
#include "xAODJet/Jet.h"

#include <functional>
#include <string>

#ifndef CUSTOM_GETTER_H
#define CUSTOM_GETTER_H

namespace FlavorTagDiscriminants {

  using VectorTP = std::vector<const xAOD::TrackParticle*>;
  using VectorD = std::vector<double>;
  std::function<VectorD(const xAOD::Jet&,const VectorTP&)> customSeqGetter(
    const std::string& name);

  namespace internal {
    std::function<std::pair<std::string, double>(const xAOD::Jet&)>
    customGetterAndName(const std::string&);

    std::function<std::pair<std::string, std::vector<double>>(
      const xAOD::Jet&,
      const std::vector<const xAOD::TrackParticle*>&)>
    customNamedSeqGetter(const std::string&);
  }
}

#endif
