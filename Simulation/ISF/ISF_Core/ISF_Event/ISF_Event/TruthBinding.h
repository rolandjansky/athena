/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthBinding.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_TRUTHBINDING_H
#define ISF_EVENT_TRUTHBINDING_H

// HepMC
#include "HepMC/GenParticle.h"

namespace ISF {

  /**
   @class TruthBinding 
   @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
   */

 class TruthBinding {
   public:
     /** constructor setting all truth particle pointers to the given particle */
     inline TruthBinding(HepMC::GenParticle* allTruthP);
     /** constructor setting all truth particle pointers individually */
     inline TruthBinding(HepMC::GenParticle* truthP, HepMC::GenParticle* primaryTruthP, HepMC::GenParticle* genZeroTruthP);

     /** copy constructor */
     inline TruthBinding(TruthBinding &rhs);

     /** destructor */
     inline ~TruthBinding();

      /** pointer to the particle in the simulation truth */
     inline HepMC::GenParticle* getTruthParticle() const;
     inline void                setTruthParticle(HepMC::GenParticle* p);

     /** pointer to the primary particle in the simulation truth */
     inline HepMC::GenParticle* getPrimaryTruthParticle() const;

     /** pointer to the simulation truth particle before any regeneration happened (eg. brem) */
     inline HepMC::GenParticle* getGenerationZeroTruthParticle() const;
     inline void                setGenerationZeroTruthParticle(HepMC::GenParticle* p);

   private:
     /** private constructor */
     inline TruthBinding();

     HepMC::GenParticle*   m_truthParticle;               //!< pointer to particle in MC truth
     HepMC::GenParticle*   m_primaryTruthParticle;        //!< pointer to corresponding primary (generator) particle
     HepMC::GenParticle*   m_generationZeroTruthParticle; //!< pointer to corresponding truth particle before any regenration
  };
  
} // end of namespace

/* implementation for inlined methods */
#include <ISF_Event/TruthBinding.icc>

#endif // ISF_EVENT_TRUTHBINDING_H
