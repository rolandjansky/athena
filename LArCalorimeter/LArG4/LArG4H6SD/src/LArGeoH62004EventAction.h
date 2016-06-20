/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArGeoH62004EventAction_h
#define LArGeoH62004EventAction_h 1

#include "G4AtlasTools/UserActionBase.h"

// For the output write handle
#include "StoreGate/WriteHandle.h"
#include "TBEvent/TBEventInfo.h"


class LArGeoTB2004Options;

class LArGeoH62004EventAction final: public UserActionBase
{
  public:
    LArGeoH62004EventAction(const std::string& type, const std::string& name, const IInterface* parent);

    virtual void EndOfEvent(const G4Event* theEvent) override;
    virtual StatusCode queryInterface(const InterfaceID&, void**);
    virtual StatusCode initialize() override;

  private:
    static int m_evnum;
    const LArGeoTB2004Options *m_largeoTB2004Options;

    SG::WriteHandle<TBEventInfo> m_ev;
};

#endif
