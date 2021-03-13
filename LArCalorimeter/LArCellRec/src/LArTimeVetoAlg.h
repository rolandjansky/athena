/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARTIMEVETOALG_H
#define LARCELLREC_LARTIMEVETOALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include <atomic>

class AthenaAttributeList;

class LArTimeVetoAlg : public AthReentrantAlgorithm {
  public:
  //Delegate constructor:
  using AthReentrantAlgorithm::AthReentrantAlgorithm; 
    
    virtual StatusCode          initialize() override;
    virtual StatusCode          execute (const EventContext& ctx) const override;
    virtual StatusCode          finalize() override;

  private:
    mutable std::atomic<unsigned> m_nevt{0};
    mutable std::atomic<unsigned> m_nevtMasked{0};

    SG::ReadCondHandleKey<AthenaAttributeList> m_eventVetoKey{this, "folderName", "/LAR/BadChannelsOfl/EventVeto", "Folder name for DB access"};
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "eventInfoKey", "EventInfo", "Key for EventInfo object"};
};
#endif
