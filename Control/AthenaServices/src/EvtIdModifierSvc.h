///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// EvtIdModifierSvc.h 
// Header file for class EvtIdModifierSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_EVTIDMODIFIERSVC_H
#define ATHENASERVICES_EVTIDMODIFIERSVC_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

// AthenaKernel
#include "AthenaKernel/IEvtIdModifierSvc.h"

// event includes
#include "EventInfo/EventID.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;


struct ItemModifier
{
  EventID::number_type runnbr;
  uint64_t             evtnbr;
  EventID::number_type timestamp;
  EventID::number_type lbknbr;
  EventID::number_type nevts;
  int mod_bit;
  ItemModifier() :
    runnbr(0),
    evtnbr(0),
    timestamp(0),
    lbknbr(0),
    nevts(0),
    mod_bit(0)
  {}
};

class EvtIdModifierSvc
  : virtual public ::IEvtIdModifierSvc,
            public ::AthService
{ 
  friend class SvcFactory<EvtIdModifierSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  EvtIdModifierSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~EvtIdModifierSvc(); 

  // Assignment operator: 
  //EvtIdModifierSvc &operator=(const EvtIdModifierSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief return the current run-nbr (after modification)
   */
  virtual number_type run_number() const;

  /** @brief return the current evt-nbr (after modification)
   */
  virtual uint64_t event_number() const;

  /** @brief return the current time-stamp (after modification)
   */
  virtual number_type time_stamp() const;

  /** @brief return the current lbk-nbr (after modification)
   */
  virtual number_type lumi_block() const;

  /** @brief tell if the svc modified the run-nbr (for *this* event!)
   */
  virtual bool has_modified_run_number() const;

  /** @brief tell if the svc modified the evt-nbr (for *this* event!)
   */
  virtual bool has_modified_event_number() const;

  /** @brief tell if the svc modified the time-stamp (for *this* event!)
   */
  virtual bool has_modified_time_stamp() const;

  /** @brief tell if the svc modified the lbk-nbr (for *this* event!)
   */
  virtual bool has_modified_lumi_block() const;

  /** @brief return the (sorted) list of run-numbers which will be modified.
   */
  virtual
  std::vector<number_type> run_number_list() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** @brief modify an `EventID`'s lumi block content.
   */
  virtual
  void
  modify_evtid(EventID*& evt_id, bool consume_stream);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  EvtIdModifierSvc();

  /// first event number at which we begin to modify event ids
  unsigned int m_firstEvtIdx;

  /// total number of events inspected so-far
  unsigned int m_allEvtsCounter;

  /// lumi block counter (per evt)
  unsigned int m_evtCounter;

  /// (prop) list of n-plets
  /// (run-nbr, evt-nbr, time-stamp, lbk-nbr, nbr-of-events-per-lbk, mod-bit)
  std::vector<uint64_t> m_evtNpletsProp;

  /// (prop) Name of the event store whose EventIDs will be modified.
  std::string m_evtStoreName;

  typedef std::vector<ItemModifier> ModDb_t;
  /// db of list of ItemModifiers:
  /// (run-nbr, evt-nbr, time-stamp, lbk-nbr, nbr-of-events-per-lbk, mod-bit)
  ModDb_t m_evtNplets;

  /// iterator pointing at the current n-plet describing how to massage evtid
  ModDb_t::iterator m_cursor;

  /// cached item-modifier: the one after the current modification has been
  /// applied
  ItemModifier m_current;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& EvtIdModifierSvc::interfaceID() 
{ 
  return IEvtIdModifierSvc::interfaceID(); 
}



#endif //> !ATHENASERVICES_EVTIDMODIFIERSVC_H
