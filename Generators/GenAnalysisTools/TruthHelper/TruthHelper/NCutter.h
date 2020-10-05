/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPMCHELPER_NCUTTER_H
#define HEPMCHELPER_NCUTTER_H

#include "TruthHelper/GenIMCselector.h"
#include "TruthHelper/GenAll.h"
#include <vector>

namespace TruthHelper {


  /// @brief Allows the chaining of a number of GenParticle selectors
  /// @deprecated Use the functions in TruthUtils instead
  class NCutter : public GenIMCselector {
  public:

    NCutter() {
      m_selectors.push_back( new GenAll() );
    }

    NCutter(const std::vector<GenIMCselector*> selectors) {
      std::vector<GenIMCselector*>::const_iterator i = selectors.begin();
      for (; i != selectors.end(); ++i) {
        m_selectors.push_back((*i)->create());
      }
    }

    NCutter(const GenIMCselector* selector) {
      m_selectors.push_back(selector->create() );
    }

    NCutter( const NCutter& src) {
      std::vector<GenIMCselector*>::const_iterator i = src.m_selectors.begin();
      for (; i != (src.m_selectors).end(); ++i) {
        m_selectors.push_back((*i)->create());
      }
    }

    NCutter& operator=(const NCutter& rhs) ;

    ~NCutter() {
      std::vector<GenIMCselector*>::iterator i = m_selectors.begin();
      for (; i != m_selectors.end(); ++i) delete(*i);
    }

    GenIMCselector* create() const;

    bool operator() (HepMC::ConstGenParticlePtr ) const;


  private:

    std::vector<GenIMCselector*> m_selectors;

  };


}


#endif
