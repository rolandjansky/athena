/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
/// This algorithm flags an event bad if it has 
/// >500 LVL1ID errors or >1000 ROBFragment errors
///////////////////////////////////////////////////////////////////

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTEVENTFLAGWRITER_H
#define SCT_RAWDATABYTESTREAMCNV_SCTEVENTFLAGWRITER_H

/** Base class */
#include "AthenaBaseComps/AthAlgorithm.h"

/** Gaudi */
#include "GaudiKernel/ToolHandle.h"

/** other */
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"

class SCTEventFlagWriter : public AthAlgorithm
{
 public:

  //! Constructor.
  SCTEventFlagWriter(const std::string &name, ISvcLocator *pSvcLocator);

  //! Destructur
  virtual ~SCTEventFlagWriter() = default;

  //! Initialize
  virtual StatusCode initialize() override;

  //! Execute
  virtual StatusCode execute() override;

  //! Finalize empty, no need to override

 private:

  ToolHandle<ISCT_ByteStreamErrorsTool> m_bsErrTool{this, "ErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};

  SG::ReadHandleKey<xAOD::EventInfo> m_xevtInfoKey;
  SG::ReadHandleKey<EventInfo> m_evtInfoKey;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTEVENTFLAGWRITER_H
