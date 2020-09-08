/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveExSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ConcurrencyFlags.h"
#include "GaudiKernel/EventContext.h"

#include <map>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveExSvc::HiveExSvc(const std::string& name, ISvcLocator* svc)
  : base_class( name, svc ) {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveExSvc::~HiveExSvc() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
HiveExSvc::initialize() {

  size_t nslots = Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents();
  if (nslots == 0) {
    nslots = 1;
  }

  ATH_MSG_INFO("initialize structures of size " << nslots);
  
  // initialize the structures with the number of concurrent events
  m_times.resize( nslots );
  m_locks.resize( nslots );
  for ( auto & m : m_locks ) {
    m = std::make_unique<std::mutex>();
  }
  
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
HiveExSvc::finalize() {
  ATH_MSG_INFO("finalize");

  // calculate the standard deviation of the sleep times
  std::map<std::string,unsigned int> sum;
  std::map<std::string,unsigned int> sumSq;
  std::map<std::string,unsigned int> num;

  for (const auto &ve : m_times) {
    for (const auto &e : ve) {
      sum[e.algName] += e.sleep_time;
      sumSq[e.algName] += e.sleep_time * e.sleep_time;
      num[e.algName] ++;
    }
  }

  info() << "listing timing by alg:";
  for (const auto &s : sum) {
    float avg = s.second/num[s.first];
    float sig = sqrt( ( sumSq[s.first] - 2*s.second*avg + num[s.first]*avg*avg  )/(num[s.first]) );
    info() << "\n  " << s.first 
           << "  avg: " << avg << "  sig: " << sig;
  }
  info() << endmsg;
    
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
HiveExSvc::add(const std::string& algName, const unsigned int& time) {

  // even though we have separate data structures per concurrent
  // event (slot), we can't be sure that multiple threads aren't
  // processing the same slot at the same time, so need to lock the mutex.
  // this reduces the waiting for a lock, compared with locking the
  // whole structure, as it's unlikely that we'll have the same Alg in
  // different concurrent events executing at the same time.

  // for a true lock-free design, we would need a separate container for
  // each Algorithm in each slot

  EventContext::ContextID_t slot = Gaudi::Hive::currentContextId();
  std::lock_guard<std::mutex> lock ( *m_locks[slot] );
  m_times[slot].push_back( tDat(algName, time) );
}

