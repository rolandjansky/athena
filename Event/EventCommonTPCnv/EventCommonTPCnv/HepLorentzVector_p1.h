///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepLorentzVector_p1.h 
// Header file for class HepLorentzVector_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_HEPLORENTZVECTOR_P1_H 
#define EVENTCOMMONTPCNV_HEPLORENTZVECTOR_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class HepLorentzVectorCnv_p1;

class HepLorentzVector_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 doubles
   */
  HepLorentzVector_p1( const double px = 0., const double py  = 0.,
		       const double pz = 0., const double ene = 0. );

  ///////////////////////////////////////////////////////////////////
  // Data: (don't bother encapsulate
  ///////////////////////////////////////////////////////////////////
  
  double m_px;
  double m_py;
  double m_pz;
  double m_ene;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline HepLorentzVector_p1::HepLorentzVector_p1( const double px, 
						 const double py,
						 const double pz, 
						 const double ene ) :
  m_px  ( px  ),
  m_py  ( py  ),
  m_pz  ( pz  ),
  m_ene ( ene )
{}

#endif //> EVENTCOMMONTPCNV_HEPLORENTZVECTOR_P1_H
