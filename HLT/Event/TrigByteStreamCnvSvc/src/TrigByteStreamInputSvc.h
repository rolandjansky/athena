/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBYTESTREAMINPUTSVC_H
#define TRIGBYTESTREAMINPUTSVC_H

#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <memory.h>

// Forward declarations
class StoreGateSvc;

/** @class TrigByteStreamInputSvc
 *  @brief A ByteStreamInputSvc implementation for online use, reading events from hltinterface::DataCollector
 *
 *  The layout and implementation are based on ByteStreamEventStorageInputSvc
 **/
class TrigByteStreamInputSvc : public ByteStreamInputSvc {
public:
  /// Standard constructor
  TrigByteStreamInputSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~TrigByteStreamInputSvc();

  // -------------------------- IInterface methods -----------------------------
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  // ------------------------- Service methods --------------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- ByteStreamInputSvc methods ----------------------
  virtual const RawEvent* nextEvent() override;
  virtual const RawEvent* previousEvent() override;
  virtual const RawEvent* currentEvent() const override;

private:
  // ------------------------- Service handles ---------------------------------
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc {this, "ROBDataProvider", "ROBDataProviderSvc"};
  ServiceHandle<StoreGateSvc> m_evtStore {this, "EventStore", "StoreGateSvc"};
  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "" , "Monitoring tool"};


  // ------------------------- Private data members ----------------------------
  struct EventCache {
    ~EventCache() = default;
    void releaseEvent();
    std::unique_ptr<RawEvent> fullEventFragment {nullptr}; //!< Current event fragment
    std::unique_ptr<uint32_t[]> rawData {nullptr}; //!< Underlying data structure
  };

  SG::SlotSpecificObj<EventCache> m_eventsCache; //!< Cache of RawEvent pointer per event slot
  uint16_t m_maxLB{0}; //!< Maximum lumi block number seen so far, used for monitoring
};

#endif // TRIGBYTESTREAMINPUTSVC_H
