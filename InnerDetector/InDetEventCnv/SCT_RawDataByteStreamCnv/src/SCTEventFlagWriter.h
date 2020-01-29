// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTEVENTFLAGWRITER_H
#define SCT_RAWDATABYTESTREAMCNV_SCTEVENTFLAGWRITER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"

class ISCT_ByteStreamErrorsTool;

/** 
 * @class SCTEventFlagWriter
 *
 * @brief Athena reentrant Algorithm to flag an event bad if the number of errors exceed threshold. 
 *
 * This algorithm flags an event bad if it has >500 LVL1ID errors or 
 * >1000 ROBFragment errors.
 */
class SCTEventFlagWriter : public AthReentrantAlgorithm
{
 public:

  /** Constructor */
  SCTEventFlagWriter(const std::string &name, ISvcLocator *pSvcLocator);

  /** Destructur */
  virtual ~SCTEventFlagWriter() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /** Execute */
  virtual StatusCode execute(const EventContext& ctx) const override;

  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:

  /** Tool that keeps track of modules that give rise to errors in the bytestream. */
  ToolHandle<ISCT_ByteStreamErrorsTool> m_bsErrTool{this, "ErrorsTool", "SCT_ByteStreamErrorsTool", 
                                                    "Tool to retrieve SCT ByteStream Errors"};

  /** Read handle for the xAOD event info. */
  SG::ReadHandleKey<xAOD::EventInfo> m_xAODEvtInfoKey{this, "xAODEventInfoKey", "EventInfo", 
                                                      "xAOD event info key"};
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTEVENTFLAGWRITER_H
