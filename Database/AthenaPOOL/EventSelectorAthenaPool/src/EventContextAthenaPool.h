/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTEXTATHENAPOOL_H
#define EVENTCONTEXTATHENAPOOL_H

/** @file EventContextAthenaPool.h
 *  @brief This file contains the class definition for the EventContextAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/IEvtSelector.h"

class EventSelectorAthenaPool;

/** @class EventContextAthenaPool
 *  @brief This class provides the context to access an event from POOL persistent store.
 **/
class EventContextAthenaPool : virtual public IEvtSelector::Context {

public:
   /// Constructor
   EventContextAthenaPool(const IEvtSelector* selector);
   /// Copy constructor
   EventContextAthenaPool(const EventContextAthenaPool& ctxt);
   /// Destructor
   virtual ~EventContextAthenaPool();
   /// Assignment operator
   const EventContextAthenaPool& operator=(const EventContextAthenaPool& rhs);

   /// @return pointer to identifier.
   virtual void* identifier() const;

private: // data
   const IEvtSelector* m_evtSelector;
};

inline EventContextAthenaPool::EventContextAthenaPool(const IEvtSelector* selector) :
	IEvtSelector::Context(),
	m_evtSelector(selector) {}
inline EventContextAthenaPool::EventContextAthenaPool(const EventContextAthenaPool& ctxt) :
	IEvtSelector::Context(*this),
	m_evtSelector(ctxt.m_evtSelector) {}
inline EventContextAthenaPool::~EventContextAthenaPool() {}

inline const EventContextAthenaPool& EventContextAthenaPool::operator=(const EventContextAthenaPool& rhs) {
   if (&rhs != this) {
      IEvtSelector::Context::operator=(rhs);
      m_evtSelector = rhs.m_evtSelector;
   }
   return(rhs);
}

inline void* EventContextAthenaPool::identifier() const {
   return((void*)(m_evtSelector));
}
#endif
