/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISCONVERSION_H
#define TRUTHHELPER_ISCONVERSION_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @deprecated Use the functions in TruthUtils instead
  class IsConversion : public GenIMCselector {
  public:

    IsConversion() {}

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr  p ) const;

  };


}

#endif
