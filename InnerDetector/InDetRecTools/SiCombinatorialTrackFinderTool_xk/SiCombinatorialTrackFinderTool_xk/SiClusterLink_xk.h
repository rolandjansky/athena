/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiClusterLink_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for link to SCT amd PIXEL clusters
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiClusterLink_xk_H
#define SiClusterLink_xk_H

#include "InDetPrepRawData/SiCluster.h"


namespace InDet{

  class SiClusterLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiClusterLink_xk();
      SiClusterLink_xk(const InDet::SiCluster*&,double&);
      SiClusterLink_xk(const SiClusterLink_xk&);
      ~SiClusterLink_xk();
      SiClusterLink_xk& operator  = (const SiClusterLink_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const InDet::SiCluster*  cluster() const {return m_cluster;}  
      const double&            xi2    () const {return m_xi2;}
      void Set(const InDet::SiCluster*&,double&);
      void Comparison(SiClusterLink_xk&);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////
      
      const InDet::SiCluster* m_cluster;
      double                  m_xi2    ;
           
      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiClusterLink_xk::SiClusterLink_xk()
    {
      m_cluster  = 0 ;
      m_xi2      = 0.;
    }

  inline SiClusterLink_xk::SiClusterLink_xk(const InDet::SiCluster*& c, double& xi2)
    {
      m_cluster  = c;
      m_xi2      = xi2;
    }

  inline SiClusterLink_xk::SiClusterLink_xk(const SiClusterLink_xk& L)
    {
      m_cluster     = L.m_cluster;
      m_xi2         = L.m_xi2;
    }
  
  inline SiClusterLink_xk& SiClusterLink_xk::operator = 
    (const SiClusterLink_xk& L) 
    {
      if(&L!=this) {
	m_cluster     = L.m_cluster;
	m_xi2         = L.m_xi2;
      }
      return(*this);
    }

  inline SiClusterLink_xk::~SiClusterLink_xk() {}

  inline void SiClusterLink_xk::Set(const InDet::SiCluster*& c, double& xi2)
    {
      m_cluster  = c;
      m_xi2      = xi2;
    }

  inline void SiClusterLink_xk::Comparison(SiClusterLink_xk& L )
    {
      if(L.m_xi2 < m_xi2) {
	double x = m_xi2;     
	m_xi2 = L.m_xi2; L.m_xi2 = x;

	const InDet::SiCluster* c = m_cluster; 
	m_cluster = L.m_cluster; L.m_cluster = c;
      }
    }
  

} // end of name space

#endif // SiClusterLink_xk


