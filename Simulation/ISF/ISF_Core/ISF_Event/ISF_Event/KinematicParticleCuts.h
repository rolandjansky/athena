/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinematicParticleCuts.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_KINEMATICPARTICLECUTS_H
#define ISF_EVENT_KINEMATICPARTICLECUTS_H 1

// ISF includes
#include "ISF_Event/ISFParticle.h"

namespace ISF {

  /** @class KinematicParticleCuts
  
      Simplistic Particle cut filter
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class KinematicParticleCuts {

    public: 
      /** Constructor */
      inline KinematicParticleCuts();

      /** virtual Destructor */
      inline virtual ~KinematicParticleCuts();

      /** check whether the given particle passes all cuts */
      inline bool pass(const ISFParticle& particle) const;

    protected:
      float    m_cut_minPosEta; //!< minimum pseudorapidity of particle position
      float    m_cut_maxPosEta; //!< maximum pseudorapidity of particle position
      float    m_cut_minMomEta; //!< minimum pseudorapidity of particle momentum
      float    m_cut_maxMomEta; //!< maximum pseudorapidity of particle momentum
      double   m_cut_minMom2;   //!< minimum squarde particle momentum magnitude
      double   m_cut_maxMom2;   //!< maximum squared particle momentum magnitude 
      double   m_cut_maxEkin;   //!< maximum particle kinetic energy 
      float    m_cut_charge;    //!< particle charge
      int      m_cut_pdg;       //!< particle PDG code
  };

}

// include inline functions file
#include "ISF_Event/KinematicParticleCuts.icc"

#endif //> !ISF_EVENT_KINEMATICPARTICLECUTS_H
