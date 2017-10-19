#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "boost/lexical_cast.hpp"
#include <stdexcept>
#include <iostream>

namespace Pythia8{
  class SuppressMPI;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::SuppressMPI> SuppressMPICreator("SuppressMPI");

namespace Pythia8{

  class SuppressMPI : public UserHooks{
    
  public:
    
    SuppressMPI():
    m_pTCut("SuppressMPI:PTCut", 10.), m_nMPIVeto(3){
      
      std::cout<<"************************************************************"<<std::endl;
      std::cout<<"*                                                          *"<<std::endl;
      std::cout<<"*        Suppressing MPI emissions with UserHook!          *"<<std::endl;
      std::cout<<"*                                                          *"<<std::endl;
      std::cout<<"************************************************************"<<std::endl;

    }
    
    ~SuppressMPI(){}
    
    bool doVetoMPIStep(int nMPI, const Event &event){
            
      // MPI 1 is the hard process.  We do not veto that!
      if(nMPI < 2){
        return false;
      }
      
      // start at the end of the event record and work back
      // This is prior to showering, so there should be at most 2 new MPI emissions
      // event[0] is documentation, so stop before that.
      size_t nEmissions=0;
      for(int ii=event.size()-1; ii > 0 && nEmissions != 2; --ii){
        if(event[ii].status() != 33) continue;
        if(event[ii].pT() > m_pTCut(settingsPtr)){
          return true;
        }
        
        ++nEmissions;
      }
      
      return false;
    }
    
    /// Switch on calling of doVetoMPIStep
    bool canVetoMPIStep(){return true;}
    /// Call doVetoMIStep three times
    /// First is the hard process
    /// second is first MPI emission 
    ///           *or* 
    /// the second part of a double diffractive event
    ///           *or*
    /// the second hard process if there is on.
    /// Therefore check up to 3
    int numberVetoMPIStep(){return m_nMPIVeto;}
    /// Switch on veto of ISR
    bool canVetoISREmission(){return false;}
    /// Switch off veto of FSR
    bool canVetoFSREmission(){return false;}
    
  private:
    
    Pythia8_UserHooks::UserSetting<double> m_pTCut;
    
    int m_nMPIVeto;
    
    
  };
  
  

}
