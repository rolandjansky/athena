/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/* -C++- */
#ifndef EVENTINFO_PILEUPEVENTINFO_H
# define EVENTINFO_PILEUPEVENTINFO_H 1
/**
 * @file PileUpEventInfo.h
 *
 * @brief This class provides information about an overlaid event.
 *  It extends EventInfo with a list of sub-evts (the original
 *  and the bkg ones)
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: PileUpEventInfo.h,v 1.9 2008-06-19 20:08:06 ketevi Exp $
 */

#include <cassert>
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "GaudiKernel/ClassID.h"

#include <list>
class StoreGateSvc;
class MsgStream;

/** @class PileUpEventInfo
 *
 * @brief This class provides information about an overlaid event.
 *  It extends EventInfo with a list of sub-evts (the original
 *  and the bkg ones)
 *
 **/

class PileUpEventInfo: public EventInfo {
public:
  /// \name structors (same as EventInfo)
  //@{
  PileUpEventInfo();

  PileUpEventInfo(EventID* id, EventType* type);
  PileUpEventInfo(EventID* id, EventType* type, TriggerInfo* trig_info);

  virtual ~PileUpEventInfo();
  //@}

  /** @class SubEvent
   *  @brief a triple allowing access to a sub-event
   **/
  struct SubEvent {
  private:
    PileUpTimeEventIndex m_timeIndex;
  public:
    EventInfo* pSubEvt; ///< redundant but handy e.g. for persist. FIXME OWNED COPY
    StoreGateSvc* pSubEvtSG;  ///< the sub event store

    typedef PileUpTimeEventIndex::time_type time_type;
    typedef PileUpTimeEventIndex::index_type index_type;
    typedef PileUpTimeEventIndex::PileUpType pileup_type;

    SubEvent();
    ~SubEvent();
    SubEvent(const SubEvent&);
    SubEvent& operator = (const SubEvent&);
    SubEvent(SubEvent&&);
    SubEvent& operator = (SubEvent&&);
    SubEvent(time_type t, const EventInfo* pse, StoreGateSvc* psg);
    SubEvent(time_type t, index_type index,
             const EventInfo* pse, StoreGateSvc* psg);
    SubEvent(time_type t, index_type index,
             PileUpTimeEventIndex::PileUpType typ,
             const EventInfo* pse, StoreGateSvc* psg);
    SubEvent(time_type t, unsigned int BCID, index_type index,
             PileUpTimeEventIndex::PileUpType typ,
             const EventInfo& rse, StoreGateSvc* psg);

    SubEvent(time_type t, std::unique_ptr<EventInfo> pse, StoreGateSvc* psg);
    SubEvent(time_type t, index_type index,
             std::unique_ptr<EventInfo> pse, StoreGateSvc* psg);
    SubEvent(time_type t, index_type index,
             PileUpTimeEventIndex::PileUpType typ,
             std::unique_ptr<EventInfo> pse, StoreGateSvc* psg);
    SubEvent(time_type t, unsigned int BCID, index_type index,
             PileUpTimeEventIndex::PileUpType typ,
             std::unique_ptr<EventInfo> pse, StoreGateSvc* psg);

    ///t0 wrto original event (hence original evt time == 0)
    time_type time() const {return m_timeIndex.time();}
    index_type index() const {return m_timeIndex.index();}

    unsigned int BCID() const;
    /** pileup event proveance */
    pileup_type type() const {return m_timeIndex.type();}

    typedef std::list<SubEvent>::const_iterator const_iterator;
    typedef std::list<SubEvent>::iterator iterator;
  };

  typedef SubEvent::time_type time_type;
  typedef SubEvent::index_type index_type;
  ///setter for the subEvt collection t=0(ns) for the original event
  void addSubEvt(time_type t, PileUpTimeEventIndex::PileUpType puType,
                 const EventInfo* pse, StoreGateSvc* psg) {
    m_subEvents.emplace_back(t, m_subEvents.size(), puType, pse, psg);
  }

  void addSubEvt(time_type t, PileUpTimeEventIndex::PileUpType puType,
                 std::unique_ptr<EventInfo> pse, StoreGateSvc* psg) {
    m_subEvents.emplace_back(t, m_subEvents.size(), puType, std::move(pse), psg);
  }

  ///setter for the subEvt collection t=0(ns) for the original event
  void addSubEvt(time_type t, unsigned int BCID,
                 PileUpTimeEventIndex::PileUpType puType,
                 const EventInfo& rse, StoreGateSvc* psg) {
    m_subEvents.emplace_back(t, BCID, m_subEvents.size(), puType, rse, psg);
  }

  void addSubEvt(time_type t, unsigned int BCID,
                 PileUpTimeEventIndex::PileUpType puType,
                 std::unique_ptr<EventInfo>& pse, StoreGateSvc* psg) {
    m_subEvents.emplace_back(t, BCID, m_subEvents.size(), puType, std::move(pse), psg);
  }

  //NO INSERTIONS only push_backs in m_subEvents!

  /// \name accessors to the subEvt collection
  //@{
  SubEvent::iterator beginSubEvt() {return m_subEvents.begin();}
  SubEvent::iterator endSubEvt() {return m_subEvents.end();}
  SubEvent::const_iterator beginSubEvt() const {return m_subEvents.begin();}
  SubEvent::const_iterator endSubEvt() const {return m_subEvents.end();}
  //@}
private:
  std::list<SubEvent> m_subEvents; //FIXME should become a vector
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(PileUpEventInfo, 2102, 1)

#include "AthenaKernel/BaseInfo.h"
SG_ADD_BASE(PileUpEventInfo, EventInfo);

#endif // EVENTINFO_PILEUPEVENTINFO_H
