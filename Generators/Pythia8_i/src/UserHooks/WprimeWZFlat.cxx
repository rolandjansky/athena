/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

#include "UserSetting.h"

namespace Pythia8{
  class WprimeWZFlat;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::WprimeWZFlat> WprimeWZFlatCreator("WprimeWZFlat");

namespace Pythia8 {
  
  class WprimeWZFlat : public UserHooks {
    
  public:
    
    // Constructor.
    WprimeWZFlat(){}
    
    // Destructor.
    ~WprimeWZFlat(){}
    
    // Allow process cross section to be modified...
    virtual bool canModifySigma() {return true;}
    
    // ...which gives access to the event at the trial level, before selection.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr, 
                                   const PhaseSpace* phaseSpacePtr, 
                                   bool /* inEvent */) {
      // All events should be 2 -> 1, but kill them if not.
      if (sigmaProcessPtr->nFinal() != 1) return 0.; 
      
      // Weight cross section with BW propagator, i.e. to remove it.
      // (inEvent = false for initialization).
      // No inEvent criteria, want weight both for cross section 
      // and MC generation.
      int idRes   = sigmaProcessPtr->resonanceA();
      double mRes = particleDataPtr->m0(idRes);
      double wRes = particleDataPtr->mWidth(idRes);
      double m2Res   = mRes*mRes;
      double gamMRat = wRes/mRes;
      double sHat = phaseSpacePtr->sHat();
      double weightBW = m2Res*m2Res + sHat*sHat*(1 + gamMRat*gamMRat) - 2.*sHat*m2Res;      
      double rH = sqrt(sHat);
      
      return (m_flattenPT(settingsPtr)) ? weightBW * pTWeight(rH) : weightBW * breitWignerDenom(rH/settingsPtr->parm("Beams:eCM"));
    }
    
  private:
    
    double breitWignerDenom(double mFrac){
            
      if(mFrac < 0.025) return breitWignerDenom(0.025);
      if(mFrac > 0.6) return breitWignerDenom(0.6);
      
      if(mFrac < 0.0425) return 1e-12/(-1.293+1.098e+2*mFrac-2.800e+3*mFrac*mFrac+2.345e+4*mFrac*mFrac*mFrac);
      if(mFrac < 0.073) return 1.248e-12*(exp(1.158+18.34*mFrac));
      
      return 5.733e-10*pow(mFrac,-3.798-0.6555*log(mFrac))/pow(1.427-mFrac,30.017);
    }
    
    double pTWeight(double rH){
      
      double pe0 = 9.705/2000.;
      double pe1 = -1.27668e-03;
      
      double weightHighpT =1./(exp(pe0+pe1*rH));
      
      double p0 = 0.00405295;
      double p1 = -1.15389e-06;
      double p2 =  -8.83305e-10;
      double p3 =  1.02983e-12;
      double p4 = -3.64486e-16;
      double p5 = 6.05783e-20;
      double p6 = -4.74988e-24;
      double p7 = 1.40627e-28;
      double weightFinal = (p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+                                                       (p6*pow(rH,6))+(p7*pow(rH,7)));
      
      if(rH < 400.) weightFinal *= 0.5;
      
      return weightHighpT * weightFinal;
    }
    
    // This switch says whether to use the old style flattening of the Breit Wigner, or additionally flatten the PT spectrum.
    // Off by default, for consistency with old production jobs
    Pythia8_UserHooks::UserSetting<int> m_flattenPT = Pythia8_UserHooks::UserSetting<int>("WprimeWZFlat:FlattenPT", 0);
    
  };  
  
} // end namespace Pythia8


