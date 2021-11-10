/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgB.h"
#include <thread>
#include <chrono>
#include <memory>

HiveAlgB::HiveAlgB( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  HiveAlgBase( name, pSvcLocator ),
  m_di(0) {}

HiveAlgB::~HiveAlgB() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode HiveAlgB::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_MSG_INFO("context: " << Gaudi::Hive::currentContext() << " for "
			   << this );

  ATH_MSG_INFO(" m_di was: " << m_di << " setting to -1 ");
  m_di = -1;

  // dump out contents of context specific data
  dump();

  ATH_CHECK( m_wrh1.initialize() );

  // initialize base class
  return HiveAlgBase::initialize();

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode HiveAlgB::finalize() {
  ATH_MSG_INFO("context: " << Gaudi::Hive::currentContext());
  ATH_MSG_DEBUG("finalize " << name());

  // dump out contents of context specific data
  dump();

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode HiveAlgB::execute() {

  ATH_MSG_DEBUG("execute " << name());

  ATH_MSG_INFO("context: " << Gaudi::Hive::currentContext() << " for "
	       << this);

  int s = sleep();

  ATH_MSG_INFO("m_di was: " << m_di << " setting to " << s);
  m_di = s;

  SG::WriteHandle<HiveDataObj> wrh1( m_wrh1 );
  ATH_CHECK(wrh1.record(std::make_unique< HiveDataObj >(20000)));

  ATH_MSG_INFO("  write: " << wrh1.key() << " = " << wrh1->val() );

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
HiveAlgB::dump() {

  std::ostringstream ost;

  // use a lambda to access all constituents of context specific data
  m_di.for_all([&ost] (size_t s, const int i) 
  	       { ost << " s: " << s << " v: " << i  << std::endl; } );

  ATH_MSG_INFO("dumping m_di: \n" << ost.str());

}

    
