///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiContext.h 
// Header file for class McAsciiContext
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCASCIIEVENTSELECTOR_MCASCIICONTEXT_H 
#define MCASCIIEVENTSELECTOR_MCASCIICONTEXT_H 

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/IEvtSelector.h"

// Forward declaration
class McAsciiEventSelector;

class McAsciiContext : public IEvtSelector::Context
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  McAsciiContext( const McAsciiEventSelector* selector );
  McAsciiContext( const McAsciiEventSelector* selector,
		  const unsigned int initRunNbr,
		  const unsigned int initEvtNbr,
		  const unsigned int evtPerRun,
		  const unsigned int initTimeStamp,
		  const unsigned int timeStampInterval );
  /// Destructor: 
  virtual ~McAsciiContext(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  virtual void* identifier() const;

  unsigned int runNumber()   const;
  unsigned int eventNumber() const;
  unsigned int timeStamp()   const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  void setCriteria( const std::string& criteria );
  void rewind();
  void next();
  void previous();
  
  void setRunNumber( const unsigned int runNbr );
  void setEventNumber( const unsigned int eventNbr );
  void setEventsPerRun( const unsigned int eventsPerRun );
  void setTimeStamp( const unsigned int timeStamp );
  void setTimeStampInterval( const unsigned int interval );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  const McAsciiEventSelector* m_selector;
  std::string            m_criteria;

  unsigned int m_initRunNbr;
  unsigned int m_initEventNbr;
  unsigned int m_eventsPerRun;

  unsigned int m_initTimeStamp;
  unsigned int m_timeStampInterval;

  unsigned int m_runNbr;
  unsigned int m_eventNbr;
  unsigned int m_timeStamp;

  bool m_firstEvent;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline void* McAsciiContext::identifier() const
{ return (void*)m_selector; }

inline unsigned int McAsciiContext::runNumber()   const { return m_runNbr; }
inline unsigned int McAsciiContext::eventNumber() const { return m_eventNbr; }
inline unsigned int McAsciiContext::timeStamp()   const { return m_timeStamp; }

inline void McAsciiContext::setCriteria( const std::string& criteria )
{ m_criteria = criteria; }

inline void McAsciiContext::rewind()
{
  m_eventNbr  = m_initEventNbr;
  m_runNbr    = m_initRunNbr;
  m_timeStamp = m_initTimeStamp;
}

inline void McAsciiContext::previous()
{
  --m_eventNbr;
  m_timeStamp -= m_timeStampInterval;
}

inline void McAsciiContext::setRunNumber( unsigned int runNbr ) 
{ m_runNbr = runNbr; }

inline void McAsciiContext::setEventNumber( unsigned int eventNo ) 
{ m_eventNbr = eventNo ; }

inline void McAsciiContext::setEventsPerRun( unsigned int eventsP ) 
{ m_eventsPerRun = eventsP ; }

inline void McAsciiContext::setTimeStamp( unsigned int time ) 
{ m_timeStamp = time; }

inline void McAsciiContext::setTimeStampInterval( unsigned int intvl ) 
{ m_timeStampInterval = intvl; }

#endif //> MCASCIIEVENTSELECTOR_MCASCIICONTEXT_H
