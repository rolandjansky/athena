///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4EEtaPhiM_p1.h 
// Header file for class P4EEtaPhiM_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4EETAPHIM_P1_H 
#define EVENTCOMMONTPCNV_P4EETAPHIM_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class P4EEtaPhiMCnv_p1;

class P4EEtaPhiM_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 floats
   */
  P4EEtaPhiM_p1( const float ene = 0., const float eta  = 0.,
		 const float phi = 0., const float mass = 0. );

  ///////////////////////////////////////////////////////////////////
  // Data: (don't bother encapsulate
  ///////////////////////////////////////////////////////////////////
  
  float m_e;
  float m_eta;
  float m_phi;
  float m_m;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline P4EEtaPhiM_p1::P4EEtaPhiM_p1( const float ene, const float eta,
				     const float phi, const float mass ) :
  m_e  ( ene  ),
  m_eta( eta  ),
  m_phi( phi  ),
  m_m  ( mass )
{}

#endif //> EVENTCOMMONTPCNV_P4EETAPHIM_P1_H
