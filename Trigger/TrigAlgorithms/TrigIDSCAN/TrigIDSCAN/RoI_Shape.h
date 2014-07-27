/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: RoI_Shape.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// Description: Holds RoI shape numbers
// 
// date: 24/07/2002
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_ROISHAPE_H )
#define IDSCAN_ROISHAPE_H

namespace TrigIdScan {

////////////////////////////////////////////////////////////////////////////////
////    RoI_Shape
////////////////////////////////////////////////////////////////////////////////


class RoI_Shape
{
 public:
  
  RoI_Shape( double ehw, double phw ) : 
    m_etaHalfWidth(ehw),
    m_phiHalfWidth(phw) {};
  
  double etaHalfWidth() const { return m_etaHalfWidth; };
  double phiHalfWidth() const { return m_phiHalfWidth; };
  
 private:

  double m_etaHalfWidth;
  double m_phiHalfWidth;

};

} // namespace

#endif
