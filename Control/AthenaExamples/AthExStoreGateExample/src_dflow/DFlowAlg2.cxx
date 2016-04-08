///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "CxxUtils/make_unique.h"

namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg2::DFlowAlg2( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_int("dflow_int"),
  m_rw_int("dflow_int"),
  m_ints("dlow_ints")
  
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("RIntFlow", 
                  m_r_int = SG::ReadHandle<int>("dflow_int"),
                  "Data flow of int");

  declareProperty("RWIntFlow", 
                  m_rw_int = SG::UpdateHandle<int>("dflow_int"),
                  "Data flow of int");

  declareProperty("IntsFlow", 
                  m_ints = SG::WriteHandle<std::vector<int> >("dflow_ints"),
                  "Data flow of integers");

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

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  ATH_MSG_INFO("name: [" << m_r_int.name() << "]");
  ATH_MSG_INFO("store [" << m_r_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_r_int.clid() << "]");

  ATH_MSG_INFO("ptr: " << m_r_int.cptr());
  if (m_r_int.isValid()) {
    ATH_MSG_INFO("val: " << *(m_r_int.cptr()));
  }
  ATH_MSG_INFO("myint rw-handle...");
  ATH_MSG_INFO("name: [" << m_rw_int.name() << "]");
  ATH_MSG_INFO("store [" << m_rw_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_rw_int.clid() << "]");

  ATH_MSG_INFO("ptr: " << m_rw_int.ptr());
  if (m_rw_int.isValid()) {
    ATH_MSG_INFO("val: " << *(m_rw_int.cptr()));
    *m_rw_int += 100;

    ATH_MSG_INFO("val: " << *m_rw_int);
  }
  ATH_MSG_INFO("cptr: " << m_rw_int.cptr());

  ATH_MSG_INFO("ints w-handle...");
  m_ints = CxxUtils::make_unique<std::vector<int> >();
  m_ints->push_back(10);
  //would be nice if it worked...  if (0 != m_r_int) m_ints->push_back(*m_r_int);
  if (m_r_int.isValid()) m_ints->push_back(*m_r_int);
  ATH_MSG_INFO("size:" << m_ints->size());
  for (int i = 0, imax = m_ints->size();
       i!=imax;
       ++i) {
    ATH_MSG_INFO("val[" << i << "]= " << m_ints->at(i));
  }
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthEx
