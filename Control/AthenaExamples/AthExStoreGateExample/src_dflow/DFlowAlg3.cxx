///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3.cxx 
// Implementation file for class DFlowAlg3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#undef NDEBUG

// AthExStoreGateExample includes
#include "DFlowAlg3.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/UpdateHandle.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

namespace SG {
template<>
struct IsThreadSafeForUH<int, std::true_type>
{
  typedef std::true_type type;
};
}

namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg3::DFlowAlg3( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
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
  ATH_CHECK( m_r_int.initialize() );
  ATH_CHECK( m_r_ints.initialize() );
  ATH_CHECK( m_w_ints.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg3::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg3::execute (const EventContext& ctx) const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadHandle<int> r_int (m_r_int, ctx);
  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  ATH_MSG_INFO("name: [" << r_int.name() << "]");
  ATH_MSG_INFO("store [" << r_int.store() << "]");
  ATH_MSG_INFO("clid: [" << r_int.clid() << "]");

  ATH_MSG_INFO("ptr: " << r_int.cptr());
  if (r_int.isValid()) {
    ATH_MSG_INFO("val: " << *(r_int.cptr()));
  }

  SG::ReadHandle<std::vector<int> > r_ints (m_r_ints, ctx);
  ATH_MSG_INFO("ints r-handle...");
  ATH_MSG_INFO("name: [" << r_ints.name() << "]");
  ATH_MSG_INFO("store [" << r_ints.store() << "]");
  ATH_MSG_INFO("clid: [" << r_ints.clid() << "]");

  ATH_MSG_INFO("ptr: " << r_ints.ptr());

  ATH_MSG_INFO("ints w-handle...");
  
  SG::WriteHandle<std::vector<int> > w_ints (m_w_ints, ctx);
  ATH_CHECK( w_ints.record (std::make_unique<std::vector<int> > (*r_ints)) );
  w_ints->push_back(10);
  if (r_int.isValid()) {
    w_ints->push_back(*r_int);
  }
  ATH_MSG_INFO("size:" << w_ints->size());
  for (int i = 0, imax = w_ints->size();
       i!=imax;
       ++i) {
    ATH_MSG_INFO("val[" << i << "]= " << w_ints->at(i));
  }

  // try to modify 'ints' via ReadHandle<>
  // shouldn't compile
#ifdef TRY_COMPILATION_ERRORS
  r_ints->push_back(666);
#endif

  // create a temporary r-handle
  SG::ReadHandle<std::vector<int> > ints(r_ints.name());
  ATH_MSG_INFO("temporary r-handle[ints] - size: " << ints->size());
  ATH_MSG_INFO("compare pointers: ok=" << (ints.ptr() == r_ints.ptr()));
  ATH_MSG_INFO("compare pointers: ok=" << (ints.ptr() == w_ints.ptr()));

  // test that modification thru one handle is seen thru the other one
  std::vector<int> save = *w_ints;
  *w_ints = std::vector<int>();
  ATH_MSG_INFO("temporary r-handle[ints] - size: " << ints->size());
  if (r_int.isValid()) {
    ATH_MSG_INFO("data mbr  r-handle[ints] - size: " << r_ints->size());
  }
  ATH_MSG_INFO("data mbr w-handle[ints] - size: " << w_ints->size());

  ATH_MSG_INFO("--restore--");
  *w_ints = save;
  ATH_MSG_INFO("temporary r-handle[ints] - size: " << ints->size());
  if (r_int.isValid()) {
    ATH_MSG_INFO("data mbr  r-handle[ints] - size: " << r_ints->size());
  }
  ATH_MSG_INFO("data mbr w-handle[ints] - size: " << w_ints->size());


  // test that inexistant proxies are correctly detected
  ATH_MSG_INFO("-- testing inexistant proxies --");
  {
    SG::ReadHandle<int> o("--r-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }
  {
    SG::WriteHandle<int> o("--w-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }
  {
    SG::UpdateHandle<int> o("--rw-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }

  // 
  if (r_int.isValid()) {
    SG::UpdateHandle<int> rw_int(r_int.name());
    if (rw_int.isValid()) {
      ATH_MSG_INFO("temporary r/w-int: " << *rw_int);
    }
    ATH_CHECK( r_int.setConst() );
    if (!r_int.isConst()) {
      ATH_MSG_ERROR("ReadHandle<int>@[" << r_int.name() << "] should be CONST !");
      return StatusCode::FAILURE;
    }
    //now we can't put it into an update handle anymore
    SG::UpdateHandle<int> rw_const_int(r_int.name());
    if (rw_const_int.isValid()) {
      ATH_MSG_ERROR("UpdateHandle<int>@[" << r_int.name() << "] should not be allowed to refer to a const value !");
      return StatusCode::FAILURE;
    }

  }

  // test WVar<T> semantics
  ATH_MSG_INFO("-- testing WVar<T> semantics...");
  {
    SG::WriteHandle<int> o(r_int.name());
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
    try {
      *o = 42;
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
    SG::UpdateHandle<int> o(r_int.name());
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
    try {
      *o = 42;
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


} //> end namespace AthEx
