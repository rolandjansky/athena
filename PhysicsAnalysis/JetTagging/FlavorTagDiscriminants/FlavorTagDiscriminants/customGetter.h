/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

  /// Factory function to produce TrackParticle -> vector<double> functions
  ///
  /// DL2 configures the its inputs when the algorithm is initalized,
  /// meaning that the list of track and jet properties that are used
  /// as inputs won't be known at compile time. Instead we build an
  /// array of "getter" functions, each of which returns one input for
  /// the tagger. The function here returns those getter functions.
  ///
  /// Many of the getter functions are trivial: they will, for example,
  /// read one double of auxdata off of the BTagging object. The
  /// sequence input getters tend to be more complicated. Since we'd
  /// like to avoid reimplementing the logic in these functions in
  /// multiple places, they are exposed here.
  ///
  /// This function will return a getter based on a string key. See the
  /// implementation for the definitions.
  ///
  /// NOTE: This function is for experts only, don't expect support.
  ///
  std::function<std::vector<double>(
    const xAOD::Jet&,
    const std::vector<const xAOD::TrackParticle*>&)>
  customSequenceGetter(const std::string& name);

  // internal functions
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
