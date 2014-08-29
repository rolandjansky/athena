/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SteppingValidation_H
#define SteppingValidation_H

#include "FadsActions/UserAction.h"
#include "SimTestToolBase.h"

#include <string>
#include <vector>

class TH1;
class TH2;
class G4Track;

// User action to do some basic step-based validation of G4

class SteppingValidation: public FADS::UserAction, public SimTestToolBase {

  public:
   // Constructor
   SteppingValidation(const std::string& type, const std::string& name, const IInterface* parent)
     : FADS::UserAction(name)
     , SimTestToolBase(type,name,parent)
     , m_stepL(0),m_stepProc(0),m_mscAngle(0),m_stepELoss(0),m_secE(0)
     , m_latPhi(0),m_latEta(0),m_EvsR(0)
     , m_prim(0),m_sec(0)
     , m_primH(0),m_primF(0),m_dh(0),m_dh2(0),m_dp(0),m_dp2(0),m_nsec(0)
    {}

   // User Action functions
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

   // Sim Test Tool functions
   StatusCode initialize();
   StatusCode processEvent();

  private:
   TH1 *m_stepL, *m_stepProc, *m_mscAngle, *m_stepELoss, *m_secE, *m_latPhi, *m_latEta;
   TH2 *m_EvsR;
   G4Track *m_prim, *m_sec;
   double m_primH,m_primF,m_dh,m_dh2,m_dp,m_dp2,m_nsec;
};


#endif
