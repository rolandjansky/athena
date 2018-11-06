/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBYTESTREAMINPUTSVC_H
#define TRIGBYTESTREAMINPUTSVC_H

#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthenaKernel/SlotSpecificObj.h"

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
  // ------------------------- service handles ---- ----------------------------
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
  ServiceHandle<StoreGateSvc> m_evtStore;

  // ------------------------- private data members ----------------------------
  struct EventCache {
    ~EventCache();
    RawEvent* rawEvent = 0; //!< current event
    // unsigned int eventStatus = 0; //!< status of the current event
  };

  SG::SlotSpecificObj<EventCache> m_eventsCache;

  // ------------------------- private helper methods --------------------------
  /// Clean up parts of previous event and re-init them
  void releaseEvent(EventCache* cache);
};

#endif // TRIGBYTESTREAMINPUTSVC_H
