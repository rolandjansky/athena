/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetViewAlg.cxx

#include <memory>
#include "JetViewAlg.h"

//**********************************************************************

// Called before event loop
StatusCode JetViewAlg::initialize() {

  // Initialise DataHandleKeys so that the scheduler is aware of 
  // data dependencies & products
  if(m_input.key().empty() || m_output.key().empty()) {
    ATH_MSG_ERROR("Input/output key(s) is/are blank!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_input.initialize());
  ATH_CHECK(m_output.initialize());

  ATH_MSG_DEBUG("Will select jets from " << m_input.key()
		<< " and write them to " << m_output.key());
  ATH_MSG_DEBUG("Selection criteria are: \n"
		<< "    pT > " << m_ptmin << " MeV"
		<< "    |eta| < " << m_absetamax
		);

  return StatusCode::SUCCESS;
}

//**********************************************************************

bool JetViewAlg::selected(const xAOD::Jet& jet) const {

  if(jet.pt() < m_ptmin) {return false;}
  if(std::abs(jet.eta()) > m_absetamax) {return false;}

  return true;
}

//**********************************************************************

StatusCode JetViewAlg::execute(const EventContext& ctx) const {  

  SG::ReadHandle<xAOD::JetContainer> inputHandle(m_input,ctx);

  // Need to place jets into a CDV because we can't put const objects in a DataVector
  auto selected_jets = std::make_unique<ConstDataVector<xAOD::JetContainer> >( SG::VIEW_ELEMENTS );
  for(const xAOD::Jet* jet : *inputHandle) {
    ATH_MSG_VERBOSE("Jet " << jet->index() << std::setprecision(3)
		    << " with pt " << jet->pt()
		    << " eta " << jet->eta()
		    );
    if( selected(*jet) ) {
      selected_jets->push_back(jet);
      ATH_MSG_VERBOSE("  Passed selection");
    } else {
      ATH_MSG_VERBOSE(" Failed selection");
    }
  }
  ATH_MSG_DEBUG("Selected " << selected_jets->size() << " from input container of size " << inputHandle->size() );

  // Write out JetContainer and JetAuxContainer
  SG::WriteHandle<ConstDataVector<xAOD::JetContainer> > jetContHandle(m_output,ctx);
  ATH_CHECK( jetContHandle.record( std::move(selected_jets) ) );

  return StatusCode::SUCCESS;
}

//**********************************************************************

