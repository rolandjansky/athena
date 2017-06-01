#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

#include <stdexcept>

namespace Pythia8{
  class mHatReweight;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::mHatReweight> mHatReweightCreator("mHatReweight");

namespace Pythia8 {

  class mHatReweight : public UserHooks {
    
  public:
   
    // Constructor
    mHatReweight()
        : m_slope("mHatReweight:Slope", 1) 
        , m_mHatConstMin("mHatReweight:mHatConstMin", 1500) 
        , m_p1("mHatReweight:p1", -3.872) 
        , m_p2("mHatReweight:p2", 15.606) 
    {
      std::cout<<"*************************************************************"<<std::endl;
      std::cout<<"*                                                           *"<<std::endl;
      std::cout<<"*  Populating high-mass events with mHatReweight UserHook!  *"<<std::endl;
      std::cout<<"*                                                           *"<<std::endl;
      std::cout<<"*************************************************************"<<std::endl;
    }
 
    // Allow process cross section to be modified...
    virtual bool canModifySigma() {return true;}
  
    // ...which gives access to the event at the trial level, before selection.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr, 
				   const PhaseSpace* phaseSpacePtr, 
				   bool /* inEvent */) {
      // All events should be 2 -> 1, but kill them if not.
      if (sigmaProcessPtr->nFinal() != 1) return 0.;
      // config
      double slope = m_slope(settingsPtr); 
      double mHatConstMin = m_mHatConstMin(settingsPtr);
      double p1 = m_p1(settingsPtr);
      double p2 = m_p2(settingsPtr);
      // event kinematics 
      double mHat = sqrt(phaseSpacePtr->sHat());
      double ecm = phaseSpacePtr->ecm();
      
      // scale factor 
      double scale = 0.0;
      // low-mass slope modification
      if( mHat < mHatConstMin ){
        scale = pow(mHat/ecm, slope);
      }
      // const high-mass
      else {
        double xsec    = pow(mHat/ecm, p1) * pow(1 - mHat/ecm, p2);
        double xsecMin = pow(mHatConstMin/ecm, p1) * pow(1 - mHatConstMin/ecm, p2);
        scale = pow(mHatConstMin/ecm, slope) * xsecMin / xsec;
      }
      return scale; 
    }

  private:
    // User-settable slope for power-law scaling (s^m)
    Pythia8_UserHooks::UserSetting<double> m_slope;
    Pythia8_UserHooks::UserSetting<double> m_mHatConstMin;
    Pythia8_UserHooks::UserSetting<double> m_p1;
    Pythia8_UserHooks::UserSetting<double> m_p2;
  };  

} // end namespace Pythia8


