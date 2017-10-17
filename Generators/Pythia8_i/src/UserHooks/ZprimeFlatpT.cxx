#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

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
      std::cout<<" mWidth="<<wRes<<std::endl;
      double m2Res   = mRes*mRes;
      double GamMRat = wRes/mRes;
      double sHat = phaseSpacePtr->sHat();
      long  double weightBW = pow2(sHat - m2Res) + pow2(sHat * GamMRat);
      long  double weightpT= 1;
      double rH = sqrt(sHat);

      double par[2]={-8.95719e+00,1.62584e-03};
      weightpT=exp(par[0]+par[1]*rH);
      if(rH>=3000){ weightpT=0; }
 
      long  double weight = weightBW*(weightpT);
      return weight;

    }
    
  };  

} // end namespace Pythia8
