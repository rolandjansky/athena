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

       class ElementWay : public std::pair<const InDet::SiDetElementLink_xk*, std::pair<float,float> > {
       public:
          ElementWay(const InDet::SiDetElementLink_xk*link, float way, float distance)
             : std::pair<const InDet::SiDetElementLink_xk*, std::pair<float,float> >(link,std::make_pair(way,distance)) {}

          const InDet::SiDetElementLink_xk* link() const { return this->first; }
          float way()                              const { return this->second.first; }
          float distance()                              const { return this->second.second; }
       };

      SiDetElementLink_xk();
      SiDetElementLink_xk(const InDetDD::SiDetectorElement*,const double*);
      SiDetElementLink_xk(const SiDetElementLink_xk&);
      ~SiDetElementLink_xk();
      SiDetElementLink_xk& operator  = (const SiDetElementLink_xk&);

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
	for(int i=0; i!=6; ++i)  m_geo   [i] = L.m_geo   [i];
	for(int i=0; i!=2; ++i)  m_center[i] = L.m_center[i];
	for(int i=0; i!=4; ++i) {
	  for(int j=0; j!=3; ++j) {m_bound[i][j]=L.m_bound[i][j];}
	}
      }
      return(*this);
    }
 

  inline InDet::SiDetElementLink_xk::SiDetElementLink_xk
    (const InDetDD::SiDetectorElement* el,const double* P)
    {
      m_detelement = el; set(P);
    } 

  inline SiDetElementLink_xk::~SiDetElementLink_xk() {}

} // end of name space

#endif // SiDetElementLink_xk


