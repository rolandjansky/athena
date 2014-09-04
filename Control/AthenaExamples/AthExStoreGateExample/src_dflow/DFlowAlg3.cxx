///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3.cxx 
// Implementation file for class DFlowAlg3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExStoreGateExample includes
#include "DFlowAlg3.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg3::DFlowAlg3( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );


  declareProperty("RIntFlow", 
                  m_r_int = SG::RVar<int>("dflow_int"),
                  "Data flow of int (read)");

  declareProperty("RIntsFlow", 
                  m_r_ints = SG::RVar<std::vector<int> >("dflow_ints"),
                  "Data flow of integers (read)");

  declareProperty("RWIntsFlow", 
                  m_rw_ints = SG::RWVar<std::vector<int> >("dflow_ints"),
                  "Data flow of integers (r/w)");
}

// Destructor
///////////////
DFlowAlg3::~DFlowAlg3()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg3::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg3::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg3::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  ATH_MSG_INFO("name: [" << m_r_int.name() << "]");
  ATH_MSG_INFO("store [" << m_r_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_r_int.clid() << "]");

  ATH_MSG_INFO("ptr: " << m_r_int.cptr());
  ATH_MSG_INFO("val: " << *(m_r_int.cptr()));

  ATH_MSG_INFO("ints r-handle...");
  ATH_MSG_INFO("name: [" << m_r_ints.name() << "]");
  ATH_MSG_INFO("store [" << m_r_ints.store() << "]");
  ATH_MSG_INFO("clid: [" << m_r_ints.clid() << "]");

  ATH_MSG_INFO("ptr: " << m_r_ints.ptr());

  ATH_MSG_INFO("ints rw-handle...");
  m_rw_ints->push_back(10);
  m_rw_ints->push_back(*m_r_int);
  ATH_MSG_INFO("size:" << m_rw_ints->size());
  for (int i = 0, imax = m_rw_ints->size();
       i!=imax;
       ++i) {
    ATH_MSG_INFO("val[" << i << "]= " << m_rw_ints->at(i));
  }

  // try to modify 'ints' via RVar<>
  // shouldn't compile
#ifdef TRY_COMPILATION_ERRORS
  m_r_ints->push_back(666);
#endif

  // create a temporary r-handle
  SG::RVar<std::vector<int> > ints(m_r_ints.name());
  ATH_MSG_INFO("temporary r-handle[ints] - size: " << ints->size());
  ATH_MSG_INFO("compare pointers: ok=" << (ints.ptr() == m_r_ints.ptr()));
  ATH_MSG_INFO("compare pointers: ok=" << (ints.ptr() == m_rw_ints.ptr()));

  // test that modification thru one handle is seen thru the other one
  std::vector<int> save = *m_rw_ints;
  m_rw_ints = std::vector<int>();
  ATH_MSG_INFO("temporary r-handle[ints] - size: " << ints->size());
  ATH_MSG_INFO("data mbr  r-handle[ints] - size: " << m_r_ints->size());
  ATH_MSG_INFO("data mbr rw-handle[ints] - size: " << m_rw_ints->size());

  ATH_MSG_INFO("--restore--");
  m_rw_ints = save;
  ATH_MSG_INFO("temporary r-handle[ints] - size: " << ints->size());
  ATH_MSG_INFO("data mbr  r-handle[ints] - size: " << m_r_ints->size());
  ATH_MSG_INFO("data mbr rw-handle[ints] - size: " << m_rw_ints->size());


  // test that inexistant proxies are correctly detected
  ATH_MSG_INFO("-- testing inexistant proxies --");
  {
    SG::RVar<int> o("--r-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }
  {
    SG::WVar<int> o("--w-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }
  {
    SG::RWVar<int> o("--rw-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }

  // 
  SG::RWVar<int> rw_int(m_r_int.name());
  ATH_MSG_INFO("temporary r/w-int: " << *rw_int);

  m_r_int.setConst();
  if (!m_r_int.isConst()) {
    ATH_MSG_ERROR("RVar<int>@[" << m_r_int.name() << "] should be CONST !");
    return StatusCode::FAILURE;
  }
  if (!rw_int.isConst()) {
    ATH_MSG_ERROR("RWVar<int>@[" << rw_int.name() << "] should be CONST !");
    return StatusCode::FAILURE;
  }

  // test WVar<T> semantics
  ATH_MSG_INFO("-- testing WVar<T> semantics...");
  {
    SG::WVar<int> o(m_r_int.name());
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
    try {
      o = 42;
      if (o.isValid()) {
        ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
        return StatusCode::FAILURE;
      }
    } catch (std::exception &err) {
      ATH_MSG_INFO("good, caught: [" << err.what() << "]");
    }
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }

  // test RWVar<T> semantics
  ATH_MSG_INFO("-- testing RWVar<T> semantics...");
  {
    SG::RWVar<int> o(m_r_int.name());
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
    try {
      o = 42;
      if (o.isValid()) {
        ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
        return StatusCode::FAILURE;
      }
    } catch (std::exception &err) {
      ATH_MSG_INFO("good, caught: [" << err.what() << "]");
    }
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
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
