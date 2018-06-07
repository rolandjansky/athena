#ifndef GENERATOR_PYTHIA8_COMPOUND_USERHOOK_H
#define GENERATOR_PYTHIA8_COMPOUND_USERHOOK_H

#include "Pythia8/UserHooks.h"
#include <vector>

namespace Pythia8{
  
  using std::vector;
  
  class CompoundUserHook : public UserHooks{
    
  public:
    
    CompoundUserHook(): UserHooks(), m_initialised(false){}
    
    ~CompoundUserHook();
    
    void addSubHook(UserHooks *hook);
    
    void addSubHooks(const std::vector<std::string> &hookNames);
    
    void initialise();
    
    // The UserHooks base class initialisation is not virtual, therefore
    // we cannot modify it to call through to the individual sub-hooks.
    // This means the sub-hooks *have* to be capable of being initialised
    // after the beams, because that is the only time we can do it
    bool initAfterBeams();
    
    bool canModifySigma();
    
    double multiplySigmaBy(const SigmaProcess *sigmaProcessPtr,
                           const PhaseSpace *phaseSpacePtr,
                           bool inEvent);
    
    bool canBiasSelection();
    
    double biasSelectionBy(const SigmaProcess *sigmaProcessPtr,
                           const PhaseSpace* phaseSpacePtr,
                           bool inEvent);
    
    bool canVetoProcessLevel();
    
    bool doVetoProcessLevel(Event &event);
    
    bool canVetoResonanceDecays();
    
    bool doVetoResonanceDecays(Event &event);
    
    bool canVetoPT();
    
    bool doVetoPT(int ipos, const Event &event);
    
    bool canVetoStep();
    
    // The number of veto steps must be the same in each active sub hook,
    // otherwise we won't know what to do.
    int numberVetoStep();
    
    bool doVetoStep(int ipos, int nISR, int nFSR, const Event &event);
    
    bool canVetoMPIStep();
    
    // The number of veto steps must be the same in each active sub hook,
    // otherwise we won't know what to do.
    int numberVetoMPIStep();
    
    bool doVetoMPIStep(int nMPI, const Event &event );
    
    bool canVetoPartonLevelEarly();
    
    bool doVetoPartonLevelEarly( const Event &event);
    
    bool retryPartonLevel();
    
    bool canVetoPartonLevel();
    
    bool doVetoPartonLevel( const Event &event);
    
    // Only one of the sub hooks can set the scale.  If two set it, throws an exception
    bool canSetResonanceScale();
    
    double scaleResonance(int iRes, const Event &event);
    
    bool canVetoISREmission();
    
    bool doVetoISREmission(int sizeOld, const Event &event, int iSys);
    
    bool canVetoFSREmission();
    
    bool doVetoFSREmission(int sizeOld, const Event &event, int iSys, bool inResonance = false );
    
    bool canVetoMPIEmission();
    
    bool doVetoMPIEmission(int sizeOld, const Event &event);
    
  private:
    
    bool m_initialised;
    vector<UserHooks *> m_subhooks;
    
  };
  
}

#endif
