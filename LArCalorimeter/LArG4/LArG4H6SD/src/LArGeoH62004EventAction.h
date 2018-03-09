/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LArGeoH62004EventAction_h
#define LARG4H6SD_LArGeoH62004EventAction_h 1

#include "G4UserEventAction.hh"
// For the output write handle
#include "StoreGate/WriteHandle.h"
#include "TBEvent/TBEventInfo.h"


class LArGeoTB2004Options;

namespace G4UA
{
  /// @brief NEEDS DOCUMENTATION
  class LArGeoH62004EventAction final: public G4UserEventAction
  {
  public:
    struct Config
    {
      float yTable=0.0;
      float cryoXposition=0.0;
    };

    LArGeoH62004EventAction(const Config& config);

    virtual void EndOfEventAction(const G4Event*) override;

  private:
    static int m_evnum;

    SG::WriteHandle<TBEventInfo> m_ev;
    float m_xpos;
    float m_ypos;
  }; // class LArGeoH62004EventAction

} // namespace G4UA

#endif