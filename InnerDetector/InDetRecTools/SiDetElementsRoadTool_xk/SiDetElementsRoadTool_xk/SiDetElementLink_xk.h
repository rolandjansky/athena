/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementLink_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for detector elements links
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementLink_xk_H
#define SiDetElementLink_xk_H

#include "InDetReadoutGeometry/SiDetectorElement.h"

namespace InDet{


  class SiDetElementLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiDetElementLink_xk();
      SiDetElementLink_xk(InDetDD::SiDetectorElement*&,const double*,bool isITk = false);
      SiDetElementLink_xk(const SiDetElementLink_xk&);
      ~SiDetElementLink_xk();
      SiDetElementLink_xk& operator  = (const SiDetElementLink_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      void set (const double*, bool isITk = false);
      const InDetDD::SiDetectorElement* detElement() const {return m_detelement;}
      float phi () const {return m_phi;}
      float z   () const {return m_z  ;}
      float dz  () const {return m_dz ;}
      void intersect(const float*,const float*,float*) const;
      bool intersectITk(const float*,const float*,float&) const;
      const float&  way () const {return m_way;}
      const bool&   used() const {return m_used;}
      void clearUsed();  
      void setUsed (float);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      const InDetDD::SiDetectorElement*   m_detelement ;
      float                               m_phi        ;
      float                               m_z          ;
      float                               m_dz         ;
      float                               m_geo    [10];
      float                               m_bound[4][3];
      float                               m_way        ;
      bool                                m_used       ;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiDetElementLink_xk::SiDetElementLink_xk()
    {
      m_detelement = 0    ;
      m_phi        = 0.   ;
      m_way        = 0.   ;
      m_used       = false;
    }

  inline SiDetElementLink_xk::SiDetElementLink_xk(const SiDetElementLink_xk& L): m_detelement(L.m_detelement)
    {
      *this = L;
    }
  
  inline SiDetElementLink_xk& SiDetElementLink_xk::operator = 
    (const SiDetElementLink_xk& L) 
    {
      if(&L!=this) {

	m_detelement =  L.m_detelement;
	m_phi        =  L.m_phi       ;
	m_z          =  L.m_z         ;
	m_dz         =  L.m_dz        ;
	for(int i=0; i!=10; ++i)  m_geo   [i] = L.m_geo   [i];
	for(int i=0; i!= 4; ++i) {
	  for(int j=0; j!=3; ++j) {m_bound[i][j]=L.m_bound[i][j];}
	}
	m_way        = L.m_way ;
	m_used       = L.m_used;
      }
      return(*this);
    }
 
  inline void InDet::SiDetElementLink_xk::clearUsed()
    {
      m_used = false;
    }

  inline void InDet::SiDetElementLink_xk::setUsed(float way)
    {
      m_used = true; m_way = way;
    }

  inline InDet::SiDetElementLink_xk::SiDetElementLink_xk
    (InDetDD::SiDetectorElement*& el,const double* P, bool isITk)
    {
      m_detelement = el; set(P, isITk); m_used = false; m_way = 0.;
    } 

  inline SiDetElementLink_xk::~SiDetElementLink_xk() {}

} // end of name space

#endif // SiDetElementLink_xk


