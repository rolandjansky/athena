/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HadronicRecoilData/ConeBuilder.h"
#include <iostream>

namespace HadronicRecoil {
  
  Cone* MakeRndCone( double eta,
		     double radius,
		     const ConeContainer* coneCont,
		     const TransverseEnergy& roughRecoil,
		     UInt_t seed ){

    double phi = 999.;
    bool hasOverlap = true;
    TRandom3 m_rnd(seed);

    int nloops = 0;

    while( hasOverlap )
      {
	// This is a trick to prevent infinite loops
        //std::cout << "n loops = " << nloops << std::endl;
	++nloops;
	
	if( nloops>1000 ){

	  std::cout << "WARNING: HadronicRecoilData/ConeBuilder. No UE correction because of possible infinity loop" << std::endl;

	  Cone* rndCone = new Cone( -999. , -999, 0.);
	  return rndCone;
	}
	
	// Phi is generated randomly
	phi = m_rnd.Uniform( -(M_PI-radius/2.), M_PI-radius/2.);

	// Eta sign
	if( m_rnd.Uniform(-1 , 1) < 0 ){
	  eta *= -1;
	}
	
	if( coneCont ){
	  // if the container size is zero --> no need to check the overlap
	  if( coneCont->size() == 0 ){
	    hasOverlap = false;
	  }
	    

	  hasOverlap = CheckOverlap(coneCont, eta, phi, radius);
	    
	  //if the cone is not defined --> no need to check the overlap
	}else {
	  hasOverlap = false;
	}
	
	if( !hasOverlap ){
	    
	  hasOverlap = CheckOverlap(roughRecoil, phi, 0.6);
	}
	  
      }
      
    // Make the cone
    Cone* rndCone = new Cone( eta, phi, radius);
      
    return rndCone;

  }

  //__________________________________________________________________
  bool CheckOverlap( const ConeContainer* coneVec, double eta, double phi, double radius)
  {
    double DR = 0.;
    double en_eta = 0.;
    double en_phi = 0.;

    for(unsigned int i=0; i < coneVec->size(); ++i){

      en_eta = coneVec->at(i)->Eta();
      en_phi = coneVec->at(i)->Phi();

      DR = DeltaR( eta, phi, en_eta, en_phi );

      if( TMath::Abs(DR) <= 2*radius ){
	return true;
      }
    }
    
    return false;
  }

  /** Returns true if the "rough recoil" and the cone center are closer than Dphi */
  bool CheckOverlap( const TransverseEnergy& roughRecoil, double phi, double Dphi)
  {
    if( DeltaPhi(roughRecoil.phi(), phi) < Dphi ){
      return true;
    }
    return false;
  }
}
