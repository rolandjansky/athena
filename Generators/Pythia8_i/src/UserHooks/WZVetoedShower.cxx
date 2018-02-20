#include "UserHooksUtils.h"
#include "Pythia8_i/UserHooksFactory.h"
#include <stdexcept>
#include <iostream>

namespace Pythia8{
  class WZVetoedShower;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::WZVetoedShower> wzVetoedShowerCreator("WZVetoedShower");

namespace Pythia8{

  /**
   *  This UserHook should be used when showering W, Z or other colourless resonances generated with PoWHEG.
   *  The following setting must be activated in Pythia:
   *
   *    SpaceShower:pTMaxMatch = 2
   *    TimeShower:pTMaxMatch  = 2
   *
   *  These set the limit for emission from both initial and final state radiation to the kinematic (beam) energy.
   *  The veto hook then vetos any emission that would be above the appropriate scale.
   *
   *  In the cases of colourless resonance production there should be no FSR emission from PoWHEG, therefore the veto
   *  determines the pT of every Pythia emission relative to the beam, and vetos if it is above the PoWHEG scale.
   *
   *  This is ok because 
   *
   *    1) Although you might think the pT should also be determined relative to any PoWHEG ISR emission, there is no way 
   *       PoWHEG could have generated an emission off its own ISR.
   *    2) There can be FSR off what was a PoWHEG ISR leg, but again, PoWHEG could never have generated that emission, so you
   *       only need to compare to the incoming (beam) legs.
   * 
   *  This is in contrast to QCD production, which requires a different veto scheme.  As such, this hook...
   *
   *               ...SHOULD NOT BE USED FOR QCD JET PRODUCTION!!
   *
   */

  class WZVetoedShower : public UserHooks{

  public:

    WZVetoedShower(): m_powhegScale(0.){

      std::cout<<"*******************************************************************"<<std::endl;
      std::cout<<"*                                                                 *"<<std::endl;
      std::cout<<"* Using vetoed shower for PoWHEG colourless resonance production! *"<<std::endl;
      std::cout<<"*                                                                 *"<<std::endl;
      std::cout<<"*******************************************************************"<<std::endl;
      
    }

    ~WZVetoedShower(){}

    /**
     *  doVetoMPIStep is called immediately after the MPI generation
     *  In this case it never actually vetoes the MPI, but since it is called
     *  before the ISR veto check this is a convenient place to find the PoWHEG
     *  scale from the LHEF event
     */
    bool doVetoMPIStep(int nMPI, const Event &){

      // Only do anything on the first call.
      if(nMPI > 1) return false;
      
      m_powhegScale = infoPtr->QRen();
//      m_powhegScale = infoPtr->QFac();

//      std::cout<<" ******** veto scale = "<<m_powhegScale<<", "<<infoPtr->QRen() <<" *********"<<std::endl;  
      
      return false;
    }

    /**
     *  This is called after the generation of each new ISR emission
     *  Can use it to test if the last generated emission is above the
     *  veto scale
     *
     */
    bool doVetoISREmission(int, const Event &evt, int iSys){

      // only veto emissions from the hard system
      if(iSys != 0) return false;
      
      size_t emission = Pythia8_UserHooks::findLastISREmission(evt);

      // Veto emission above the veto scale
      if(evt[emission].pT() > m_powhegScale) return true;

      return false;
    }
    /**
     *  This is similar to the ISR veto
     *
     */
    bool doVetoFSREmission(int, const Event &evt, int iSys, bool){

      // only veto emissions from the hard system
      if(iSys != 0) return false;
      
      size_t emission = Pythia8_UserHooks::findLastFSREmission(evt);

      // Veto if above the POWHEG scale
      if (evt[emission].pT() > m_powhegScale) return true;

      return false;
    }

    /// Switch on calling of doVetoMPIStep
    bool canVetoMPIStep(){return true;}
    /// Call doVetoMIStep once
    int numberVetoMPIStep(){return 1;}
    /// Switch on veto of ISR
    bool canVetoISREmission(){return true;}
    /// Switch off veto of FSR
    bool canVetoFSREmission(){return true;}

  private:

    double m_powhegScale;
  };
}

