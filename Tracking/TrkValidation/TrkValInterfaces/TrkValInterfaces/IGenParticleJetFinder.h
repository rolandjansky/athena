/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IGenParticleJetFinder.h
//   Header file for interface for GenParticleJetFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_IGENPARTICLEJETFINDER_H
#define TRK_IGENPARTICLEJETFINDER_H

#include <vector>
#include <string>
//#include "TrkValEvent/GenParticleJet.h"
#include "GaudiKernel/IAlgTool.h"

#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
  class GenParticleJet;

  static const InterfaceID IID_IGenParticleJetFinder("IGenParticleJetFinder",1,0);

/** @class IGenParticleJetFinder

    @brief provides the interface for determining truth jets (inside Tracking
           validation framework)

    @author Wolfgang.Liebig -at- cern.ch
*/

class IGenParticleJetFinder : virtual public IAlgTool {
  public:
    //! Interface ID, declared here, and defined below
    static const InterfaceID& interfaceID();
    
    /* original method from Loek: not a factory. 
    virtual StatusCode jetMCFinder(std::vector <HepMC::GenParticle *>, 
    std::vector< Trk::GenParticleJet >&, double); */

    /** @brief central method to supply the jets including a
        GenParticle-to-jet association

        This method is a factory, i.e. vector ownership is given back and
        on failure condition returns NULL. */
    virtual std::vector< Trk::GenParticleJet >*  jetMCFinder
      (std::vector <const HepMC::GenParticle *>& ) const=0;

  };

inline const InterfaceID& Trk::IGenParticleJetFinder::interfaceID() {
    return IID_IGenParticleJetFinder;
}

} // end namespace
#endif
