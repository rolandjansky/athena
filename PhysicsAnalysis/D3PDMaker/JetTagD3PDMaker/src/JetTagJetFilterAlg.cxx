/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetFilterAlg.h"

#include "JetEvent/JetCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"
#include "DataModel/ConstDataVector.h"


namespace D3PD {


JetTagJetFilterAlg::JetTagJetFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("JetContainer",      m_jetContainerName = "" );
  declareProperty("FilteredJetContainer",  m_filteredJetContainerName = "" );
  declareProperty("PtMinCut",  m_ptMin = 5000. );
  declareProperty("EtaCut",  m_etaCut = 3. );
  declareProperty("UseEMScale", m_useEMScale = false);
}

StatusCode JetTagJetFilterAlg::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode JetTagJetFilterAlg::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode JetTagJetFilterAlg::execute() {

  if(""==m_jetContainerName){
    ATH_MSG_ERROR(" jet container name not specified");
    return StatusCode::FAILURE;
  }

  if(""==m_filteredJetContainerName){
    m_filteredJetContainerName = m_jetContainerName+"_JetTagFilter_D3PD";
  }

  const JetCollection *jetcoll = 0;
  CHECK( evtStore()->retrieve(jetcoll, m_jetContainerName ) );


  // output container for muons
  ConstDataVector<JetCollection>*  filteredcoll =
    new ConstDataVector<JetCollection>(SG::VIEW_ELEMENTS);

  JetCollection::const_iterator jetItr = jetcoll->begin();
  JetCollection::const_iterator jetEnd = jetcoll->end();
  for(; jetItr!=jetEnd ; ++jetItr) {
    if(!pass(*jetItr))continue;
    filteredcoll->push_back(*jetItr);
  }
  
  CHECK( evtStore()->record(filteredcoll,  m_filteredJetContainerName) );

  return StatusCode::SUCCESS;

}

bool JetTagJetFilterAlg::pass(const Jet* jet){

  if(m_useEMScale){
    SignalStateHelper t(jet, P4SignalState::JETEMSCALE );
    if(jet->pt() < m_ptMin ) return false;
    if( fabs(jet->eta()) > m_etaCut) return false;
  }
  else{
    if(jet->pt() < m_ptMin ) return false;
    if( fabs(jet->eta()) > m_etaCut) return false;
  }

  /// add additional cuts here

  return true;
	   
}

} // namespace D3PD
