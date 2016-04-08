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
  IntegerProperty          m_runNo;
  UnsignedLongLongProperty m_firstEventNo;
  UnsignedLongLongProperty m_eventsPerRun;
  IntegerProperty          m_firstLBNo;
  IntegerProperty          m_eventsPerLB;
  IntegerProperty          m_initTimeStamp;
  IntegerProperty          m_timeStampInterval;

  /// Flags to indicate override of run/event/time 
  ///  These are always true and are here for consistency
  ///  with other event selectors which only optionally 
  ///  override these numbers.
  BooleanProperty m_overrideRunNumber;
  BooleanProperty m_overrideEventNumber;
  BooleanProperty m_overrideLBNumber;
  BooleanProperty m_overrideTimeStamp;

  mutable McContext *m_ctx;

};

#endif  // MCEVENTCNV_MCEVENTSELECTOR_H




