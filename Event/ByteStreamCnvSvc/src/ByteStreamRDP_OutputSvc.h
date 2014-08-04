/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ServiceHandle.h"

class IROBDataProviderSvc;

class ByteStreamRDP_OutputSvc: public ByteStreamOutputSvc {
public:
   /// Constructors:
   ByteStreamRDP_OutputSvc(const std::string& name, ISvcLocator* svcloc);
   /// Destructor.
   virtual ~ByteStreamRDP_OutputSvc();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   /// Implementation of the ByteStreamOutputSvc interface methods.
   virtual bool putEvent(RawEvent* re);

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

private: // data
   int                m_totalEventCounter; //!< number of event counter
   RawEvent* m_re;
   OFFLINE_FRAGMENTS_NAMESPACE::DataType* m_buf;
   ServiceHandle<IROBDataProviderSvc> m_robProvider; 

private: // properties
   StringProperty m_bsOutputStreamName; //!< stream name for multiple output 
};

#endif  
