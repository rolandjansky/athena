/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ThreadGaudi.h"
#include "TrigHLTBaseTreeTool.h"
#include <TTree.h>
#include "TrigInterfaces/Algo.h"



TrigHLTBaseTreeTool::TrigHLTBaseTreeTool(const std::string & type, 
					 const std::string & name,
					 const IInterface* parent)
    : TrigMonitorToolBase(type, name, parent),
      m_pass{0,0},
      m_path("/EXPERT/NTUPLES/"),
      m_algo(0),
      m_tree(0),
      event(0)
{
    declareProperty("BookingDir", m_path);
}
TrigHLTBaseTreeTool::~TrigHLTBaseTreeTool() {}

StatusCode TrigHLTBaseTreeTool::bookHists() {

  ITHistSvc *rootHistSvc;
  ATH_CHECK(service("THistSvc", rootHistSvc));
    
  // find out for whom we are running (i.e. Algo)    
  const HLT::Algo *parentAlg = dynamic_cast<const HLT::Algo*>(parent());
  if ( parentAlg ) {
	m_parentName = getGaudiThreadGenericName(parentAlg->name());
	m_algo = const_cast<HLT::Algo*>(parentAlg);
  } else {
	ATH_MSG_WARNING("Not a HLT::Algo class");
  }
    
  // book the tree 
  std::string treeName = m_path + m_parentName;
  // try to get global tree  make my own tree if can not get global
    
  m_tree = new TTree ( m_parentName.c_str(), "HLT::Alg Tree");    
  if ( rootHistSvc->regTree(treeName, m_tree).isFailure() ) {
	ATH_MSG_WARNING("Unable to register TTree");
  }
  m_tree->Branch("PassInfo", &m_pass, "Lvl1Id/I:RoIId/I" );
  return StatusCode::SUCCESS;
}

StatusCode TrigHLTBaseTreeTool::fillHists() { 
    if ( m_algo ) {
      m_pass.Lvl1Id = m_algo->config()->getLvl1Id();
    }
    if ( event > 0 )
	m_tree->Fill();
    event++;
    return StatusCode::SUCCESS;
}

StatusCode TrigHLTBaseTreeTool::finalHists() {
    m_tree->Fill();
    return StatusCode::SUCCESS;
}
