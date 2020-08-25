/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UserHooksUtils.h"
#include "Pythia8_i/UserHooksFactory.h"
#include <stdexcept>
#include <iostream>

namespace Pythia8{
  class QCDVetoedShower;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::QCDVetoedShower> qcdVetoedShowerCreator("QCDVetoedShower");

namespace Pythia8{

  /**
   *  This UserHook should be used when showering QCD jets generated with PoWHEG-box.
   *  The following settings must be activated in Pythia:
   *
   *    SpaceShower:pTMaxMatch = 2
   *    TimeShower:pTMaxMatch  = 2
   *
   *  These set the limit for emission from both initial and final state radiation to the kinematic (beam) energy.
   *  The veto hook then vetos any emission that would be above the appropriate scale.
   *
   *  In QCD, emissions could come from any PoWHEG leg, therefore each candidate Pythia emission has its pT determined
   *  relative to each PoWHEG leg, including the beams.  If any of these pTs is larger than the PoWHEG veto scale then
   *  that proposed emission is rejected.
   *
   *  this differs from colourless resonance production, which requires a different scheme and therefore this hook...
   *
   *              ...SHOULD NOT BE USED FOR W/Z OR OTHER COLOURLESS RESONANCE PRODUCTION!!
   */
  class QCDVetoedShower : public UserHooks{

  public:

    QCDVetoedShower(): m_powhegScale(0.){

      std::cout<<"*******************************************************************"<<std::endl;
      std::cout<<"*                                                                 *"<<std::endl;
      std::cout<<"*        Using vetoed shower for PoWHEG QCD production!           *"<<std::endl;
      std::cout<<"*                                                                 *"<<std::endl;
      std::cout<<"*******************************************************************"<<std::endl;
      
    }

    ~QCDVetoedShower(){}

    /**
     *  doVetoMPIStep is called immediately after the MPI generation
     *  In this case it never actually vetoes the MPI, but since it is called
     *  before the ISR veto check this is a convenient place to find the PoWHEG
     *  scale from the LHEF event
     */
    bool doVetoMPIStep(int nMPI, const Event &evt){

      // Only do anything on the first call.
      if(nMPI > 1) return false;
      
      m_powhegScale = infoPtr->QRen();
            
      m_powhegLegs.clear();
      // start the loop at 1, since entry 0 represents the event as a whole
      for(int ii=1; ii != evt.size(); ++ii){
        // here Event::isFinal refers to whether the particle can still decay/radiate, or whether it is an internal leg
        if(evt[ii].isFinal()){
          m_powhegLegs.push_back(ii);
        }
      }
      
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

      return checkEmission(emission, evt);

    }
    
    /**
     *  This is similar to the ISR veto
     *
     */
    bool doVetoFSREmission(int, const Event &evt, int iSys, bool){

      // only veto emissions from the hard system
      if(iSys != 0) return false;
      
      size_t emission = Pythia8_UserHooks::findLastFSREmission(evt);

      return checkEmission(emission, evt);
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
    
    bool checkEmission(size_t emission, const Event &evt){
      // Veto if above the POWHEG scale
      if (evt[emission].pT() > m_powhegScale) return true;
      
      // determine pT relative to each outgoing PoWHEG leg and veto if above veto scale
            
      for(std::vector<size_t>::const_iterator legIndex = m_powhegLegs.begin();
          legIndex != m_powhegLegs.end(); ++legIndex){
        
        if(Pythia8_UserHooks::pTLeg(emission, *legIndex, evt) > m_powhegScale)return true;
      }
      return false;
    }
    
    double m_powhegScale;
    // Indices of the PoWHEG legs in the event record
    std::vector<size_t> m_powhegLegs;
  };
}

