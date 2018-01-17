// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_IPARTICLE_H
#define EVENTKERNEL_IPARTICLE_H 1
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//             Interface for Particle-like objects                           //
//                                                                           //
//             Created by: Kyle Cranmer <kyle.cranmer@cern.ch>               //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"
#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/PdtPdg.h"
#include "AthLinks/ElementLink.h"

#ifndef SIMULATIONBASE
#ifndef XAOD_ANALYSIS
// VxVertex includes
#include "VxVertex/VxContainer.h"
#endif
#endif

// Forward declarations
namespace Trk { class RecVertex; }


/** Information about type of data used to fill particle.
 NOTE! This must be consistant with ../python/ParticleDataType.py */
namespace ParticleDataType {
  enum DataType {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4} ;
}

/** typedef ChargeType used to anticipate changes here*/
typedef double ChargeType ;

class IParticle : public virtual INavigable4Momentum
{
 public:

  virtual ~IParticle();

  /** Return enum indicating real data, fast, or full simulation
      Return Type has a DataType enum with the following values:
     {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4}  */
  virtual ParticleDataType::DataType dataType() const = 0;

  /** Return a RecVertex corresponding to particle Origin  */
  virtual const Trk::RecVertex* origin() const = 0;

#ifndef SIMULATIONBASE
#ifndef XAOD_ANALYSIS
  /** Return an ElementLink corresponding to particle's Origin */
  virtual const ElementLink<VxContainer>& originLink() const = 0;
#endif
#endif

  /** method to check if charge information is available*/
  virtual bool hasCharge()         const  = 0;

  /** returns charge as a typedef ChargeType
      currently Charge Type is a double for jets
      this may be changed to an int and ask jets to extend this interface  */
  virtual ChargeType charge() const = 0;

  /** method to check if particle id information is available*/
  virtual bool hasPdgId()          const = 0;

  /** Return enum indicating particle id
      the enum file is available in Event/EventKernel/PdtPdg.h  */
  virtual PDG::pidType pdgId()     const = 0;
  
};

CLASS_DEF(IParticle, 154185086, 1)
#endif
