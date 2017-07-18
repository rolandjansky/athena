/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IOVENTRYT_H
#define ATHENAKERNEL_IOVENTRYT_H

/*****************************************************************************
 *
 *  IOVEntryT.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *
 *  Validity range object that manages start and stop times, holding link
 *  to templated object.
 *
 *****************************************************************************/

#include "GaudiKernel/EventIDRange.h"
#include <set>

class EventIDBaseComp
{
public:
  bool operator() (const EventIDBase& e1, const EventIDBase& e2)
  {
    auto isRunLumi = [] (const EventIDBase& id)
      { return id.run_number() != EventIDBase::UNDEFNUM &&
               id.lumi_block() != EventIDBase::UNDEFNUM; };
    if (isRunLumi(e1) && isRunLumi(e2)) {
      unsigned int rn1 = e1.run_number();
      unsigned int rn2 = e2.run_number();
      unsigned int lb1 = e1.lumi_block();
      unsigned int lb2 = e2.lumi_block();
      return (std::tie(rn1, lb1) <
              std::tie(rn2, lb2));
    }
    return e1 < e2;
  }
};

template <typename T>
class IOVEntryT {
public: 

  // Ordered by increasing start times
  class IOVEntryTStartCritereon {
  public: 
    bool operator() ( const IOVEntryT<T> &p1, const IOVEntryT<T> &p2 ) const {
      return EventIDBaseComp() (p2.range().start(), p1.range().start());
    }
    bool operator() ( const IOVEntryT<T> *p1, const IOVEntryT<T> *p2 ) const {
      return EventIDBaseComp() (p2->range().start(), p1->range().start());
    }
  };

  // Order by decreasing stop times
  class IOVEntryTStopCritereon {
  public: 
    bool operator() ( const IOVEntryT<T> &p1, const IOVEntryT<T> &p2 ) const {
      return EventIDBaseComp() (p1.range().stop(), p2.range().stop());
    }
    bool operator() ( const IOVEntryT<T> *p1, const IOVEntryT<T> *p2 ) const {
      return EventIDBaseComp() (p1->range().stop(), p2->range().stop());
    }
  };

  IOVEntryT(){};
  IOVEntryT( T *obj, const EventIDRange& range):
    m_objPtr(obj), m_range(range){}

  ~IOVEntryT() {}

  EventIDRange range() const { return m_range; }
  void setRange( const EventIDRange& range) { m_range=range; }

  T* objPtr() const { return m_objPtr; }
  void setPtr( T* ptr ) { m_objPtr = ptr; }

  friend std::ostream& operator<< (std::ostream& os, const IOVEntryT<T>& rhs) {
    os << rhs.range() << " " << rhs.m_objPtr;
    return os;
  }

private:
  T* m_objPtr {};
  EventIDRange m_range {};

};

template <typename T>
class IOVEntryComp {
public:
  bool operator() ( const IOVEntryT<T> &p1, const IOVEntryT<T> &p2 ) const {
    return EventIDBaseComp() (p2->range().strart(), p1->range().start());
  }
};

// // Order by obj pointer
// class IOVEntryTProxyCritereon {
// public:
//   bool operator() ( const IOVEntryT &p1, const IOVEntryT &p2) const {
//     return p1.proxy() < p2.proxy();
//   }
//   bool operator() ( const IOVEntryT *p1, const IOVEntryT *p2) const {
//     return p1->proxy() < p2->proxy();
//   }
// };

#endif
