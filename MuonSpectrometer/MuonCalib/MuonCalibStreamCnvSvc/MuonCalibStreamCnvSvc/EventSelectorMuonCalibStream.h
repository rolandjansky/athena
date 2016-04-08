/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVC_EVENTSELECTORMUONCALIBSTREAM_H
#define MUONCALIBSTREAMCNVSVC_EVENTSELECTORMUONCALIBSTREAM_H
//  EventSelectorMuonCalibStream


// Include files.
#include <map>
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IProperty.h"

#include "MuCalDecode/CalibEvent.h" 

// Forward declarations.
class ISvcLocator;
class EventContextMuonCalibStream;
class MuonCalibStreamInputSvc;
class IMuonCalibStreamDataProviderSvc;

// Namespaces.
using namespace std;

// Class EventSelectorMuonCalibStream.
class EventSelectorMuonCalibStream : virtual public AthService, 
                           virtual public IEvtSelector, 
                           virtual public IProperty {

 public:
  // Standard Constructor.
  EventSelectorMuonCalibStream(const string& name, ISvcLocator* svcloc);
  
  // Standard Destructor.
  ~EventSelectorMuonCalibStream();

  // Implementation of Service base class methods.
  virtual StatusCode initialize();

  // Implementation of the IEvtSelector interface methods.
  virtual StatusCode createContext(Context*& it) const;
  virtual StatusCode next(Context& it) const;
  virtual StatusCode next(Context& it, int jump) const;
  virtual StatusCode previous(Context& it) const;
  virtual StatusCode previous(Context& it, int jump) const;

  virtual StatusCode last(Context& it) const;
  virtual StatusCode rewind(Context& it) const;

  virtual StatusCode createAddress(const Context& it,IOpaqueAddress*& iop) const;
  virtual StatusCode releaseContext(Context*& it) const;
  virtual StatusCode resetCriteria(const std::string& criteria, Context& context) const;

  // Implementation of IInterface methods.
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  // make entries in StoreGate for this event  

 private:
  // property 
  string                   m_eventSourceName;

   // 
  EventContextMuonCalibStream*    m_beginIter;
  EventContextMuonCalibStream*    m_endIter;
  MuonCalibStreamInputSvc*        m_eventSource;
  IMuonCalibStreamDataProviderSvc* m_dataProvider; 
  
  int m_SkipEvents;            // Number of events to skip at the beginning 
  mutable int m_NumEvents;     // Number of Events read so far.

};

#endif  
