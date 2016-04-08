/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=====================================
// EMEC/HEC/FCAL TB stepping action,
// do not stopping the particle in leakage det. !!!!
//=====================================
#ifndef LArGeoH62004SteppingAction_h
#define LArGeoH62004SteppingAction_h 1

#include "G4AtlasTools/UserActionBase.h"

#include <string>
#include <vector>

class LArGeoTB2004Options;

class LArGeoH62004SteppingAction final: public UserActionBase
{
  public:
    LArGeoH62004SteppingAction(const std::string& type, const std::string& name, const IInterface* parent);
    void ClearVector(){ m_tracks.clear(); }
    ~LArGeoH62004SteppingAction();

  public:
    virtual void Step(const G4Step* theStep) override;
    virtual StatusCode queryInterface(const InterfaceID&, void**);
    virtual StatusCode initialize() override;

  private:
    std::vector<int> m_tracks;
    const LArGeoTB2004Options *m_largeoTB2004Options;
    bool m_ownOptions;
    float m_yTable;
};

#endif
