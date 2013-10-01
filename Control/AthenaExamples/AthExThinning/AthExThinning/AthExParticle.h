///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticle.h 
// Header file for class AthExParticle
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXPARTICLE_H 
#define ATHEXTHINNING_ATHEXPARTICLE_H 

// STL includes

// Gaudi includes

// AthExThinning includes
#include "AthExThinning/AthExIParticle.h"

// Forward declaration

class AthExParticle : public AthExIParticle
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  AthExParticle( double px = 0., double py  = 0., 
		 double pz = 0., double ene = 0.);

  /** Copy constructor: 
   */
  AthExParticle( const AthExParticle& rhs );

  /** Assignment operator: 
   */
  AthExParticle& operator=( const AthExParticle& rhs ); 

  /** Constructor with parameters: 
   */

  /** Destructor: 
   */
  virtual ~AthExParticle();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  ///@{ Implementation of the AthExIParticle interface
  virtual double px() const;
  virtual double py() const;
  virtual double pz() const;
  virtual double e () const;
  ///@} 

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  double m_px;
  double m_py;
  double m_pz;
  double m_ene;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline double AthExParticle::px() const { return m_px;  }
inline double AthExParticle::py() const { return m_py;  }
inline double AthExParticle::pz() const { return m_pz;  }
inline double AthExParticle::e () const { return m_ene; }


#endif //> ATHEXTHINNING_ATHEXPARTICLE_H
