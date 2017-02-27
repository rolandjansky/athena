// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUTIL_JETDISTANCES_H
#define JETUTIL_JETDISTANCES_H
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <cmath>


namespace jet {
  namespace JetDistances {
    
    // phi convention enforcement
    inline static double fixedPhi(double aPhi)
    {
      while ( aPhi < -M_PI )
        {
          aPhi += 2.*M_PI;
        }
      while ( aPhi > M_PI )
        {
          aPhi -= 2.*M_PI;
        }
      return aPhi;
    }

    
    template<class PARTICLE>
    inline static double deltaEta(const PARTICLE& jet1,
                                  const PARTICLE& jet2)
    { return jet1.eta() - jet2.eta(); }
  
    inline static double deltaEta(const double eta1,
                                  const double eta2)
    { return (eta1 - eta2); }
    
    template<class PARTICLE>
    inline static double deltaPhi(const PARTICLE& jet1, 
                                  const PARTICLE& jet2)
    { 
      return fixedPhi(jet1.phi() - jet2.phi());
    }
    
    
    inline static double deltaPhi(const double phi1,
                                  const double phi2)
    { 
      return fixedPhi( phi1 - phi2 );
    }

    
    template<class PARTICLE>
    inline static double deltaR2(const PARTICLE& jet1, 
                                const PARTICLE& jet2){ 
      return  ( deltaEta(jet1,jet2) * deltaEta(jet1,jet2) + 
                deltaPhi(jet1,jet2) * deltaPhi(jet1,jet2) ); 
    }
    
    inline static double deltaR2(const double eta1, const double phi1,
                                const double eta2, const double phi2 ) { 
      return ( deltaEta(eta1,eta2) * deltaEta(eta1,eta2) +
               deltaPhi(phi1,phi2) * deltaPhi(phi1,phi2) ); 
    }

    template<class PARTICLE>
    inline static double deltaR(const PARTICLE& jet1, 
                                const PARTICLE& jet2){ 
      return sqrt( deltaR2(jet1, jet2) );                   
    }
    
    inline static double deltaR(const double eta1, const double phi1,
                                const double eta2, const double phi2 ) { 
      return sqrt( deltaR2(eta1, phi1, eta2, phi2) ); 
    }

        

  } // namespace JetDistances

} // namespace jet

#endif
