/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=====================================
// EMEC/HEC/FCAL TB stepping action, 
// do not stopping the particle in leakage det. !!!!
//=====================================
#ifndef LArGeoH62004SteppingAction_h
#define LArGeoH62004SteppingAction_h 1

#include "FadsActions/UserAction.h"
#include <string>
#include <vector>


class LArGeoH62004SteppingAction : public FADS::UserAction
{
  public:
    LArGeoH62004SteppingAction(std::string s): UserAction(s) {};
    virtual ~LArGeoH62004SteppingAction(){};
    void ClearVector(){ m_tracks.clear(); }

  public:
    void SteppingAction(const G4Step* theStep);

  private:
    std::vector<int> m_tracks;  
};

#endif

