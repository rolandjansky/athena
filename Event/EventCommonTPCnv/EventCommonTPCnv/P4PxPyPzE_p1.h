///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4PxPyPzE_p1.h 
// Header file for class P4PxPyPzE_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4PXPYPZE_P1_H 
#define EVENTCOMMONTPCNV_P4PXPYPZE_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class P4PxPyPzECnv_p1;

class P4PxPyPzE_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor: with 4 floats
   */
  P4PxPyPzE_p1( const float px = 0., const float py  = 0.,
		const float pz = 0., const float ene = 0. );

  ///////////////////////////////////////////////////////////////////
  // Data: (don't bother encapsulate
  ///////////////////////////////////////////////////////////////////
  
  float m_px;
  float m_py;
  float m_pz;
  float m_ene;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline P4PxPyPzE_p1::P4PxPyPzE_p1( const float px, const float py,
				   const float pz, const float ene ) :
  m_px  ( px  ),
  m_py  ( py  ),
  m_pz  ( pz  ),
  m_ene ( ene )
{}

#endif //> EVENTCOMMONTPCNV_P4PXPYPZE_P1_H
