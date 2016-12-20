/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	McEventSelector.h
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//  Description: The McEventSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria".
//
//	Author     : P. Calafiura
//  Created    : 2/28/00
//	Changes    : 
//
//
//====================================================================

#ifndef MCEVENTSELECTOR_MCEVENTSELECTOR_H
#define MCEVENTSELECTOR_MCEVENTSELECTOR_H 1

// Include files
#include "GaudiKernel/IEvtSelector.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/IEventSeek.h"

// Forward declarations
class EventSource;
class ISvcLocator;
class McContext;

#ifdef WIN32
// Disable warning C4786: identifier was truncated to '255' characters in the debug information
#pragma warning ( disable : 4786 )
#endif


//--------------------------------------------------------------------
// Event Selector 
//--------------------------------------------------------------------

class McEventSelector : virtual public AthService, 
                        virtual public IEvtSelector,
                        virtual public IEventSeek {
public:

  virtual StatusCode initialize();
  virtual StatusCode stop();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, 
				    void** ppvInterface);
  virtual StatusCode createContext(Context*& refpCtxt) const;

  virtual StatusCode last(Context& refContext) const;
  virtual StatusCode next(Context& refCtxt) const;
  virtual StatusCode next(Context& refCtxt,int jump) const;
  virtual StatusCode previous(Context& refCtxt) const;
  virtual StatusCode previous(Context& refCtxt,int jump) const;
  virtual StatusCode rewind(Context& refCtxt) const;

  virtual StatusCode createAddress(const Context& refCtxt, 
				   IOpaqueAddress*&) const;
  virtual StatusCode releaseContext(Context*& refCtxt) const;
  virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;

  virtual StatusCode seek(int);
  virtual int curEvent() const;

  /// Service Constructor
  McEventSelector( const std::string& name, ISvcLocator* svcloc );

  ~McEventSelector();

private:
  //  EventSource* p_eventSource;
  Gaudi::CheckedProperty<int>          m_runNo;
  Gaudi::CheckedProperty<unsigned long long> m_firstEventNo;
  Gaudi::CheckedProperty<unsigned long long> m_eventsPerRun;
  Gaudi::CheckedProperty<int>          m_firstLBNo;
  Gaudi::CheckedProperty<int>          m_eventsPerLB;
  Gaudi::CheckedProperty<int>          m_initTimeStamp;
  Gaudi::Property<int>          m_timeStampInterval;

  /// Flags to indicate override of run/event/time 
  ///  These are always true and are here for consistency
  ///  with other event selectors which only optionally 
  ///  override these numbers.
  Gaudi::Property<bool> m_overrideRunNumber;
  Gaudi::Property<bool> m_overrideEventNumber;
  Gaudi::Property<bool> m_overrideLBNumber;
  Gaudi::Property<bool> m_overrideTimeStamp;

  mutable McContext *m_ctx;

};

#endif  // MCEVENTCNV_MCEVENTSELECTOR_H




