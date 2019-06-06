/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_IOVENTRY_H
#define IOVSVC_IOVENTRY_H

/*****************************************************************************
 *
 *  IOVEntry.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVEntry.h,v 1.8 2004-04-19 23:10:26 leggett Exp $
 *
 *  Validity range object that manages start and stop times, holding link
 *  to object DataProxy
 *
 *****************************************************************************/

#ifndef ATHENAKERNEL_IOVRANGE_H
 #include "AthenaKernel/IOVRange.h"
#endif
#ifndef SGTOOLS_DATAPROXY_H
 #include "SGTools/DataProxy.h"
#endif

#ifndef _CPP_SET
 #include <set>
#endif

class IOVEntry {
public: 

  // Ordered by increasing start times
  class IOVEntryStartCritereon {
  public: 
    bool operator() ( const IOVEntry &p1, const IOVEntry &p2 ) const {
      return p1.range()->start() > p2.range()->start();
    }
    bool operator() ( const IOVEntry *p1, const IOVEntry *p2 ) const {
      return p1->range()->start() > p2->range()->start();
    }
  };

  // Order by decreasing stop times
  class IOVEntryStopCritereon {
  public: 
    bool operator() ( const IOVEntry &p1, const IOVEntry &p2 ) const {
      return p1.range()->stop() < p2.range()->stop();
    }
    bool operator() ( const IOVEntry *p1, const IOVEntry *p2 ) const {
      return p1->range()->stop() < p2->range()->stop();
    }
  };

  typedef std::multiset<IOVEntry*,IOVEntryStartCritereon>::iterator startITR;
  typedef std::multiset<IOVEntry*,IOVEntryStopCritereon>::iterator  stopITR;

  IOVEntry( const SG::DataProxy *proxy, IOVRange* range):
    m_proxy(proxy), m_range(range), 
    m_removedStart(false), m_removedStop(false),
    m_startITR(0), m_stopITR(0) {}

  IOVEntry (const IOVEntry&) = delete;
  IOVEntry& operator= (const IOVEntry&) = delete;

  ~IOVEntry() { delete m_range; }

  IOVRange* range() const { return m_range; }
  void setRange( IOVRange* range) { m_range=range; }

  const SG::DataProxy* proxy() const { return m_proxy; }

  bool removedStart() const { return m_removedStart; }
  bool removedStop()  const { return m_removedStop; }

  void setRemovedStart(bool b) { m_removedStart = b; }
  void setRemovedStop(bool b)  { m_removedStop = b;  }

  void setStartITR( startITR itr ) { m_startITR = itr; }
  void setStopITR(  stopITR  itr ) { m_stopITR  = itr; }

  startITR getStartITR() const { return m_startITR; }
  stopITR  getStopITR()  const { return m_stopITR;  }

private:
  const SG::DataProxy* m_proxy;
  IOVRange* m_range;

  bool m_removedStart;
  bool m_removedStop;

  startITR m_startITR;
  stopITR  m_stopITR;

};

// Order by DataProxy
class IOVEntryProxyCritereon {
public:
  bool operator() ( const IOVEntry &p1, const IOVEntry &p2) const {
    return p1.proxy() < p2.proxy();
  }
  bool operator() ( const IOVEntry *p1, const IOVEntry *p2) const {
    return p1->proxy() < p2->proxy();
  }
};

#endif
