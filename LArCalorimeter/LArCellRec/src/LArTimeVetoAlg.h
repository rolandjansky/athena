/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARTIMEVETOALG_H
#define LARCELLREC_LARTIMEVETOALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"


class AthenaAttributeList;

class LArTimeVetoAlg : public AthReentrantAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    LArTimeVetoAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~LArTimeVetoAlg();
    
    virtual StatusCode          initialize() override;
    virtual StatusCode          execute_r (const EventContext& ctx) const override;
    virtual StatusCode          finalize() override;

  private:
  // remove for now, will add statistics by MT compliant way later  
  // int m_nevt;
  // int m_nevtMasked;
  SG::ReadCondHandleKey<AthenaAttributeList> m_eventVetoKey{this, "folderName", "/LAR/BadChannelsOfl/EventVeto", "Folder name for DB access"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "eventInfoKey", "EventInfo", "Key for EventInfo object"};
};
#endif
