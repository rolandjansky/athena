// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATATOXAOD_H
#define SCT_RAWDATATOXAOD_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// StoreGate Data Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
// SCT ntuple container type
#include "InDetRawData/SCT_RDO_Container.h"
// xAOD container type
#include "xAODTracking/SCTRawHitValidationContainer.h"

#include <string>

class ISvcLocator;
class SCT_ID;

/** Algorithm to read RDO information from SCT ntuple and write augmented xAOD.
 **/
class SCT_RawDataToxAOD : public AthReentrantAlgorithm {
public:
  SCT_RawDataToxAOD(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const;
  StatusCode finalize();

private:
  const SCT_ID* m_SCTHelper;

  SG::ReadHandleKey<SCT_RDO_Container> m_rdoContainerName{this, "SctRdoContainer", "SCT_RDOs"};
  SG::WriteHandleKey<xAOD::SCTRawHitValidationContainer> m_xAodRawHitContainerName{this, "SctxAodRawHitContainer", "SCT_RawHits"};
};

#endif // SCT_RAWDATATOXAOD_H
