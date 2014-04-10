///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4PtEtaPhiM_p1.h 
// Header file for class P4PtEtaPhiM_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4PTETAPHIM_P1_H 
#define EVENTCOMMONTPCNV_P4PTETAPHIM_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class P4PtEtaPhiMCnv_p1;

class P4PtEtaPhiM_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 floats
   */
  P4PtEtaPhiM_p1( const float pt  = 0., const float eta  = 0.,
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
inline P4PtEtaPhiM_p1::P4PtEtaPhiM_p1( const float pt,  const float eta,
				       const float phi, const float mass ) :
  m_pt  ( pt   ),
  m_eta ( eta  ),
  m_phi ( phi  ),
  m_mass( mass )
{}

#endif //> EVENTCOMMONTPCNV_P4PTETAPHIM_P1_H
