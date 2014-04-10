///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4IPtCotThPhiM_p1.h 
// Header file for class P4IPtCotThPhiM_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4IPTCOTTHPHIM_P1_H 
#define EVENTCOMMONTPCNV_P4IPTCOTTHPHIM_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class P4IPtCotThPhiMCnv_p1;

class P4IPtCotThPhiM_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 floats
   */
  P4IPtCotThPhiM_p1( const float iPt = 0., const float cotTh = 0.,
		     const float phi = 0., const float mass  = 0. );

  ///////////////////////////////////////////////////////////////////
  // Data: (don't bother encapsulate
  ///////////////////////////////////////////////////////////////////
  
  float m_iPt;
  float m_cotTh;
  float m_phi;
  float m_mass;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline P4IPtCotThPhiM_p1::P4IPtCotThPhiM_p1( const float iPt, 
					     const float cotTh,
					     const float phi, 
					     const float mass ) :
  m_iPt   ( iPt   ),
  m_cotTh ( cotTh ),
  m_phi   ( phi   ),
  m_mass  ( mass  )
{}

#endif //> EVENTCOMMONTPCNV_P4IPTCOTTHPHIM_P1_H
