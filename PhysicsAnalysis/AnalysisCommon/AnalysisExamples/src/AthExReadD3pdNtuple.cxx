///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExReadD3pdNtuple.cxx 
// Implementation file for class AthEx::ReadD3pdNtuple
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AnalysisExamples includes
#include "AthExReadD3pdNtuple.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// ROOT includes
#include "TTree.h"

namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ReadD3pdNtuple::ReadD3pdNtuple( const std::string& name, 
                                ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("TreeName", 
                  m_treeName = "/temp/TTreeStream/egamma", 
                  "Name of the TTree to access in THistSvc");
  m_runnbr = 0;
  m_evtnbr = 0;
  m_el_n   = 0;
  m_el_eta = 0;
  m_el_jetcone_dr = 0;
}

// Destructor
///////////////
ReadD3pdNtuple::~ReadD3pdNtuple()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ReadD3pdNtuple::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ServiceHandle<ITHistSvc> hsvc("THistSvc/THistSvc", this->name());
  if (!hsvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve THistSvc!");
    return StatusCode::FAILURE;
  }
  TTree *tree = 0;
  if (!hsvc->getTree(m_treeName, tree).isSuccess() || 
      0 == tree) {
    ATH_MSG_ERROR("Could not register with our ttree!");
    return StatusCode::FAILURE;
  }

  // connect our variables to the tree
  if (!initBranches(tree).isSuccess()) {
    ATH_MSG_ERROR("could not configure branches !");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadD3pdNtuple::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ReadD3pdNtuple::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  msg(MSG::INFO) 
    << "run-nbr: " << m_runnbr << endmsg
    << "evt-nbr: " << m_evtnbr << endmsg
    << "el-nbr:  " << m_el_n << endmsg;
  if (m_el_n > 0) {
    msg(MSG::INFO) << "el.eta[0]: " << m_el_eta->at(0) << endmsg
                   << "el_jetcone_dr[0].size: "
                   << m_el_jetcone_dr->at(0).size()
                   << endmsg;
    for (std::size_t i = 0, imax = m_el_jetcone_dr->at(0).size();
         i != imax;
         ++i) {
      msg(MSG::INFO) << " jet-cone-dr[0][" << i << "]: "
                     << m_el_jetcone_dr->at(0)[i]
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

StatusCode
ReadD3pdNtuple::initBranches(TTree *tree)
{
  {
    TBranch *br = tree->GetBranch("RunNumber");
    if (0==br) {
      ATH_MSG_ERROR("could not set branch address for [RunNumber]");
      return StatusCode::FAILURE;
    }
    br->SetStatus(1);
    br->SetAddress(&m_runnbr);
  }
  {
    TBranch *br = tree->GetBranch("EventNumber");
    if (0==br) {
      ATH_MSG_ERROR("could not set branch address for [EventNumber]");
      return StatusCode::FAILURE;
    }
    br->SetStatus(1);
    br->SetAddress(&m_evtnbr);
  }
  {
    TBranch *br = tree->GetBranch("el_n");
    if (0==br) {
      ATH_MSG_ERROR("could not set branch address for [el_n]");
      return StatusCode::FAILURE;
    }
    br->SetStatus(1);
    br->SetAddress(&m_el_n);
  }
  ServiceHandle<IDictLoaderSvc> dictsvc("AthDictLoaderSvc", this->name());
  {
    // load the reflex dict for this type:
    if (dictsvc->load_type("std::vector<float>").Name() == "") {
      ATH_MSG_ERROR("could not load reflex type for [std::vector<float>]!");
      return StatusCode::FAILURE;
    }
    TBranch *br = tree->GetBranch("el_eta");
    if (0==br) {
      ATH_MSG_ERROR("could not set branch address for [el_eta]");
      return StatusCode::FAILURE;
    }
    br->SetStatus(1);
    br->SetAddress(&m_el_eta);
  }
  {
    // load the reflex dict for this type:
    if (dictsvc->load_type("std::vector<std::vector<float> >").Name() == "") {
      ATH_MSG_ERROR("could not load reflex type for [std::vector<std::vector<int> >]!");
      return StatusCode::FAILURE;
    }
    TBranch *br = tree->GetBranch("el_jetcone_dr");
    if (0==br) {
      ATH_MSG_ERROR("could not set branch address for [el_jetcone_dr]");
      return StatusCode::FAILURE;
    }
    br->SetStatus(1);
    br->SetAddress(&m_el_jetcone_dr);
  }

  return StatusCode::SUCCESS;
}

} //> end namespace AthEx
