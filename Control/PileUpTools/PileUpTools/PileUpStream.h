/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPSTREAM_PILEUPSTREAM_H
#define PILEUPSTREAM_PILEUPSTREAM_H
/** @file PileUpStream.h
 * @brief a triple selector/context/store defines a stream
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include <string>

// Framework include files
#include "GaudiKernel/IEvtSelector.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "xAODEventInfo/EventInfo.h"

// Forward declarations
class IMessageSvc;
class ISvcLocator;
class StoreGateSvc;
class ActiveStoreSvc;
class PileUpMergeSvc;

/** @class PileUpStream
 * @brief a triple selector/context/store defines a stream
 */
class PileUpStream {
public:
  typedef IEvtSelector::Context EvtIterator;

  /// @name Structors
  //@{
  PileUpStream();
  PileUpStream(const PileUpStream& rhs);
  PileUpStream& operator=(const PileUpStream& rhs);
  PileUpStream(const std::string& name, 
	       IEvtSelector* sel, 
	       StoreGateSvc* store);
  PileUpStream(const std::string& name, 
	       ISvcLocator* svcLoc,
	       const std::string& selecName);
  PileUpStream(const std::string& name, 
	       ISvcLocator* svcLoc,
	       IEvtSelector* sel);

  virtual ~PileUpStream();
  //@}
  /// @name accessors
  //@{
  inline const std::string& name() { return m_name; }
  inline IEvtSelector& selector() { return *p_sel; }
  inline EvtIterator& iterator() { return *p_iter; }
  inline StoreGateSvc& store() { return *p_SG; }
  inline const IEvtSelector& selector() const { return *p_sel; }
  inline const EvtIterator& iterator() const { return *p_iter; }
  inline const StoreGateSvc& store() const { return *p_SG; }
  //@}

  ///return next Event, load store with next Event
  const xAOD::EventInfo* nextEventPre(bool readRecord=true);

  ///like nextEventPre, but doesn't actually load anything
  bool nextEventPre_Passive(bool readRecord);

  /// setup input and overlay selectors and iters
  bool setupStore();
  /// backward compatibility
  inline bool setupStore(bool ) { return setupStore();}

  /// finalize and release store. To be called on ... finalize()
  StatusCode finalize();

  /// has this stream already be used in this event?
  bool used() const {return m_used;}
  void setUsed() {m_used=true;}
  void resetUsed() {m_used=false;}
  bool hasRing() const { return m_hasRing; }
  /// index of the ring for which this stream was used originally
  unsigned int originalIRing() const { return m_iOriginalRing; }
  void setOriginalIRing(unsigned int iR) {
    if (!m_hasRing) {
      m_hasRing=true;
      m_iOriginalRing=iR; 
    }
  }

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }
  
private:
  ISvcLocator* serviceLocator() { return p_svcLoc; }

  ///increment event iterator before loading store
  StatusCode nextRecordPre();

  ///increment event iterator 
  StatusCode nextRecordPre_Passive();

  ///clear store and load new proxies 
  bool loadStore();
 
  ///set ActiveStore
  void setActiveStore();

  //test for end of stream
  //  bool isNotEmpty() const;

  /// Stream name
  std::string m_name;
  /// ServiceLocator 
  ISvcLocator* p_svcLoc;
  /// Selector 
  IEvtSelector* p_sel;
  /// StoreGateSvc;
  StoreGateSvc* p_SG;
  /// Input Iterators
  EvtIterator* p_iter; 

  PileUpMergeSvc* p_mergeSvc;
  
  ActiveStoreSvc* p_activeStore;

  //mutable so that ownership can be passed upon copy
  mutable bool m_ownEvtIterator; ///> do we own p_iter? 
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
  bool m_neverLoaded;  ///> has an event been loaded into this stream?
  mutable bool m_ownStore;  ///> is p_SG a store we cloned from the master one?

  bool m_used; ///has this stream already been used? (for the current event)
  bool m_hasRing;
  unsigned int m_iOriginalRing; ///> original ring in which event was used
};
#endif // PILEUPSTREAM_PILEUPSTREAM_H





