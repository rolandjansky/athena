/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiTauMassDecorator.cxx
// Author: Nadav Tamir (nadavtamir@mail.tau.ac.il)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTau/DiTauMassDecorator.h"
#include "xAODTracking/Vertex.h"
#include "xAODTau/DiTauJetContainer.h"

namespace DerivationFramework {

  DiTauMassDecorator::DiTauMassDecorator(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_ditauContainerName("DiTauJets")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("DiTauContainerName", m_ditauContainerName);
  }

  StatusCode DiTauMassDecorator::addBranches() const
  {
    // retrieve container
    const xAOD::DiTauJetContainer* xDTContainer = evtStore()->retrieve< const xAOD::DiTauJetContainer >( m_ditauContainerName );
    if( ! xDTContainer ) {
      ATH_MSG_ERROR ("Couldn't retrieve di-tau container with key: " << m_ditauContainerName );
      return StatusCode::FAILURE;
    }

    for(auto xDT : *xDTContainer) {
      TLorentzVector tlvLead = TLorentzVector();
      TLorentzVector tlvSubLead = TLorentzVector();
      tlvLead.SetPtEtaPhiE(xDT->subjetPt(0), xDT->subjetEta(0), xDT->subjetPhi(0), xDT->subjetE(0));
      if(xDT->nSubjets() >=2) {
	tlvSubLead.SetPtEtaPhiE(xDT->subjetPt(1), xDT->subjetEta(1), xDT->subjetPhi(1), xDT->subjetE(1));
      }
      float DTmass = (tlvLead+tlvSubLead).M();
      SG::AuxElement::Decorator< float > LSLMassDeco( "LSLMass" );
      LSLMassDeco(*xDT) = DTmass;
    }

    return StatusCode::SUCCESS;
  }
}
