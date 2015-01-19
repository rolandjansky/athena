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

#ifndef TRT_DriftCircleLink_xk_H
#define TRT_DriftCircleLink_xk_H

#include "InDetPrepRawData/TRT_DriftCircle.h"

namespace InDet{

  class TRT_DriftCircleLink_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      TRT_DriftCircleLink_xk();
      TRT_DriftCircleLink_xk(const TRT_DriftCircleLink_xk&);
      ~TRT_DriftCircleLink_xk();
      TRT_DriftCircleLink_xk& operator  = (const TRT_DriftCircleLink_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const TRT_DriftCircle* cluster () const {return m_cluster  ;}  
      const double&          distance() const {return m_distance ;}
      const double&         sdistance() const {return m_sdistance;}
      const double&          impact  () const {return m_impact   ;}
      const double&          zlocal  () const {return m_zlocal   ;}
      const double&         szlocal  () const {return m_szlocal  ;}
      const double&          way     () const {return m_way      ;}
      const int&             number  () const {return m_number   ;}
      
      const Trk::Surface& surface() const;

      void set(const int&,double&,double&,double&,double&);
      void set(const int&,double&,double&,double&,double&,double&,double&);
      void set(const TRT_DriftCircle* const&);
      void newImpactParameter(const double&);
      void radiusCorrection(const double&);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////
      
      const TRT_DriftCircle*  m_cluster     ;
      int                     m_number      ;
      double                  m_distance    ;
      double                  m_impact      ;
      double                  m_zlocal      ;
      double                  m_way         ;
      double                  m_sdistance   ;
      double                  m_szlocal     ;
          
      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_DriftCircleLink_xk::TRT_DriftCircleLink_xk()
    {
      m_cluster   = 0 ;
      m_number    = 0 ;
      m_distance  = 0.;
      m_impact    = 0.;
      m_zlocal    = 0.;
      m_way       = 0.;
      m_sdistance = 0.;
      m_szlocal   = 0.;
    }

  inline TRT_DriftCircleLink_xk::TRT_DriftCircleLink_xk(const TRT_DriftCircleLink_xk& L)
    {
      *this = L;
    }
  
  inline TRT_DriftCircleLink_xk& TRT_DriftCircleLink_xk::operator = 
    (const TRT_DriftCircleLink_xk& L) 
    {
      m_cluster     = L.m_cluster  ;
      m_number      = L.m_number   ;
      m_distance    = L.m_distance ;
      m_impact      = L.m_impact   ;
      m_zlocal      = L.m_zlocal   ;
      m_way         = L.m_way      ;
      m_sdistance   = L.m_sdistance;
      m_szlocal     = L.m_szlocal  ;  
      return(*this);
    }

  inline TRT_DriftCircleLink_xk::~TRT_DriftCircleLink_xk() {}

  inline void TRT_DriftCircleLink_xk::set
    (const int& n,double& d,double& im,double& zl, double& s) 
    {
      m_cluster      = 0 ;
      m_number       = n ;
      m_distance     = d ;
      m_impact       = im;
      m_zlocal       = zl;
      m_way          = s ;
      m_sdistance    = 0.; 
      m_szlocal      = 0.;
    }

  inline void TRT_DriftCircleLink_xk::set
    (const int& n,double& d,double& im,double& zl, double& s,double& sd,double& sz) 
    {
      set(n,d,im,zl,s); m_sdistance = sd; m_szlocal = sz;;
    }

  inline void TRT_DriftCircleLink_xk::set
    (const TRT_DriftCircle* const& c) 
    {
      m_cluster = c ;
    }

  inline void TRT_DriftCircleLink_xk::newImpactParameter(const double& d)
    {
      if((m_impact*m_distance) > 0.) {m_distance = d; m_impact = d;}
      else                           {m_distance = d; m_impact =-d;}
    }

  inline const Trk::Surface&  TRT_DriftCircleLink_xk::surface() const
    {
      return m_cluster->detectorElement()->surface(m_cluster->identify());
    }

  inline void TRT_DriftCircleLink_xk::radiusCorrection(const double& dr)
    {
      m_zlocal+=(m_szlocal*dr);
      double d = m_sdistance*dr; 
      if((m_impact*m_distance) > 0.) {m_distance+=d; m_impact+=d;} 
      else                           {m_distance+=d; m_impact-=d;}
    }

} // end of name space

#endif // TRT_DriftCircleLink_xk


