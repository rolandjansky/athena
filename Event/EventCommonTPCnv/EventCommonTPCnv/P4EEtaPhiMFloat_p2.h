///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4EEtaPhiMFloat_p2.h 
// Header file for class P4EEtaPhiMFloat_p2
// Author:  I. Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4EETAPHIMFloat_P2_H 
#define EVENTCOMMONTPCNV_P4EETAPHIMFloat_P2_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class P4EEtaPhiMCnv_p2;

class P4EEtaPhiMFloat_p2
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 doubles
   */
  P4EEtaPhiMFloat_p2( const float ene = 0., const float eta  = 0.,
		 const float phi = 0., const float mass = 0. );

  float m_e;
  float m_eta;
  float m_phi;
  float m_m;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline P4EEtaPhiMFloat_p2::P4EEtaPhiMFloat_p2( const float ene, const float eta,
				     const float phi, const float mass ) :
  m_e  ( ene  ),
  m_eta( eta  ),
  m_phi( phi  ),
  m_m  ( mass )
{}

#endif //> EVENTCOMMONTPCNV_P4EETAPHIMFloat_P2_H
