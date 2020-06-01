/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IGenParticleSelector.h
//   Header file for interface for GenParticleSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_IGENPARTICLESELECTOR_H
#define TRK_IGENPARTICLESELECTOR_H

#include <vector>
#include <string>
//#include "GeneratorObjects/McEventCollection.h"
#include "GaudiKernel/IAlgTool.h"

class McEventCollection;
#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
  static const InterfaceID IID_IGenParticleSelector("IGenParticleSelector",1,0);

/** @class IGenParticleSelector
    @brief provides the interface for tools to select generated particles
*/

class IGenParticleSelector : virtual public IAlgTool {
  public:
    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();
    
    /** explain */
    virtual std::vector<const HepMC::GenParticle *>*
      selectGenSignal (const McEventCollection*) const=0;
  };

inline const InterfaceID& Trk::IGenParticleSelector::interfaceID() {
    return IID_IGenParticleSelector;
}

} // end namespace
#endif
