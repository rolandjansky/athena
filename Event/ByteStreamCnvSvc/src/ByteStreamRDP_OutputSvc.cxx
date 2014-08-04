/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of ByteStreamRDP_OutputSvc
//  Initial implementation Jan 10, 2003
//             
//===================================================================
//

// Include files.
#include "ByteStreamRDP_OutputSvc.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

// Constructor.
ByteStreamRDP_OutputSvc::ByteStreamRDP_OutputSvc(const std::string& name, ISvcLocator* svcloc)
	: ByteStreamOutputSvc(name, svcloc),
	m_totalEventCounter(0), m_re(0), m_buf(0), m_robProvider("ROBDataProviderSvc", name) {
   declareProperty("BSOutputStreamName", m_bsOutputStreamName = name);
}

// Destructor.
ByteStreamRDP_OutputSvc::~ByteStreamRDP_OutputSvc() {
   delete [] m_buf;
   delete m_re; m_re = 0;
}

// Open the first input file and read the first event.
StatusCode ByteStreamRDP_OutputSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!ByteStreamOutputSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize ByteStreamOutputSvc base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve ROBDataProviderSvc
   if (!m_robProvider.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ROBDataProviderSvc");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

// Receive the next event.
bool  ByteStreamRDP_OutputSvc::putEvent(RawEvent* re) {
  delete [] m_buf;
  delete m_re; m_re = 0;
  // Keep a local copy
  uint32_t reSize = re->fragment_size_word();
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType reStart;
  re->start(reStart);
  m_buf = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[reSize];
  memcpy (reinterpret_cast<void *>(m_buf), reinterpret_cast<const void *>(reStart), reSize*sizeof(reStart[0]));
  m_re = new RawEvent(m_buf);
  // Give RawEvent to RDP 
  m_robProvider->setNextEvent(m_re); 
  // Event Count 
  ++m_totalEventCounter; 
  ATH_MSG_DEBUG("Number of Events in ByteStreamRDP_OutputSvc: " << m_totalEventCounter); 
  return(true); 
}

StatusCode ByteStreamRDP_OutputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (ByteStreamOutputSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<ByteStreamOutputSvc*>(this);
   } else {
     // Interface is not directly available: try out a base class
     return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
