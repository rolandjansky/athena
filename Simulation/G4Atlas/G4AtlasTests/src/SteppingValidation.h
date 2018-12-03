/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SteppingValidation_H
#define SteppingValidation_H

#include "SimTestToolBase.h"

#include <string>
#include <vector>

class TH1;
class TH2;
class G4Track;

// User action to do some basic step-based validation of G4


#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"
namespace G4UA{
  
  
  class SteppingValidation:
  public G4UserRunAction, public G4UserEventAction, public G4UserSteppingAction, public SimTestHisto
  {

  public:
    // ctor
  SteppingValidation(): SimTestHisto(),
      m_stepL(0),m_stepProc(0),m_mscAngle(0),m_stepELoss(0),m_secE(0),
      m_latPhi(0),m_latEta(0),m_EvsR(0),
      m_prim(0),m_sec(0),
      m_primH(0),m_primF(0),m_dh(0),m_dh2(0),m_dp(0),m_dp2(0),m_nsec(0)
      {};
    
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:
    TH1 *m_stepL, *m_stepProc, *m_mscAngle, *m_stepELoss, *m_secE, *m_latPhi, *m_latEta;
    TH2 *m_EvsR;
    G4Track *m_prim, *m_sec;
    double m_primH,m_primF,m_dh,m_dh2,m_dp,m_dp2,m_nsec;
  }; // class SteppingValidation
  
  
} // namespace G4UA 



#endif
