///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4PtEtaPhiMFloat_p2.h 
// Header file for class P4PtEtaPhiMFloat_p2
// Author: I. Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4PTETAPHIMFLOAT_P2_H 
#define EVENTCOMMONTPCNV_P4PTETAPHIMFLOAT_P2_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class P4PtEtaPhiMCnv_p2;

class P4PtEtaPhiMFloat_p2
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 floats
   */
  P4PtEtaPhiMFloat_p2( const float pt  = 0., const float eta  = 0.,
		  const float phi = 0., const float mass = 0. );

  ///////////////////////////////////////////////////////////////////
  // Data: (don't bother encapsulate
  ///////////////////////////////////////////////////////////////////
  
  float m_pt;
  float m_eta;
  float m_phi;
  float m_mass;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline P4PtEtaPhiMFloat_p2::P4PtEtaPhiMFloat_p2( const float pt,  const float eta,
				       const float phi, const float mass ) :
  m_pt  ( pt   ),
  m_eta ( eta  ),
  m_phi ( phi  ),
  m_mass( mass )
{}

#endif //> EVENTCOMMONTPCNV_P4PTETAPHIMFLOAT_P2_H
