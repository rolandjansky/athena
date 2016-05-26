///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IEvtIdModifierSvc.h 
// Header file for class IEvtIdModifierSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_IEVTIDMODIFIERSVC_H 
#define ATHENAKERNEL_IEVTIDMODIFIERSVC_H 1 

/** @class IEvtIdModifierSvc
 */

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ClassID.h"

// AthenaKernel includes

// fwd declares
class EventID;

class IEvtIdModifierSvc
  : virtual public ::IInterface
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  typedef unsigned int number_type; //FIXME: keep in synch with EventID!!

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IEvtIdModifierSvc();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief return the current run-nbr (after modification)
   */
  virtual number_type run_number() const =0;

  /** @brief return the current evt-nbr (after modification)
   */
  virtual uint64_t event_number() const =0;

  /** @brief return the current time-stamp (after modification)
   */
  virtual number_type time_stamp() const =0;

  /** @brief return the current lbk-nbr (after modification)
   */
  virtual number_type lumi_block() const =0;

  /** @brief tell if the svc modified the run-nbr (for *this* event!)
   */
  virtual bool has_modified_run_number() const =0;

  /** @brief tell if the svc modified the evt-nbr (for *this* event!)
   */
  virtual bool has_modified_event_number() const =0;

  /** @brief tell if the svc modified the time-stamp (for *this* event!)
   */
  virtual bool has_modified_time_stamp() const =0;

  /** @brief tell if the svc modified the lbk-nbr (for *this* event!)
   */
  virtual bool has_modified_lumi_block() const =0;

  /** @brief return the (sorted) list of run-numbers which will be modified.
   */
  virtual
  std::vector<number_type> run_number_list() const =0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** @brief modify an `EventID`'s content
   */
  virtual
  void
  modify_evtid(EventID*& evt_id, bool consume_stream=false) = 0;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& IEvtIdModifierSvc::interfaceID() 
{ 
  static const InterfaceID IID_IEvtIdModifierSvc("IEvtIdModifierSvc", 1, 0);
  return IID_IEvtIdModifierSvc; 
}

#endif //> !ATHENAKERNEL_IEVTIDMODIFIERSVC_H
