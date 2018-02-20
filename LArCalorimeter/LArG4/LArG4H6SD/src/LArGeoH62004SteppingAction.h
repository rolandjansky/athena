/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=====================================
// EMEC/HEC/FCAL TB stepping action,
// do not stopping the particle in leakage det. !!!!
//=====================================
#ifndef LARG4H6SD_LArGeoH62004SteppingAction_h
#define LARG4H6SD_LArGeoH62004SteppingAction_h 1


#include "G4UserSteppingAction.hh"

namespace G4UA
{
  /// @brief NEEDS DOCUMENTATION
  class LArGeoH62004SteppingAction final: public G4UserSteppingAction
  {
  public:

    struct Config
    {
      float yTable=0.0;
      bool checkprim=false;
      bool printstep=false;
    };

    LArGeoH62004SteppingAction(const Config& config);
    virtual void UserSteppingAction(const G4Step*) override;

  private:
    float m_yTable;
    bool m_checkprim;
    bool m_printstep;
  }; // class LArGeoH62004SteppingAction

} // namespace G4UA

#endif