#include "Pythia8_i/CompoundUserHook.h"
#include "Pythia8_i/UserHooksFactory.h"
#include <string>

namespace Pythia8{
  
  //////////////////////////////////////////////////////////////////////
  CompoundUserHook::~CompoundUserHook(){
  
    for(UserHooks *h: m_subhooks){
      delete h;
    }
    
  }
  //////////////////////////////////////////////////////////////////////
  void CompoundUserHook::addSubHook(UserHooks *hook){
    m_subhooks.push_back(hook);
    return;
  }
  
  void CompoundUserHook::addSubHooks(const std::vector<std::string> &hookNames){
    
    for(const auto &hn: hookNames){
      addSubHook(Pythia8_UserHooks::UserHooksFactory::create(hn));
    }
    return;
  }
  
  //////////////////////////////////////////////////////////////////////
  void CompoundUserHook::initialise(){
    
    if(m_initialised) return;
    m_initialised = true;
    
    for(auto &hook: m_subhooks){
      hook->initPtr(infoPtr, settingsPtr, particleDataPtr, rndmPtr, beamAPtr, beamBPtr, beamPomAPtr, beamPomBPtr, coupSMPtr, partonSystemsPtr, sigmaTotPtr);
    }
    return;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::initAfterBeams(){
    
    // The UserHooks base class initialisation is not virtual, therefore
    // we cannot modify it to call through to the individual sub-hooks.
    // This means the sub-hooks *have* to be capable of being initialised
    // after the beams, because that is the only time we can do it
    for(auto &hook: m_subhooks){
      if(!hook->initAfterBeams()) throw std::runtime_error("Multiple ATLAS UserHooks must be capable of being initialised after the beams.");
    }
    
    return true;
  }

  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canModifySigma(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canModifySigma();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  double CompoundUserHook::multiplySigmaBy(const SigmaProcess *sigmaProcessPtr,
                                           const PhaseSpace* phaseSpacePtr,
                                           bool inEvent){
    
    double result = 1.;
    
    for(auto &hook: m_subhooks){
      if(hook->canModifySigma()) result *= hook->multiplySigmaBy(sigmaProcessPtr, phaseSpacePtr, inEvent);
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canBiasSelection(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canBiasSelection();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  double CompoundUserHook::biasSelectionBy(const SigmaProcess *sigmaProcessPtr,
                         const PhaseSpace* phaseSpacePtr,
                                           bool inEvent){
    initialise();
    selBias = 1.;
    
    for(auto &hook: m_subhooks){
      if(hook->canModifySigma()) selBias *= hook->biasSelectionBy(sigmaProcessPtr, phaseSpacePtr, inEvent);
    }
    return selBias;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoProcessLevel(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoProcessLevel();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoProcessLevel(Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoProcessLevel()) result = result || hook->doVetoProcessLevel(event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoResonanceDecays(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoResonanceDecays();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoResonanceDecays(Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoResonanceDecays()) result = result || hook->doVetoResonanceDecays(event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoPT(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoPT();
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoPT(int ipos, const Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoPT()) result = result || hook->doVetoPT(ipos, event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoStep(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoStep();
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  int CompoundUserHook::numberVetoStep(){
    int result = -1;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoStep()){
        if(result < 0){
          result = hook->numberVetoStep();
        }else if(result != hook->numberVetoStep()){
          throw std::runtime_error("The result of numberVetoStep, if active, must be the same in all sub hooks.  Found two hooks with numberVetoStep = " + std::to_string(result) + " and " +
                                   std::to_string(hook->numberVetoStep()));
        }
      }
    }
   
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoStep(int ipos, int nISR, int nFSR, const Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoStep()) result = result || hook->doVetoStep(ipos, nISR, nFSR, event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoMPIStep(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoMPIStep();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  int CompoundUserHook::numberVetoMPIStep(){
    int result = -1;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoMPIStep()){
        if(result < 0){
          result = hook->numberVetoMPIStep();
        }else if(result != hook->numberVetoMPIStep()){
          throw std::runtime_error("The result of numberVetoMPIStep, if active, must be the same in all sub hooks.  Found two hooks with numberVetoMPIStep = " + std::to_string(result) + " and " +
                                   std::to_string(hook->numberVetoMPIStep()));
        }
      }
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoMPIStep(int nMPI, const Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoMPIStep()) result = result || hook->doVetoMPIStep(nMPI, event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoPartonLevelEarly(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoPartonLevelEarly();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoPartonLevelEarly(const Event &event){
    
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoPartonLevelEarly()) result = result || hook->doVetoPartonLevelEarly(event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::retryPartonLevel(){
    bool result = false;
    bool start = true;
    
    for(auto &hook: m_subhooks){
      
      if(!(hook->canVetoPT() || hook->canVetoStep() ||
           hook->canVetoMPIStep() || hook->canVetoPartonLevelEarly()) ) continue;
      
      if(start){
        result = hook->retryPartonLevel();
        start = false;
      }else if(result != hook->retryPartonLevel()){
        throw std::runtime_error("retryPartonLevel must return the same value in all sub-hooks used");
      }
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoPartonLevel(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoPartonLevel();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoPartonLevel( const Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoPartonLevel()) result = result || hook->doVetoPartonLevel(event);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canSetResonanceScale(){
    
    initialise();
    bool result = false;
    bool set = false;
    
    for(auto &hook: m_subhooks){
      
      if(hook->canSetResonanceScale()){
        if(set) throw std::runtime_error("The ResonanceScale is set by more than one user hook.  don't know how to proceed!");
        
        result = canSetResonanceScale();
        set = true;
      }
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  double CompoundUserHook::scaleResonance(int iRes, const Event &event){
    double result = 0.;
    
    for(auto &hook: m_subhooks){
      if(hook->canSetResonanceScale()) result = hook->scaleResonance(iRes, event);
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoISREmission(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoISREmission();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoISREmission(int sizeOld, const Event &event, int iSys){
    
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoISREmission()) result = result || hook->doVetoISREmission(sizeOld, event, iSys);
    }
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoFSREmission(){
    
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoFSREmission();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoFSREmission(int sizeOld, const Event &event,
                                           int iSys, bool inResonance){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoFSREmission()) result = result || hook->doVetoFSREmission(sizeOld, event, iSys, inResonance);
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::canVetoMPIEmission(){
    initialise();
    bool result = false;
    
    for(auto &hook: m_subhooks){
      result = result || hook->canVetoMPIEmission();
    }
    
    return result;
  }
  
  //////////////////////////////////////////////////////////////////////
  bool CompoundUserHook::doVetoMPIEmission(int sizeOld, const Event &event){
    bool result = false;
    
    for(auto &hook: m_subhooks){
      if(hook->canVetoMPIEmission()) result = result || hook->doVetoMPIEmission(sizeOld, event);
    }
    
    return result;
  }
  
}
