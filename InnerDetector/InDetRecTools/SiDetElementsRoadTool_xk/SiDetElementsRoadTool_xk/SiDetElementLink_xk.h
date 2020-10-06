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
#include <utility>

namespace InDet{


  class SiDetElementLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:

       class UsedFlag {
       public:
          UsedFlag() : m_used(false) {}

          bool used() const { return m_used; }

          void setUsed() { m_used=true; }
       private:
          bool m_used;
       };

       class ElementWay {
       const InDet::SiDetElementLink_xk* m_link;
       float m_way;
       public:
          ElementWay(const InDet::SiDetElementLink_xk*link, float way)
             : m_link(link), m_way(way) {}

          const InDet::SiDetElementLink_xk* link() const { return m_link; }
          float way()                              const { return m_way; }
       };

      SiDetElementLink_xk();
      SiDetElementLink_xk(const InDetDD::SiDetectorElement*,const double*);
      SiDetElementLink_xk(const SiDetElementLink_xk&) = default;
      ~SiDetElementLink_xk() = default;
      SiDetElementLink_xk& operator  = (const SiDetElementLink_xk&) = default;

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      void set (const double*);
      const InDetDD::SiDetectorElement* detElement() const {return m_detelement;}
      float phi () const {return m_phi;}
      void intersect(const float*,const float*,float*) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      const InDetDD::SiDetectorElement*   m_detelement ;
      float                               m_phi        ;
      float                               m_geo     [6];
      float                               m_center  [2];
      float                               m_bound[4][3];

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
    }


  inline InDet::SiDetElementLink_xk::SiDetElementLink_xk
    (const InDetDD::SiDetectorElement* el,const double* P)
    {
      m_detelement = el; set(P);
    } 


} // end of name space

#endif // SiDetElementLink_xk


