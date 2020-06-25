/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISSTATUS_H
#define TRUTHHELPER_ISSTATUS_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @brief Returns true if the particle has a user-set status code
  /// @deprecated Use the functions in TruthUtils instead
  class IsStatus : public GenIMCselector {
  public:

    IsStatus(int stat)
      : m_stat(stat)
    { }

    IsStatus(const IsStatus& rhs)
      : GenIMCselector(), m_stat(rhs.m_stat)
    { }

    IsStatus& operator=(const IsStatus& rhs);

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr  p ) const;


  private:

    int m_stat;

  };

}


#endif
