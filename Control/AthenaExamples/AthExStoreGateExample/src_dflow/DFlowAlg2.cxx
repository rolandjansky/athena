///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg2.cxx 
// Implementation file for class DFlowAlg2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#undef NDEBUG

// AthExStoreGateExample includes
#include "DFlowAlg2.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"

namespace AthEx {


DFlowAlg2::DFlowAlg2( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
}

// Destructor
///////////////
DFlowAlg2::~DFlowAlg2()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg2::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK( m_r_int.initialize() );
  ATH_CHECK( m_w_int.initialize() );
  ATH_CHECK( m_ints.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2::execute (const EventContext& ctx) const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  SG::ReadHandle<int> r_int (m_r_int, ctx);
  ATH_MSG_INFO("name: [" << r_int.name() << "]");
  ATH_MSG_INFO("store [" << r_int.store() << "]");
  ATH_MSG_INFO("clid: [" << r_int.clid() << "]");

  ATH_MSG_INFO("ptr: " << r_int.cptr());
  if (r_int.isValid()) {
    ATH_MSG_INFO("val: " << *(r_int.cptr()));
  }
  SG::WriteHandle<int> w_int (m_w_int, ctx);
  ATH_MSG_INFO("myint w-handle...");
  ATH_MSG_INFO("name: [" << w_int.name() << "]");
  ATH_MSG_INFO("store [" << w_int.store() << "]");
  ATH_MSG_INFO("clid: [" << w_int.clid() << "]");

  ATH_CHECK( w_int.record (std::make_unique<int> (*r_int + 1000)) );

  ATH_MSG_INFO("val: " << *w_int);
  ATH_MSG_INFO("cptr: " << w_int.cptr());

  ATH_MSG_INFO("ints w-handle...");
  SG::WriteHandle<std::vector<int> > ints (m_ints, ctx);
  ATH_CHECK( ints.record (std::make_unique<std::vector<int> >()) );
  ints->push_back(10);
  if (r_int.isValid()) ints->push_back(*r_int);
  ATH_MSG_INFO("size:" << ints->size());
  for (int i = 0, imax = ints->size();
       i!=imax;
       ++i) {
    ATH_MSG_INFO("val[" << i << "]= " << ints->at(i));
  }
  return StatusCode::SUCCESS;
}

} //> end namespace AthEx
