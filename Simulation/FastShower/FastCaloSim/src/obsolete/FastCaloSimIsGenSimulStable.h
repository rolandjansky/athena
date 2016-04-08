/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *************************************************************************
// *                                                                       *
// *  class FastCaloSimIsGenSimulStable                                    *
// *                                                                       *
// * this predicate returns true if the particle is stable in a generated  * 
// * event or is a Geant decay product                                     * 
// * i.e. the particle belongs to the final state.                         *
// *                                                                       *
// *************************************************************************
#ifndef TRUTHHELPER_FASTCALOSIMISGENSIMULSTABLE_H
#define TRUTHHELPER_FASTCALOSIMISGENSIMULSTABLE_H

#ifndef TRUTHHELPER_GENIMCSELECTOR_H
#include "TruthHelper/GenIMCselector.h"
#endif

using namespace TruthHelper;

  class FastCaloSimIsGenSimulStable: public GenIMCselector {
  public:
    FastCaloSimIsGenSimulStable();
    GenIMCselector* create() const;
    virtual bool operator()( const HepMC::GenParticle* const p )const;
    virtual bool operator()( const HepMC::GenParticle& p ) const;
  private:
  };
#endif
