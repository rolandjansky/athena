///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExReadD3pdEvtStore.cxx 
// Implementation file for class AthEx::ReadD3pdEvtStore
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AnalysisExamples includes
#include "AthExReadD3pdEvtStore.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// SGTools
#include "SGTools/BuiltinsClids.h"  // to put ints,... in evtstore
#include "SGTools/StlVectorClids.h" // to put std::vectors... in evtstore


namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ReadD3pdEvtStore::ReadD3pdEvtStore( const std::string& name, 
                                ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  m_runnbr = "RunNumber";
  m_evtnbr = "EventNumber"; 
  m_el_n   = "el_n";
  m_el_eta = "el_eta";
  m_el_jetcone_dr = "el_jetcone_dr";
  
}

// Destructor
///////////////
ReadD3pdEvtStore::~ReadD3pdEvtStore()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ReadD3pdEvtStore::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ReadD3pdEvtStore::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ReadD3pdEvtStore::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const uint32_t *runnbr = 0;
  const uint32_t *evtnbr = 0;
  const int32_t *el_n = 0;
  const std::vector<float> *el_eta = 0;
  const std::vector<std::vector<float> > *el_jetcone_dr = 0;

  if (!evtStore()->retrieve(runnbr, m_runnbr).isSuccess()) {
    ATH_MSG_INFO("could not retrieve [RunNumber]");
    return StatusCode::SUCCESS;
  }

  if (!evtStore()->retrieve(evtnbr, m_evtnbr).isSuccess()) {
    ATH_MSG_INFO("could not retrieve [EventNumber]");
    return StatusCode::SUCCESS;
  }

  if (!evtStore()->retrieve(el_n, m_el_n).isSuccess()) {
    ATH_MSG_INFO("could not retrieve [el_n]");
    return StatusCode::SUCCESS;
  }

  if (!evtStore()->retrieve(el_eta, m_el_eta).isSuccess()) {
    ATH_MSG_INFO("could not retrieve [el_eta]");
    return StatusCode::SUCCESS;
  }

  if (!evtStore()->retrieve(el_jetcone_dr, m_el_jetcone_dr).isSuccess()) {
    ATH_MSG_INFO("could not retrieve [el_jetcone_dr]");
    return StatusCode::SUCCESS;
  }

  msg(MSG::INFO) 
    << "run-nbr: " << *runnbr << endmsg
    << "evt-nbr: " << *evtnbr << endmsg
    << "el-nbr:  " << *el_n << endmsg;
  if (*el_n > 0) {
    msg(MSG::INFO) << "el.eta[0]: " << el_eta->at(0) << endmsg
                   << "el_jetcone_dr[0].size: "
                   << el_jetcone_dr->at(0).size()
                   << endmsg;
    for (std::size_t i = 0, imax = el_jetcone_dr->at(0).size();
         i != imax;
         ++i) {
      msg(MSG::INFO) << " jet-cone-dr[0][" << i << "]: "
                     << el_jetcone_dr->at(0)[i]
                     << endmsg;
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
