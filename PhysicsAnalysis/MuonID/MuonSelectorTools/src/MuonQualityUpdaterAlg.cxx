/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonQualityUpdaterAlg.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODCore/ShallowCopy.h"

namespace CP {

  MuonQualityUpdaterAlg::MuonQualityUpdaterAlg(const std::string& name, ISvcLocator* svcloc) : AthAlgorithm(name,svcloc),
											       m_tool("CP::MuonSelectionTool") /*public tool*/ {

    declareProperty("Input",m_input_muons="");
    declareProperty("Output",m_output_muons="");
    declareProperty("Tool",m_tool);

  }


  StatusCode MuonQualityUpdaterAlg::initialize() {
    if(m_input_muons=="") { ATH_MSG_ERROR("You must specify an input muon collection"); return StatusCode::FAILURE; }
    CHECK( m_tool.retrieve() );

    return StatusCode::SUCCESS;
  }


  StatusCode MuonQualityUpdaterAlg::execute() {

    //fetch input collection
    const xAOD::MuonContainer* muons = 0;
    CHECK( evtStore()->retrieve(muons,m_input_muons) );

    // create a shallow copy container for the outputs
    auto OutMuons = xAOD::shallowCopyContainer( *muons );
    CHECK( evtStore()->record( OutMuons.first,  m_output_muons ) );
    CHECK( evtStore()->record( OutMuons.second, m_output_muons + "Aux." ) );

    for ( xAOD::Muon* muon : *(OutMuons.first) ) {
      m_tool->setQuality(*muon);
      m_tool->setPassesHighPtCuts(*muon);
      m_tool->setPassesIDCuts(*muon);
      //m_tool->setPassesLowPtEfficiencyCuts(*muon);
    }

    return StatusCode::SUCCESS;
  }

} //CP namespace
