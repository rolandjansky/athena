/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class NoiseOnSurface
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for noises on surface
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef NoiseOnSurface_H
#define NoiseOnSurface_H

class MsgStream;

namespace Trk {

  class NoiseOnSurface
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      NoiseOnSurface();
      NoiseOnSurface(const NoiseOnSurface&);
      ~NoiseOnSurface();
      NoiseOnSurface& operator  = (const NoiseOnSurface&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const double&  covarianceAzim() const {return m_covarianceAzim;}
      const double&  covariancePola() const {return m_covariancePola;}
      const double&  covarianceIMom() const {return m_covarianceIMom;}
      const double&  correctionIMom() const {return m_correctionIMom;}
      void initiate();
      void set(const double&,const double&,const double&,const double&);

      ///////////////////////////////////////////////////////////////////
      // Print
      ///////////////////////////////////////////////////////////////////

      std::ostream& dump(std::ostream&) const;
      MsgStream&    dump(MsgStream&   ) const;	

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      double m_covarianceAzim;
      double m_covariancePola;
      double m_covarianceIMom;
      double m_correctionIMom;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };

  /////////////////////////////////////////////////////////////////////////////////
  // Overload operator
  /////////////////////////////////////////////////////////////////////////////////

  std::ostream& operator << (std::ostream&,const NoiseOnSurface&); 
  MsgStream& operator    << (MsgStream&, const NoiseOnSurface& );
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline NoiseOnSurface::NoiseOnSurface():m_covarianceAzim(0.), m_covariancePola(0.),
    m_covarianceIMom(0.), m_correctionIMom(0.)
    {
    }

  inline NoiseOnSurface::NoiseOnSurface(const NoiseOnSurface& N)
    {
      m_covarianceAzim = N.m_covarianceAzim;
      m_covariancePola = N.m_covariancePola;
      m_covarianceIMom = N.m_covarianceIMom;
      m_correctionIMom = N.m_correctionIMom;
    }
  
  inline NoiseOnSurface& NoiseOnSurface::operator = (const NoiseOnSurface& N) 
    {
      if (&N != this){
        m_covarianceAzim = N.m_covarianceAzim;
        m_covariancePola = N.m_covariancePola;
        m_covarianceIMom = N.m_covarianceIMom;
        m_correctionIMom = N.m_correctionIMom;
      }
      return(*this);
    }

  inline NoiseOnSurface::~NoiseOnSurface() {}

  inline void NoiseOnSurface::initiate()
    {
      m_covarianceAzim = 0.;
      m_covariancePola = 0.;
      m_covarianceIMom = 0.;
      m_correctionIMom = 1.;
    }

  inline void NoiseOnSurface::set
    (const double& az,const double& po,const double& im,const double& imc)
    {
      m_covarianceAzim = az ;
      m_covariancePola = po ;
      m_covarianceIMom = im ;
      m_correctionIMom = imc;
    }

} // end of name space

#endif // NoiseOnSurface
