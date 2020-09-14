/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

namespace Pythia8{
  class SingleTopWideEta;
}


Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::SingleTopWideEta> singleTopWideEta("SingleTopWideEta");

namespace Pythia8 {

  class SingleTopWideEta : public UserHooks {
    
  public:
    
    // Constructor.
    SingleTopWideEta(){}
    
    // Destructor.
    ~SingleTopWideEta(){}
    
    // Allow process cross section to be modified...
    virtual bool canModifySigma() {return true;}
  
    // ...which gives access to the event at the trial level, before selection.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr, 
				   const PhaseSpace* phaseSpacePtr, 
				   bool /* inEvent */) {
      // Throw up on events that are not t-channel single top
      if(sigmaProcessPtr->code() != 603){
        throw std::runtime_error("SingleTopWideEta: Can only be run on qq -> tq (t-channel W) events, code 603. Event had code" + std::to_string(sigmaProcessPtr->code()) + ".");
      }
      
      double sHat = phaseSpacePtr->sHat();
      double flatEta= 1.;
      double rH = std::sqrt(sHat);

      if(rH <= 160.){
        double c = -5.55578e+01;
        double slope=  2.88096e-01;
        flatEta = exp(c+slope*rH);
      }


      if( 160.  < rH  && rH <  180. ){
        double g1 = 8.07441e-1 ;
        double g2 = 1.7313e2 ;
        double g3 = 2.4357;
        flatEta = g1*std::exp(-0.5*std::pow((g2-rH)/g3,2));
      }


      if(rH >= 180.){
        double c1 = 8.84562;
        double slope1 = -9.22426e-2;
        flatEta = std::exp(c1+slope1*rH);
      }

      return flatEta;

    }
    
  };  

} // end namespace Pythia8
