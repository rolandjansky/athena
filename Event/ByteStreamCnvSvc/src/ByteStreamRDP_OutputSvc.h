/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMRDP_OUTPUTSVC_H
#define BYTESTREAMRDP_OUTPUTSVC_H

/**
  @class      ByteStreamRDP_OutputSvc
  @brief      Writing the data to ROBDataProviderSvc for transient BS

              RDP == RobDataProvider
 Description: An class for writing FullEventFragment into RobDataProvider
              this class implements the interface ByteStreamOutputSvc
              for the conversion service to write the output. 
*/

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "GaudiKernel/ServiceHandle.h"

class IROBDataProviderSvc;

class ByteStreamRDP_OutputSvc: public ByteStreamOutputSvc {
public:
   /// Constructors:
   ByteStreamRDP_OutputSvc(const std::string& name, ISvcLocator* svcloc);

   /// Required of all Gaudi Services
   virtual StatusCode initialize() override;

   /// Implementation of the ByteStreamOutputSvc interface methods.
   virtual bool putEvent(RawEvent* re) override;
   virtual bool putEvent(RawEvent* re, const EventContext& ctx) override;

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

private: // data
   struct EventCache {
      void releaseEvent() {
         this->rawEvent.reset();
         this->dataBuffer.reset();
      }
      std::unique_ptr<RawEvent> rawEvent {nullptr}; //!< Current event fragment
      std::unique_ptr<uint32_t[]> dataBuffer {nullptr}; //!< Underlying data structure
   };
   SG::SlotSpecificObj<EventCache> m_eventsCache; //!< Cache of event data for each slot

   ServiceHandle<IROBDataProviderSvc> m_robProvider {
      this, "ROBDataProviderSvc", "ROBDataProviderSvc", "ROB data provider"};
   Gaudi::Property<std::string> m_bsOutputStreamName {
      this, "BSOutputStreamName", "", "Stream name for multiple output"};
};

#endif  
