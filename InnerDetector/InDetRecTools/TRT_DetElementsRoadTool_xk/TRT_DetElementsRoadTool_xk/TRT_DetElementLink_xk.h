/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DetElementLink_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for detector elements links
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DetElementLink_xk_H
#define TRT_DetElementLink_xk_H

#include "InDetReadoutGeometry/TRT_BaseElement.h"

namespace InDet {


  class TRT_DetElementLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      TRT_DetElementLink_xk();
      TRT_DetElementLink_xk(const InDetDD::TRT_BaseElement*&,const double*);
      TRT_DetElementLink_xk(const TRT_DetElementLink_xk&);
      ~TRT_DetElementLink_xk();
      TRT_DetElementLink_xk& operator  = (const TRT_DetElementLink_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const InDetDD::TRT_BaseElement* detElement() const {return m_detelement ;}
      const float&  phi    ()                      const {return m_phi        ;}
      const float&  sin    ()                      const {return m_sin        ;}
      const float&  cos    ()                      const {return m_cos        ;}
      const float&  centerf()                      const {return m_centerf    ;}
      const float&  way    ()                      const {return m_way        ;}
      const bool  & used   ()                      const {return m_used       ;}
      void clearUsed();  
      void setUsed (float);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      const InDetDD::TRT_BaseElement*  m_detelement ;
      float                            m_phi        ;
      float                            m_sin        ;
      float                            m_cos        ;
      float                            m_centerf    ;
      float                            m_way        ;
      bool                             m_used       ;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_DetElementLink_xk::TRT_DetElementLink_xk()
    {
      m_detelement = 0    ;
      m_phi        = 0.   ;
      m_sin        = 0.   ;
      m_cos        = 0.   ;
      m_centerf    = 0.   ;
      m_way        = 0.   ;
      m_used       = false;
    }

  inline TRT_DetElementLink_xk::TRT_DetElementLink_xk(const TRT_DetElementLink_xk& L): m_detelement(L.m_detelement)
    {
      *this = L; 
    }
  
  inline TRT_DetElementLink_xk& TRT_DetElementLink_xk::operator = 
    (const TRT_DetElementLink_xk& L) 
    {
      if(&L!=this) {
	m_detelement =  L.m_detelement;
	m_phi        =  L.m_phi       ;
	m_sin        =  L.m_sin       ;
	m_cos        =  L.m_cos       ;
	m_centerf    =  L.m_centerf   ;
	m_way        =  L.m_way       ;
	m_used       =  L.m_used      ;
      }
      return(*this);
    }

  inline void InDet::TRT_DetElementLink_xk::clearUsed()
    {
      m_used = false;
    }

  inline void InDet::TRT_DetElementLink_xk::setUsed(float way)
    {
      m_used = true; m_way = way;
    }

  inline TRT_DetElementLink_xk::~TRT_DetElementLink_xk() {}

} // end of name space

#endif // TRT_DetElementLink_xk


