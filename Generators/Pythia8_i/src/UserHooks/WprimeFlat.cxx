/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

namespace Pythia8{
  class WprimeFlat;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::WprimeFlat> WprimeFlatCreator("WprimeFlat");

namespace Pythia8 {

  class WprimeFlat : public UserHooks {
    
  public:
    
    // Constructor.
    WprimeFlat(){}
    
    // Destructor.
    ~WprimeFlat(){}
    
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
      double GamMRat = wRes/mRes;
      double sHat = phaseSpacePtr->sHat();
      double weightBW = pow2(sHat - m2Res) + pow2(sHat * GamMRat);      
      double m = sqrt(sHat)/8000.0;
      if(m < 0.0375)
       weightBW *= 121.88e-12*exp(13.0*m);
      else
       weightBW *= 1.0e-12*exp(18.5*m-1.4*log(m));

      return weightBW;
    }
    
  };  

} // end namespace Pythia8


