/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_MIXINGEVENTSELECTOR_H
#define ATHENASERVICES_MIXINGEVENTSELECTOR_H
/** @file MixingEventSelector.h
 *  @brief a stream of events read from different selectors.
 *
 * $Id: MixingEventSelector.h,v 1.4 2008-09-23 22:00:47 binet Exp $
 * @author Paolo Calafiura
 */

#include <cassert>
#include <ostream>
#include <vector>

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IAthenaSelectorTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Property.h" /*StringArrayProperty*/

class ISvcLocator;
template <class TYPE> class SvcFactory;

class MixingEventIterator;

class StoreGateSvc;

class IAtRndmGenSvc;

namespace CLHEP {
  class RandFlat;
}


/** @class MixingEventSelector 
 *  @brief a stream of events read from different selectors.
 *
 * @details Each source contains a particular event type ("trigger").
 * We want to mix the triggers according to a recipe specified via the
 * "TriggerList" property
 *  @code
 *   Mixer.TriggerList += {"50% BSelector", "20 % MinBiasSelec"};
 *  @endcode
 * The algorithms downstream of the"mixer" will see events coming at random 
 * according to this admixture.  
 * The "OutputRunNumber" property allows to assign a new run number to the events.
 *  @code
 *   Mixer.OutputRun=1234;
 *  @endcode
 * The event  number can be randomized to mimic the behaviour of the 
 * processing farms. So
 *  @code
 *   Mixer.EventRandomRange=100;
 *  @endcode
 * will shuffle the first 100 events, then move to the second hundred, etc
 *   
 */
class MixingEventSelector : virtual public AthService,
			    virtual public IEvtSelector,
			    virtual public IAddressProvider
{
public:
  /** @class Context 
   * @brief  dummy for now 
   */
  struct Context : public IEvtSelector::Context {
    virtual ~Context() {}
    virtual void* identifier() const {
      return (void*)0;
    }
  } ;
  ///\name IEvtSelector implementation
  //@{
  virtual StatusCode next(IEvtSelector::Context& refCtxt) const;
  virtual StatusCode createAddress(const IEvtSelector::Context& refCtxt,
                                   IOpaqueAddress*&) const;

  virtual StatusCode createContext(IEvtSelector::Context*& refpCtxt) const;
  virtual StatusCode last(IEvtSelector::Context&) const;
  virtual StatusCode next(IEvtSelector::Context&, int) const;
  virtual StatusCode previous(IEvtSelector::Context&) const;
  virtual StatusCode previous(IEvtSelector::Context&,int) const;
  virtual StatusCode rewind(IEvtSelector::Context& refCtxt) const;
  virtual StatusCode releaseContext(IEvtSelector::Context*& refCtxt) const;
  virtual StatusCode resetCriteria(const std::string& cr,IEvtSelector::Context& c) const;
  //@}

  ///\name IAddressProvider interface (forwarded to current Trigger)
  //@{
  virtual StatusCode preLoadAddresses(StoreID::type  storeID ,
				      IAddressProvider::tadList& tads  );
  virtual StatusCode loadAddresses(StoreID::type  storeID ,
                                   IAddressProvider::tadList& tads  );
  virtual StatusCode updateAddress(StoreID::type  storeID ,
				   SG::TransientAddress*  tad);
  //@}

  
  /// IInterface implementation
  virtual StatusCode queryInterface( const InterfaceID& riid,
				     void** ppvInterface );

  /// \name Service implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //@}

  /// TriggerList property call-back. Decode TriggerList, obtain selector ptrs
  /// and store them alongside fractions in m_trigList
  void setUpTriggerList(Property& trigList); 
  /// functor that creates a Trigger object and adds it to m_trigList
  void decodeTrigger(std::string triggDescr);

protected:
  /// our factory
  friend class SvcFactory<MixingEventSelector>;
  /// \name structors
  //@{
  MixingEventSelector(const std::string& name, ISvcLocator* svc);
  virtual ~MixingEventSelector();
  //@}

private:
  bool validTrigger() const { return (m_trigList.end()!=m_pCurrentTrigger);}
  /// \name Properties
  //@{
  StringProperty      m_statusFileName;    ///< optional stream output snapshot
  StringArrayProperty m_triggerListProp;   ///< the recipe
  IntegerProperty m_outputRunNumber;        
  UnsignedLongArrayProperty m_eventNumbers;///< use these as event numbers
  StringProperty      m_mergedEventInfoKey;///< SG key of MergedEventInfo
  ToolHandleArray<IAthenaSelectorTool> m_helperTools;

  //@}


  /** @class Trigger
   *  @brief a selector with an associated frequency 
   */
  class Trigger {
  private:
    IEvtSelector* m_pSelector;
    unsigned int m_firstEvent;
    unsigned int m_toRead;
    unsigned int m_todo;
    mutable unsigned int m_reads;
    mutable IEvtSelector::Context* m_current;

  public:
    Trigger(IEvtSelector* pSel, 
	    //IEvtSelector::Context* pContext,
	    unsigned int firstEvt, unsigned int lastEvt);

    unsigned int todo() const { return m_todo; }
    unsigned int toRead() const { return m_toRead; }
    unsigned int read() const { return m_reads; }
    bool done() const;

    StatusCode next() const;
    StatusCode previous() const;
    StatusCode createAddress(IOpaqueAddress*&) const;
    StatusCode createContext(IEvtSelector::Context*& pCtxt) const;

    bool operator == (const Trigger& rhs) const {
      return ( m_pSelector == rhs.m_pSelector ); 
    }
    bool operator != (const Trigger& rhs) const {
      return ( m_pSelector != rhs.m_pSelector ); 
    }
    bool operator < (const Trigger& rhs) const {
      return ( m_todo < rhs.m_todo || 
	      ((m_todo == rhs.m_todo) &&  (m_pSelector < rhs.m_pSelector)) ); 
    }

    const std::string& name() const {
      return dynamic_cast<IService&>(selector()).name(); 
    }
    std::string toString() const ;

  private:
    IEvtSelector::Context& currentContext() const; //throws GaudiException
    IEvtSelector& selector() const {
      assert( m_pSelector );
      return *m_pSelector;
    }

  };

  /** @class TriggerList 
   *  @brief the list of available selectors with their frequency range 
   */
  class TriggerList {
  private:
    std::vector<Trigger> m_trigs;
    std::vector<unsigned int> m_rangeEnd;
    unsigned int m_todo;
  public:
    TriggerList() : m_trigs(), m_rangeEnd(), m_todo(0) {}
    typedef std::vector<Trigger>::const_iterator const_iterator;
    typedef std::vector<Trigger>::iterator iterator;
    bool add(const Trigger&);  ///< @returns false if duplicate
    bool remove(iterator); 
    iterator begin() { return m_trigs.begin(); }
    iterator end() { return m_trigs.end(); }
    const_iterator begin() const { return m_trigs.begin(); }
    const_iterator end() const { return m_trigs.end(); }
    bool empty() const { return m_trigs.empty(); }
    iterator elementInRange(unsigned int); 
    unsigned int todo() const { return m_todo; }
    std::string toString() const;
  };

  mutable TriggerList m_trigList; ///<the ingredients

  /// \name trigger list management
  //@{
  /// choose current trigger at random. Actually modifies state (use mutables)
  TriggerList::iterator setCurrentTrigger() const;  
  mutable TriggerList::iterator m_pCurrentTrigger;  ///<the current trigger
  TriggerList::iterator currentTrigger() const;     ///<"intelligent" accessor to the above
  //@}

  /// \name event number management
  //@{
  ///setup and lookup m_evtsNotUsedSoFar. Returns next event no
  unsigned long getEventNo() const;
  StatusCode addMergedEventInfo() const;
  mutable unsigned long m_eventPos;       ///< the internal event number
  //@}
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  mutable StoreGateSvc_t m_pEventStore;
  ServiceHandle<IAtRndmGenSvc> m_atRndmSvc;
  StringProperty m_randomStreamName;
  CLHEP::RandFlat* m_chooseRangeRand;

};
#endif // ATHENASERVICES_MIXINGEVENTSELECTOR_H

