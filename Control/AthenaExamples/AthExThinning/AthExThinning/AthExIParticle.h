///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExIParticle.h 
// Header file for class AthExIParticle
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXIPARTICLE_H 
#define ATHEXTHINNING_ATHEXIPARTICLE_H 

// STL includes

// Gaudi includes

// Forward declaration

class AthExIParticle
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~AthExIParticle();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  ///@{ AthExIParticle interface
  virtual double px() const = 0;
  virtual double py() const = 0;
  virtual double pz() const = 0;
  virtual double e () const = 0;
  ///@} 

}; 

#endif //> ATHEXTHINNING_ATHEXIPARTICLE_H
