/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

namespace InDet {


  class TRT_DetElementLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:
       class Used_t {
       public:
          Used_t() : m_used(false) {}

          bool used() const { return m_used; }

          void setUsed() { m_used=true; }
       private:
          bool m_used;
       };

      TRT_DetElementLink_xk();
      TRT_DetElementLink_xk(const InDetDD::TRT_BaseElement*&,const double*);

      TRT_DetElementLink_xk(const TRT_DetElementLink_xk&) = default;
      TRT_DetElementLink_xk(TRT_DetElementLink_xk &&)     = default;

      TRT_DetElementLink_xk& operator  = (const TRT_DetElementLink_xk&) = default;
      TRT_DetElementLink_xk& operator  = (TRT_DetElementLink_xk&&)      = default;

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const InDetDD::TRT_BaseElement* detElement() const {return m_detelement ;}
      const float&  phi    ()                      const {return m_phi        ;}
      const float&  sin    ()                      const {return m_sin        ;}
      const float&  cos    ()                      const {return m_cos        ;}
      const float&  centerf()                      const {return m_centerf    ;}

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      const InDetDD::TRT_BaseElement*  m_detelement ;
      float                            m_phi        ;
      float                            m_sin        ;
      float                            m_cos        ;
      float                            m_centerf    ;

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
    }

   inline InDet::TRT_DetElementLink_xk::TRT_DetElementLink_xk
   (const InDetDD::TRT_BaseElement*& el,const double* P)
   {
      m_detelement = el                           ; // det elements
      m_phi        = float(P[ 2])                 ; // azimuthal angle
      m_sin        = float(P[ 5])                 ; // sin(phi)
      m_cos        = float(P[ 6])                 ; // cos(phi)
      m_centerf    = float(P[18])                 ; //
   }


} // end of name space

#endif // TRT_DetElementLink_xk


