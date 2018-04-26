/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DriftCircleLink_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for link to SCT amd PIXEL clusters
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DriftCircleLinkN_xk_H
#define TRT_DriftCircleLinkN_xk_H

#include "InDetPrepRawData/TRT_DriftCircle.h"

namespace InDet{

  class TRT_DriftCircleLinkN_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      TRT_DriftCircleLinkN_xk();
      TRT_DriftCircleLinkN_xk(const TRT_DriftCircleLinkN_xk&);
      ~TRT_DriftCircleLinkN_xk();
      TRT_DriftCircleLinkN_xk& operator  = (const TRT_DriftCircleLinkN_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const TRT_DriftCircle*   circle () const {return m_circle ;}
      const float&             phi    () const {return m_phi    ;}
      const unsigned int&      address() const {return m_address;}
      unsigned int             buffer () const {return m_address/1000;}
      unsigned int             layer  () const {return m_address%1000;}
      void set(const TRT_DriftCircle*const&, const float&, const unsigned int&); 

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////
      
      const TRT_DriftCircle*  m_circle ;
      float                   m_phi    ;
      unsigned int            m_address;
           
      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_DriftCircleLinkN_xk::TRT_DriftCircleLinkN_xk()
    {
      m_circle =  0 ;
      m_phi     = 0.;
      m_address = 0 ;
    }

  inline TRT_DriftCircleLinkN_xk::TRT_DriftCircleLinkN_xk(const TRT_DriftCircleLinkN_xk& L)
    {
      *this = L;
    }
  
  inline TRT_DriftCircleLinkN_xk& TRT_DriftCircleLinkN_xk::operator = 
    (const TRT_DriftCircleLinkN_xk& L) 
    {
      if(&L!=this) {
	m_circle  = L.m_circle ;
	m_phi     = L.m_phi    ;
	m_address = L.m_address;
      } 
      return(*this);
    }

  inline TRT_DriftCircleLinkN_xk::~TRT_DriftCircleLinkN_xk() {}

  inline void TRT_DriftCircleLinkN_xk::set
    (const TRT_DriftCircle* const& d, const float& p, const unsigned int& a) 
    {
      m_circle  = d;
      m_phi     = p;
      m_address = a;
    }

} // end of name space

#endif // TRT_DriftCircleLinkN_xk


