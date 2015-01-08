/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiNoise_bt
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for noises
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 5/17/2006 T.Koffas
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiNoise_bt_H
#define SiNoise_bt_H

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

//class Trk::TrackParameters;

namespace InDet{

  class SiNoise_bt
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiNoise_bt();
      SiNoise_bt(const SiNoise_bt&);
      ~SiNoise_bt();
      SiNoise_bt& operator  = (const SiNoise_bt&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const int&     model         () const {return m_model         ;} 
      const double&  covarianceAzim() const {return m_covarianceAzim;}
      const double&  covariancePola() const {return m_covariancePola;}
      const double&  covarianceIMom() const {return m_covarianceIMom;}
      const double&  correctionIMom() const {return m_correctionIMom;}

      void initiate();
      void production(int,int,const Trk::TrackParameters&);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      int    m_model         ;
      double m_covarianceAzim;
      double m_covariancePola;
      double m_covarianceIMom;
      double m_correctionIMom;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiNoise_bt::SiNoise_bt()
    {
      initiate();
    }

  inline SiNoise_bt::SiNoise_bt(const SiNoise_bt& N)
    {
      m_model          = N.m_model         ;
      m_covarianceAzim = N.m_covarianceAzim;
      m_covariancePola = N.m_covariancePola;
      m_covarianceIMom = N.m_covarianceIMom;
      m_correctionIMom = N.m_correctionIMom;
    }
  
  inline SiNoise_bt& SiNoise_bt::operator = (const SiNoise_bt& N) 
    {
      m_model          = N.m_model         ;
      m_covarianceAzim = N.m_covarianceAzim;
      m_covariancePola = N.m_covariancePola;
      m_covarianceIMom = N.m_covarianceIMom;
      m_correctionIMom = N.m_correctionIMom;
      return(*this);
    }

  inline SiNoise_bt::~SiNoise_bt() {}

  inline void SiNoise_bt::initiate()
    {
      m_model          = 0 ;
      m_covarianceAzim = 0.;
      m_covariancePola = 0.;
      m_covarianceIMom = 0.;
      m_correctionIMom = 1.;
    }

} // end of name space

#endif // SiNoise_bt


