#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

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
      
      return weightBW * breitWignerDenom(sqrt(sHat)/8000.0);
    }
    
  private:
    
    double breitWignerDenom(double mFrac){
      
      if(mFrac < 0.025) return breitWignerDenom(0.025);
      if(mFrac > 0.6) return breitWignerDenom(0.6);
      
      if(mFrac < 0.0425) return 1e-12/(-1.293+1.098e+2*mFrac-2.800e+3*mFrac*mFrac+2.345e+4*mFrac*mFrac*mFrac);
      if(mFrac < 0.073) return 1.248e-12*(exp(1.158+18.34*mFrac));
      
      return 5.733e-10*pow(mFrac,-3.798-0.6555*log(mFrac))/pow(1.427-mFrac,30.017);
    }
    
  };  
  
} // end namespace Pythia8


