/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef LARG4FASTSIMSVC_ILARG4FASTSIMSVC_H
#define LARG4FASTSIMSVC_ILARG4FASTSIMSVC_H

// Standard includes
#include "GaudiKernel/IInterface.h"
#include "G4ParticleDefinition.hh"

#include "AtlasHepMC/GenEvent_fwd.h"
// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ILArG4FastSimSvc("ILArG4FastSimSvc", 0 , 0);

class ILArG4FastSimSvc : virtual public IInterface {
  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ILArG4FastSimSvc; }

    // actual interface definiton

    virtual bool   flagToShowerLib( const G4ParticleDefinition& )     const = 0;	      //!< get switch for frozen showers
    virtual double maxEneToShowerLib( const G4ParticleDefinition& )   const = 0;	      //!< get lower energy limit for frozen showers
    virtual double minEneToShowerLib( const G4ParticleDefinition& )   const = 0;	      //!< get upper energy limit for frozen showers

    virtual bool   ContainLow()         const = 0;            //!< Apply check containment at low eta
    virtual bool   ContainHigh()        const = 0;            //!< Apply check containment at high eta
    virtual bool   ContainCrack()       const = 0;            //!< Apply check containment in crack region

    virtual bool                generateFSStartingPoints() const = 0;
    virtual bool                generateFSStartingPoint( const HepMC::GenEvent*) const = 0;
    virtual float               generateFSStartingPointsRatio() const = 0;

    virtual int DetectorTag() const = 0;
};

#endif // LARG4FASTSIMSVC_ILARG4FASTSIMSVC_H
