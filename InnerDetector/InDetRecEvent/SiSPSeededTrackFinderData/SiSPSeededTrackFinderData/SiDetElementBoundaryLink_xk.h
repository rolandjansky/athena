/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementBoundaryLink_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for detector elements links
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementBoundaryLink_xk_H
#define SiDetElementBoundaryLink_xk_H

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkPatternParameters/PatternTrackParameters.h"

namespace InDet{


  class SiDetElementBoundaryLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiDetElementBoundaryLink_xk(const InDetDD::SiDetectorElement*&);
      SiDetElementBoundaryLink_xk(const SiDetElementBoundaryLink_xk&);
      ~SiDetElementBoundaryLink_xk();
      SiDetElementBoundaryLink_xk& operator  = (const SiDetElementBoundaryLink_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const InDetDD::SiDetectorElement* detElement() const {return m_detelement;}
      int intersect(const Trk::PatternTrackParameters&,double&) const; 

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      const InDetDD::SiDetectorElement*   m_detelement ;
      double                              m_bound[4][3]{};
      double                              m_dR         ;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiDetElementBoundaryLink_xk::SiDetElementBoundaryLink_xk(const SiDetElementBoundaryLink_xk& L)
    {
      m_detelement =  L.m_detelement;
      for(int i=0; i!=4; ++i) {
	for(int j=0; j!=3; ++j) {m_bound[i][j]=L.m_bound[i][j];}
      }
      m_dR = L.m_dR;
    }
  
  inline SiDetElementBoundaryLink_xk& SiDetElementBoundaryLink_xk::operator = 
    (const SiDetElementBoundaryLink_xk& L) 
    {
      if(&L!=this) {
	m_detelement =  L.m_detelement;
	for(int i=0; i!=4; ++i) {
	  for(int j=0; j!=3; ++j) {m_bound[i][j]=L.m_bound[i][j];}
	}
	m_dR = L.m_dR;
      }
      return(*this);
    }

  inline SiDetElementBoundaryLink_xk::~SiDetElementBoundaryLink_xk() {}

} // end of name space

#endif // SiDetElementBoundaryLink_xk
