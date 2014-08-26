/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArGeoH62004EventAction_h
#define LArGeoH62004EventAction_h 1

#include "FadsActions/UserAction.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

class LArGeoTB2004Options;

class LArGeoH62004EventAction : public FADS::UserAction
{
  public:
    LArGeoH62004EventAction(std::string s);
    virtual ~LArGeoH62004EventAction(){};

    void EndOfEventAction(const G4Event* theEvent);

  private:
    static int m_evnum;
    const LArGeoTB2004Options *m_largeoTB2004Options;
    IMessageSvc* m_msgSvc; 
    StoreGateSvc*   m_storeGate;
    StoreGateSvc*   m_detectorStore;
};

#endif

