/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMomentumCorrections/BrandeisMomentumResolution.h"
#include <cmath>

using namespace std;

namespace Brandeis {

  // The function below returns the ID momentum resolution, in percent        
  // muon trasverse momentum is taken as argument and must be in **GeV** 
  //--------------------------------------------------------------------      
  double GetMomentumResolutionID( double mu_pt, double mu_eta, bool isMC )
  {
    const double par1_ID = ( isMC ? 1.78254 : 1.93223 );
    const double par2_ID = ( isMC ? 0.26479 : 0.29712 );
    const double PEAK = 0.75;
    const double WIDTH1 = 0.10;
    const double WIDTH2 = 0.15;

    // protections
    if(fabs(mu_eta)>2.5) return 999.;    
    
    // "canonical" term, describing multiple scattering and alignment constributions
    double sigmaID = 100*sqrt((par1_ID/100)*(par1_ID/100) + (par2_ID/1000)*(par2_ID/1000)*(mu_pt*mu_pt-1600));
    
    // ID peculiar features
    double sigmaPtEta = 0.;
    if (fabs(mu_eta) < Brandeis::etaID )
      {
	sigmaPtEta = sigmaID * (1.0 + (Brandeis::p2ID + mu_pt*Brandeis::p3ID + mu_pt*mu_pt*Brandeis::p4ID + mu_pt*mu_pt*mu_pt*Brandeis::p5ID)*pow(fabs(mu_eta),2)
				+ (Brandeis::p6ID  + mu_pt*Brandeis::p7ID  + mu_pt*mu_pt*Brandeis::p8ID  + mu_pt*mu_pt*mu_pt*Brandeis::p9ID) *pow(fabs(mu_eta),4)
				+ (Brandeis::p10ID + mu_pt*Brandeis::p11ID + mu_pt*mu_pt*Brandeis::p12ID + mu_pt*mu_pt*mu_pt*Brandeis::p13ID)*pow(fabs(mu_eta),6)
				+ (Brandeis::p14ID + mu_pt*Brandeis::p15ID + mu_pt*mu_pt*Brandeis::p16ID + mu_pt*mu_pt*mu_pt*Brandeis::p17ID)*pow(fabs(mu_eta),8));
      } else
      {
	sigmaPtEta = sigmaID * (1.0 + (Brandeis::p2ID + mu_pt*Brandeis::p3ID + mu_pt*mu_pt*Brandeis::p4ID + mu_pt*mu_pt*mu_pt*Brandeis::p5ID)*pow(Brandeis::etaID,2)
				+ (Brandeis::p6ID  + mu_pt*Brandeis::p7ID  + mu_pt*mu_pt*Brandeis::p8ID  + mu_pt*mu_pt*mu_pt*Brandeis::p9ID) *pow(Brandeis::etaID,4)
				+ (Brandeis::p10ID + mu_pt*Brandeis::p11ID + mu_pt*mu_pt*Brandeis::p12ID + mu_pt*mu_pt*mu_pt*Brandeis::p13ID)*pow(Brandeis::etaID,6)
				+ (Brandeis::p14ID + mu_pt*Brandeis::p15ID + mu_pt*mu_pt*Brandeis::p16ID + mu_pt*mu_pt*mu_pt*Brandeis::p17ID)*pow(Brandeis::etaID,8)
				+ (Brandeis::p18ID + mu_pt*Brandeis::p19ID + mu_pt*mu_pt*Brandeis::p20ID + mu_pt*mu_pt*mu_pt*Brandeis::p21ID)*pow(fabs(mu_eta) - Brandeis::etaID,1)
				+ (Brandeis::p22ID + mu_pt*Brandeis::p23ID + mu_pt*mu_pt*Brandeis::p24ID + mu_pt*mu_pt*mu_pt*Brandeis::p25ID)*pow(fabs(mu_eta) - Brandeis::etaID,2) );
      }
    
    // peak near eta ~ 0.75
    double sigmaPeak = (Brandeis::p26ID + mu_pt*Brandeis::p27ID)*exp(-0.5*pow((fabs(mu_eta)-PEAK)/WIDTH1,2));
    
    // peak at eta ~ 0 -> WILL BE IMPROVED
    double sigmaZero = ((Brandeis::p28ID)*exp(-fabs(mu_eta)/WIDTH2));

    
    // merge various terms -> return % resolution
    return sqrt( pow(sigmaPtEta*(1.0 + sigmaZero),2) + pow(sigmaPeak,2) );
  }

} // namespace
