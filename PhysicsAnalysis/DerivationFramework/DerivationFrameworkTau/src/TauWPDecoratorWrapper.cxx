/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauWPDecoratorWrapper.cxx
// Author: Guillermo Hamity (ghamity@cern.ch)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTau/TauWPDecoratorWrapper.h"
#include "tauRecTools/TauWPDecorator.h"
#include "StoreGate/ReadHandle.h"
#include "xAODCore/ShallowCopy.h"

namespace DerivationFramework {

  TauWPDecoratorWrapper::TauWPDecoratorWrapper(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_tTauWPDecorator("tauRecTools::TauWPDecorator")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TauWPDecorator", m_tTauWPDecorator);
  }

  StatusCode TauWPDecoratorWrapper::initialize()
  {
    // initialize WP decorator tool
    ATH_CHECK( m_tTauWPDecorator.retrieve() );

    // retrieve its properties
    StringProperty newScoreName("NewScoreName", "");
    ATH_CHECK( m_tTauWPDecorator->getProperty(&newScoreName) );
    m_scoreNameTrans = newScoreName.value();

    StringArrayProperty decorWPNames("DecorWPNames", {});
    ATH_CHECK( m_tTauWPDecorator->getProperty(&decorWPNames) );
    m_decorWPs = decorWPNames.value();

    // initialize read handle key
    ATH_CHECK( m_tauContainerKey.initialize() );
    
    return StatusCode::SUCCESS;
  }

  StatusCode TauWPDecoratorWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TauWPDecoratorWrapper::addBranches() const
  {
    // retrieve tau container
    SG::ReadHandle<xAOD::TauJetContainer> tauJetsReadHandle(m_tauContainerKey);
    if (!tauJetsReadHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << tauJetsReadHandle.key());
      return StatusCode::FAILURE;
    }
    const xAOD::TauJetContainer* tauContainer = tauJetsReadHandle.cptr();

    // in a DAOD_PHYS->DAOD_PHYSLITE workflow, the new WP might have been decorated in a previous AOD->DAOD_PHYS step
    // therefore, we must check whether the decoration already exists 
    if (!tauContainer->empty()) {
      if (!m_isDecorAvailable.isValid()) {
	bool avail = tauContainer->at(0)->isAvailable<float>(m_scoreNameTrans) ;
	m_isDecorAvailable.set(avail);
      }
    }
    else {
      return StatusCode::SUCCESS;
    }
    // if the decoration exists, do nothing
    if (*m_isDecorAvailable.ptr()) {
      return StatusCode::SUCCESS;
    }

    // create shallow copy
    auto shallowCopy = xAOD::shallowCopyContainer (*tauContainer);
    
    // this variable is removed from the AOD content and must be redecorated
    // unfortunately its recalculation is not robust against charged track thinning
    static const SG::AuxElement::Accessor<float> acc_absEtaLead("ABS_ETA_LEAD_TRACK");

    for (auto tau : *shallowCopy.first) {
      float absEtaLead = -1111.;
      if(tau->nTracks() > 0) {
	const xAOD::TrackParticle* track = tau->track(0)->track();
	absEtaLead = std::abs( track->eta() );
      }
      acc_absEtaLead(*tau) = absEtaLead;
      
      // pass the shallow copy to the WP decorator tool
      ATH_CHECK( m_tTauWPDecorator->execute(*tau) );

      // copy over relevant decorations (flattened score and working points)
      const xAOD::TauJet* xTau = tauContainer->at(tau->index());
      // FIXME: could be replaced with WriteDecorHandle later
      xTau->auxdecor<float>(m_scoreNameTrans) = tau->auxdataConst< float >(m_scoreNameTrans);
      for (const std::string& decvar : m_decorWPs) {
	xTau->auxdecor<char>(decvar) = tau->auxdataConst<char>(decvar);
      }
    }

    delete shallowCopy.first;
    delete shallowCopy.second;

    return StatusCode::SUCCESS;
  }
}
