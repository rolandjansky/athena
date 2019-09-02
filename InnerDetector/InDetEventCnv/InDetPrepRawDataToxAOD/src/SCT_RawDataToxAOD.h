// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATATOXAOD_H
#define SCT_RAWDATATOXAOD_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// SCT ntuple container type
#include "InDetRawData/SCT_RDO_Container.h"
// StoreGate Data Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
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

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

private:
  const SCT_ID* m_SCTHelper{nullptr};

  SG::ReadHandleKey<SCT_RDO_Container> m_rdoContainerName{this, "SctRdoContainer", "SCT_RDOs"};
  SG::WriteHandleKey<xAOD::SCTRawHitValidationContainer> m_xAodRawHitContainerName{this, "SctxAodRawHitContainer", "SCT_RawHits"};
};

#endif // SCT_RAWDATATOXAOD_H
