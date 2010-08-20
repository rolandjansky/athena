///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiContext.cxx 
// Implementation file for class McAsciiContext
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Package includes
#include "McAsciiContext.h"
#include "McAsciiEventSelector.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
McAsciiContext::McAsciiContext( const McAsciiEventSelector* selector ):
  m_selector( selector ), 
  m_initRunNbr        ( 0 ), 
  m_initEventNbr      ( 0 ), 
  m_eventsPerRun      ( 0 ),
  m_initTimeStamp     ( 0 ), 
  m_timeStampInterval ( 0 ), 
  m_runNbr            ( 0 ),
  m_eventNbr          ( 0 ),
  m_timeStamp         ( 0 ),
  m_firstEvent( true ) 
{}

McAsciiContext::McAsciiContext( const McAsciiEventSelector* selector, 
                                const unsigned int initRunNbr, 
                                const unsigned int initEventNbr, 
                                const unsigned int evPerRun, 
                                const unsigned int initTimeStamp,
                                const unsigned int timeStampInt ) :
  m_selector          ( selector ),
  m_initRunNbr        ( initRunNbr ), 
  m_initEventNbr      ( initEventNbr ), 
  m_eventsPerRun      ( evPerRun ),
  m_initTimeStamp     ( initTimeStamp ), 
  m_timeStampInterval ( timeStampInt ), 
  m_runNbr            ( 0 ),
  m_eventNbr          ( 0 ),
  m_timeStamp         ( 0 ),
  m_firstEvent(true) 
{}

// Destructor
///////////////
McAsciiContext::~McAsciiContext()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void McAsciiContext::next()
{
  if ( m_firstEvent ) {
    m_firstEvent = false;
    
    m_eventNbr  = m_initEventNbr;
    m_runNbr    = m_initRunNbr;
    m_timeStamp = m_initTimeStamp;

    return;
  }

  if ( m_eventNbr >= m_eventsPerRun ) {
    ++m_runNbr;
    m_eventNbr = 0;
  }

  ++m_eventNbr;
  m_timeStamp += m_timeStampInterval;    
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


