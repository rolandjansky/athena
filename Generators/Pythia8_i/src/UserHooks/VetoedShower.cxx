/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"
#include <stdexcept>
#include <iostream>

namespace Pythia8{
  class VetoedShower;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::VetoedShower> vetoedShowerCreator("VetoedShower");

namespace Pythia8{


  class VetoedShower : public UserHooks{

  public:

    VetoedShower(): m_pTVeto(0.),  m_doneISR(false){
      std::cout<<std::endl;
      std::cout<<"************************************************************************************************"<<std::endl;
      std::cout<<"Pythia8::VetoedShower: *** WARNING: THIS USER HOOK IS NOT VALIDATED FOR QCD JET PRODUCTION!! ***"<<std::endl;
      std::cout<<"************************************************************************************************"<<std::endl;
      std::cout<<std::endl;
    }

    ~VetoedShower(){}

    /**
     *  doVetoMPIStep is called immediately after the MPI generation
     *  In this case it nver actually vetoes the MPI, but since it is called
     *  before the ISR veto check this is a convenient place to find the veto
     *  scale from the LHEF event
     */
    bool doVetoMPIStep(int, const Event&){

      m_pTVeto = infoPtr->QFac();
      m_doneISR = false;

//      std::cout<<" ******** veto scale = "<<m_pTVeto<<" *********"<<std::endl;

      return false;
    }

    /**
     *  This is called after the generation of each new ISR emission
     *  Can use it to test if the last generated emission is above the
     *  veto scale
     *
     *  Since the ISR is ordered in pT (with the same definition relative to the
     *  beam as Powheg), once we find an emission below the veto scale we know that
     *  all subsequent emissions will also be below the scale.  Therefore m_doneISR
     *  gets set to true at that point and nothing more needs doing
     *
     */
    bool doVetoISREmission(int, const Event &e, int){
      // Note ignoring new iSys param for now (addition required to fix compilation against >= 8.157

      if(m_doneISR) return false;

      int i = e.size() - 1;

      while(i != -1){

        if(e[i].isFinal() && e[i].status() == 43) break;

        --i;

        if(i == 0){
          e.list();
          throw std::runtime_error("Pythia8::VetoedShower::doVetoISREmission: Could not find ISR emission");
        }
      }

      //check the leg is connected to the hard system
      int iMother = e[i].mother1();
      if(e[iMother].status() != -41)
        throw std::runtime_error("Pythia8::VetoedShower::doVetoISREmission: Unexpected status code in ISR");

      iMother = e[iMother].daughter2();
      if(iMother != partonSystemsPtr->getInA(0) &&
         iMother != partonSystemsPtr->getInB(0)) return false;

      // Veto emission above the veto scale
      if(e[i].pT() > m_pTVeto) return true;

      m_doneISR = true;
      return false;
    }
    /**
     *  This is similar to the ISR veto, with the exception that since the ordering is in
     *  a slightly different pT definition to Powheg the veto must always be checked, rather
     *  than stopping once an emission is allowed
     *
     */
    bool doVetoFSREmission(int, const Event &e, int, bool){
      // Note ignoring new iSys and isResonance params for now (addition required to fix compilation against >= 8.157

      int i = e.size() - 1;

      while(i != -1){
        if (e[i].isFinal() && e[i].status() == 51) break;

        --i;

        if(i == 0){
          e.list();
          throw std::runtime_error("Pythia8::VetoedShower::doVetoFSREmission: Could not find FSR emission");
        }
      }

      // Make sure radiation from the hard system
      int  iMother = e[i].mother1();
      int  sysSize = partonSystemsPtr->sizeOut(0);
      bool hardSys = false;

      for (int j = 0; j < sysSize; ++j){
        int iOut = partonSystemsPtr->getOut(0, j);
        if (iOut == iMother) {
          hardSys = true;
          break;
        }
      }

      if (!hardSys) return false;

      // Veto if above the POWHEG scale
      if (e[i].pT() > m_pTVeto) return true;

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

    double m_pTVeto;
    bool m_doneISR;
  };
}

