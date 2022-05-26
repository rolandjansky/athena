/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

#include "UserSetting.h"

namespace Pythia8{
  class ZprimeFlatpT;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::ZprimeFlatpT> zPrimeFlatpTCreator("ZprimeFlatpT");

namespace Pythia8 {

  class ZprimeFlatpT : public UserHooks {
    
  public:
    
    // Constructor.
    ZprimeFlatpT(){}
    
    // Destructor.
    ~ZprimeFlatpT(){}
    
    // Allow process cross section to be modified...
    virtual bool canModifySigma() {return true;}
  
    // ...which gives access to the event at the trial level, before selection.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr, 
				   const PhaseSpace* phaseSpacePtr, 
				   bool /* inEvent */) {
      // All events should be 2 -> 1, but kill them if not.
     // if (sigmaProcessPtr->nFinal() != 1) return 0.; 
      if (sigmaProcessPtr->nFinal() != 1) return 0.; 
      
      // Weight cross section with BW propagator, i.e. to remove it.
      // (inEvent = false for initialization).
      // No inEvent criteria, want weight both for cross section 
      // and MC generation.
      int idRes   = sigmaProcessPtr->resonanceB();
      double mRes = particleDataPtr->m0(idRes);

      double wRes = particleDataPtr->mWidth(idRes);
      double m2Res   = mRes*mRes;
      double GamMRat = wRes/mRes;
      double sHat = phaseSpacePtr->sHat();
      long  double weightBW = pow2(sHat - m2Res) + pow2(sHat * GamMRat);
      long  double weightpT= 1;
      double rH = std::sqrt(sHat);

      double par[2]={-8.95719e+00,1.62584e-03};
      weightpT=std::exp(par[0]+par[1]*rH);
      if(rH>=m_maxSHat(settingsPtr)){ weightpT=0; }
 
      double weightDecay = 1.;
      
      if(rH < m_doDecayWeightBelow(settingsPtr)){
        double p0 = -0.000527117;
        double p1 =  2.64665e-06;
        
        weightDecay = 0.008/(p0+(p1*rH));
      }
      
      long  double weight = weightBW * weightpT * weightDecay;
      return weight;

    }
    
    private:
    
    // This sets the \sqrt{\hat{s}} above which no events will be generated
    // It is 3000. GeV by default for consistency with the first incarnation of this hook
    Pythia8_UserHooks::UserSetting<double> m_maxSHat = Pythia8_UserHooks::UserSetting<double>("ZprimeFlatpT:MaxSHat", 3000.);
    
    // This sets the \sqrt{\hat{s}} below which an additional decay weight will be added
    // It is zero by default, since this feature did not exist inthe earlier incarnation of this hook
    Pythia8_UserHooks::UserSetting<double> m_doDecayWeightBelow = Pythia8_UserHooks::UserSetting<double>("ZprimeFlatpT:DoDecayWeightBelow", 0.);
    
    
  };  

} // end namespace Pythia8
