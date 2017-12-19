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

#include "AthenaKernel/IEvtSelectorSeek.h"

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
                        virtual public IEvtSelectorSeek {
public:

  virtual StatusCode initialize() override;
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface(const InterfaceID& riid, 
				    void** ppvInterface) override;
  virtual StatusCode createContext(Context*& refpCtxt) const override;

  virtual StatusCode last(Context& refContext) const override;
  virtual StatusCode next(Context& refCtxt) const override;
  virtual StatusCode next(Context& refCtxt,int jump) const override;
  virtual StatusCode previous(Context& refCtxt) const override;
  virtual StatusCode previous(Context& refCtxt,int jump) const override;
  virtual StatusCode rewind(Context& refCtxt) const override;

  virtual StatusCode createAddress(const Context& refCtxt, 
				   IOpaqueAddress*&) const override;
  virtual StatusCode releaseContext(Context*& refCtxt) const override;
  virtual StatusCode resetCriteria(const std::string& cr,Context& c)const override;

  virtual StatusCode seek(Context& refCtxt, int) const override;
  virtual int curEvent (const Context& refCtxt) const override;
  virtual int size (Context& refCtxt) const override;

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




