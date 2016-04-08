/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RawEvent/LUCID_Digit.h"
#include <iostream>

LUCID_Digit::LUCID_Digit() {
  
  m_tubeID = 0;
  m_npe    = 0;
  m_npeGas = 0; 
  m_npePmt = 0;
  m_qdc    = 0; 
  m_tdc    = 0; 
  m_isHit  = 0; 
}

LUCID_Digit::LUCID_Digit(const unsigned short tubeID, 
  			 const double         npe, 
 			 const unsigned short npeGas, 
 			 const unsigned short npePmt, 
			 const unsigned short qdc, 
			 const unsigned short tdc,
			 const bool           isHit): 
  m_tubeID(tubeID), 
  m_npe   (npe), 
  m_npeGas(npeGas), 
  m_npePmt(npePmt), 
  m_qdc   (qdc), 
  m_tdc   (tdc), 
  m_isHit (isHit) 
{}
