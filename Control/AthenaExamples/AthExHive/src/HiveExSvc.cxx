/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveExSvc.h"


#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
DECLARE_SERVICE_FACTORY(HiveExSvc)


thread_local int s_i(0);


HiveExSvc::HiveExSvc(const std::string& name, ISvcLocator* svc)
  : base_class( name, svc )
  ,m_di(0)
    
 {


}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveExSvc::~HiveExSvc() {

}

StatusCode
HiveExSvc::initialize() {
  ATH_MSG_INFO("initialize");

  return StatusCode::SUCCESS;
}

StatusCode
HiveExSvc::finalize() {
  ATH_MSG_INFO("finalize");

  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "listing all contents of m_di" << std::endl;

  m_di.for_all( [&log] (size_t s, const int i) 
   		{ log << "    s: " << s << "  v: " << i << std::endl; } );

  log << "listing all contents of m_ci" << std::endl;

  m_ci.for_all( [&log] (size_t s, const int *i) 
   		{ log << "    s: " << s << "  v: " << *i << std::endl; } );


  log << "listing all contents of m_dq" << std::endl;

  m_dq.for_all([&log](size_t s, const std::vector<sDat> v) {
      log << "   s: " << s;
      int st(0);
      for (auto e : v) {
	st += e.time;
	log << " " << e.name << ":" << e.nEvt << "/" << e.nSlot
	    << "|" << e.time;
      }
      // this is not a real sum of the time, because slot != thread
      log << "   total time: " << st << " ms\n";
    } 
    );

  log << endmsg;

  

  return StatusCode::SUCCESS;
}

void
HiveExSvc::add(const std::string& n, const int& t) {
  
  EventContext::ContextID_t s = Gaudi::Hive::currentContextId();
  EventContext::ContextEvt_t e = Gaudi::Hive::currentContextEvt();


  std::vector<sDat> &v = m_dq;
  v.push_back( sDat(s,e,t,n) );

  //  ((std::vector<sDat>)m_dq).push_back( sDat(s,e,n) );


}

void 
HiveExSvc::set(const int& i) {

  m_di = i;
  
  // this is not thread safe!
  if (! m_ci) {
    m_ci = new int(i);
  } else {
    *m_ci = i;
  }

}

void 
HiveExSvc::setTL(const int& i) {

  s_i = i;
  
}

int
HiveExSvc::get() const {
  //  return *m_ci;
  return m_di;
}

int
HiveExSvc::getTL() const {
  return s_i;
}


