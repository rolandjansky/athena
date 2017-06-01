/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IInputConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IINPUTCONVERTER_H
#define ISF_INTERFACES_IINPUTCONVERTER_H 1

// Gaudi
#include "GaudiKernel/IInterface.h"

// StoreGate
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// Simulation includes
#include "ISF_Event/ISFParticleContainer.h"

/** Declaration of the interface ID ( interface id, major version, minor version) */
static const InterfaceID IID_IInputConverter("IInputConverter", 1, 0);

// forward declarations
class McEventCollection;
class G4Event;
namespace HepMC {
  class GenEvent;
}

namespace ISF {

  class ISFParticle;

  /**
     @class IInputConverter

     Interface to Athena service that converts an input McEventCollection
     into a container of ISFParticles.

     @author Elmar.Ritsch -at- cern.ch
  */

  class IInputConverter : virtual public IInterface {
  public:

    /** Virtual destructor */
    virtual ~IInputConverter(){}

    /** Gaudi InterfaceID */
    static const InterfaceID& interfaceID() { return IID_IInputConverter; }

    /** Convert selected particles from the given McEventCollection into ISFParticles
        and push them into the given ISFParticleContainer */
    virtual StatusCode convert(const McEventCollection& inputGenEvents,
                               ISFParticleContainer& simParticles,
                               bool isPileup) const = 0;

    /** Convert selected particles from the given McEventCollection into G4PrimaryParticles
        and push them into the given G4Event */
    virtual StatusCode convertHepMCToG4Event(McEventCollection& inputGenEvents,
                                             G4Event*& outputG4Event,
                                             bool isPileup) const = 0;

    /** Converts vector of ISF::ISFParticles to G4Event */
    virtual G4Event* ISF_to_G4Event(const std::vector<const ISF::ISFParticle*>& isp, HepMC::GenEvent *genEvent) const = 0;

  };

} // end of namespace

#endif // ISF_INTERFACES_IINPUTCONVERTER_H
