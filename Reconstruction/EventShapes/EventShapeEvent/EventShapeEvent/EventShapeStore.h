// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPES_EVENTSHAPESTORE_H
#define EVENTSHAPES_EVENTSHAPESTORE_H

#include "EventShapeEvent/EventShapes.h"

#include <vector>

class EventShapeStore
{
 public:
  
  template<class T>
    friend class EventShapeStoreConverterBase;
  
  EventShapeStore() {};
  
  ~EventShapeStore() {};
  
  bool get(const EventShapes::EventShapeType& type, double& value, unsigned int index = 0) const;

  // bool get(const EventShapes::EventShapeType& type, 
  // 	   std::vector<double>& values, unsigned int index = 0);
  // bool get(const EventShapes::EventShapeType& type,
  // 	   std::vector<std::vector<double> >& valueBlock);
  
  void insert(EventShapes &es)
    {
      for(store_iter it=m_store.begin(); it!=m_store.end(); it++ )
	if(it->getType()==es.getType())
	  {
	    it->swapValues( es );
	    return;
	  }
      m_store.push_back(es);
    };
  
  void insert(EventShapes::EventShapeType type, double value)
    {
      for(store_iter it=m_store.begin(); it!=m_store.end(); it++ )
	if(it->getType()==type)
	  {
	    it->setValue(value);
	    return;
	  }
      m_store.push_back(EventShapes(type,value));
    };
  
  void insert(EventShapes::EventShapeType type, std::vector<double>& values, bool canSwap = false)
    {
      for(store_iter it=m_store.begin(); it!=m_store.end(); it++ )
	if(it->getType()==type)
	  {
	    it->setValues(values, canSwap);
	    return;
	  }
      m_store.push_back(EventShapes(type, values, canSwap));
    };

 private:
  
  typedef  std::vector<EventShapes>    store_type;
  
  typedef  store_type::iterator        store_iter;
  
  typedef  store_type::const_iterator  store_Citer;
  
  store_type   m_store;
  
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(EventShapeStore, 79242525, 1)

#endif

