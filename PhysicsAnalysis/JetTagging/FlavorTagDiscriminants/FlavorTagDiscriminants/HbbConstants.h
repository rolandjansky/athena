/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Note: please don't include any ROOT in here (i.e. xAOD::Jet or
// anything from the EDM), it's not a stable dependency (in particular
// the TString overloads change between releases).

#ifndef HBB_CONSTANTS_H
#define HBB_CONSTANTS_H

namespace FlavorTagDiscriminants {

  namespace hbb_key {
    // these must be specified correctly in the neural network inputs
    const std::string pt = "pt"; // should be in MeV
    const std::string eta = "eta";
    const std::string subjet = "subjet";
    const std::string fatjet = "fatjet";
  }
}

#endif
