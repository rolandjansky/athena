/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeParticleCuts.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_CONEPARTICLECUTS_H
#define ISF_EVENT_CONEPARTICLECUTS_H 1

// ISF includes
#include "ISF_Event/ISFParticle.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

// STL includes
#include <set>

namespace ISF {

  // typedefs:
  typedef std::vector< Amg::Vector3D >  ConeDirections;
  typedef std::set< int >               PDGCodesSet;

  /** @class ConeParticleCuts
  
      Simplistic Particle cut filter
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class ConeParticleCuts {

    public: 
      /** Constructor */
      inline ConeParticleCuts();

      /** virtual Destructor */
      inline virtual ~ConeParticleCuts();

      /** check whether particle passes cuts to create a new cone
           -> if so register new cone and return true
           -> if no cone is created: return false */
      inline bool checkAndRegisterCone(const ISFParticle& particle);

      /** check whether particle passes cuts to create a new cone with given direction
           -> if so register new cone and return true
           -> if no cone is created: return false */
      inline bool checkAndRegisterCone(const ISFParticle&   particle,
                                       const Amg::Vector3D& momentum);

      /** check whether particle passes cuts */
      inline bool checkParticle(const ISFParticle& particle);

      /** check whether the given particle passes all cuts (ie is within a cone) */
      inline bool insideCone(const ISFParticle& particle) const;

      /** reset internal cone representation */
      inline void resetCones(void);

    protected:
      bool                                      m_cut_checkPosition; //!< check if particle position is inside cone
      double                                    m_cut_minPt;         //!< min pT for particles to create cones around
      PDGCodesSet                               m_cut_absPdgs;       //!< abs(PDG) for particles to create cones around
      double                                    m_cut_coneSize2;     //!< cone size (squared for faster comparison)

    private:
      ConeDirections                            m_coneDirections;    //!< stl collection containing all current cones
  };

}

// include inline functions file
#include "ISF_Event/ConeParticleCuts.icc"

#endif //> !ISF_EVENT_CONEPARTICLECUTS_H
