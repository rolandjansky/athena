/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTLAMonitorAlgorithm.h"


TrigTLAMonitorAlgorithm::TrigTLAMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}

TrigTLAMonitorAlgorithm::~TrigTLAMonitorAlgorithm() {}


StatusCode TrigTLAMonitorAlgorithm::initialize() {
  ATH_CHECK( m_jetContainerKey   .initialize() );
  ATH_CHECK( m_pfjetContainerKey .initialize() );
  ATH_CHECK( m_photonContainerKey.initialize() );
  ATH_CHECK( m_muonContainerKey  .initialize() );

  return AthMonitorAlgorithm::initialize();
}

StatusCode TrigTLAMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  //
  // print the trigger chain names 
  std::string chainName;
  
  int size_AllChains = m_allChains.size();
  ATH_MSG_DEBUG(" Size of the AllChains trigger container: " << size_AllChains );
  for (int i =0; i<size_AllChains; i++){
    chainName = m_allChains[i];
    ATH_MSG_DEBUG("  Chain number: " << i << " AllChains Chain Name: " << chainName );
  }

  for ( const std::string& trigName : m_allChains ) {
    // TODO: Test if trigName has fired

    // proceed filling the histogram

    //
    // jets

    SG::ReadHandle<xAOD::JetContainer> jets = SG::makeHandle( m_jetContainerKey, ctx );
    if (! jets.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain JetContainer Collection with name "<< m_jetContainerKey);
      return StatusCode::FAILURE;
    }

    ANA_CHECK(fillParticleHistograms<xAOD::Jet>(jets, "jet", trigName));

    //
    // particle flow jets

    SG::ReadHandle<xAOD::JetContainer> pfjets = SG::makeHandle( m_pfjetContainerKey, ctx );
    if (! jets.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain particle flow JetContainer Collection with name "<< m_pfjetContainerKey);
      return StatusCode::FAILURE;
    }

    ANA_CHECK(fillParticleHistograms<xAOD::Jet>(pfjets, "pfjet", trigName));

    //
    // photons

    SG::ReadHandle<xAOD::PhotonContainer> phs = SG::makeHandle( m_photonContainerKey, ctx );
    if (! phs.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain PhotonContainer Collection with name "<< m_photonContainerKey);
      return StatusCode::FAILURE;
    }

    ATH_CHECK(fillParticleHistograms<xAOD::Photon>(phs, "ph", trigName));

    //
    // muons

    SG::ReadHandle<xAOD::MuonContainer> muons = SG::makeHandle( m_muonContainerKey, ctx );
    if (! muons.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain MuonContainer Collection with name "<< m_muonContainerKey);
      return StatusCode::FAILURE;
    }

    ANA_CHECK(fillParticleHistograms<xAOD::Muon>(muons, "muon", trigName));

    // Delta Angles
    if(jets->size()>=2) {
      ANA_CHECK(fillDeltaRHistograms(jets  ->at(0), jets  ->at(1), "jet0jet1"    , trigName));
    }

    if(pfjets->size()>=2) {
      ANA_CHECK(fillDeltaRHistograms(pfjets->at(0), pfjets->at(1), "pfjet0pfjet1", trigName));
    }

    if(jets->size()>=1 && phs->size()>=1) {
      ANA_CHECK(fillDeltaRHistograms(jets  ->at(0), phs   ->at(0), "jet0ph0"     , trigName));
    }

    if(pfjets->size()>=1 && phs->size()>=1) {
      ANA_CHECK(fillDeltaRHistograms(pfjets->at(0), phs   ->at(0), "pfjet0ph0"   , trigName));
    }

  } // for AllChains

  return StatusCode::SUCCESS;
}

StatusCode TrigTLAMonitorAlgorithm::fillDeltaRHistograms(const xAOD::IParticle* p0, const xAOD::IParticle* p1, const std::string& prefix, const std::string& trigName) const
{
  // histograms
  Monitored::Scalar<double> dr(prefix+"dr_"  +trigName,0.0);

  // fill
  dr = p0->p4().DeltaR(p1->p4());
  fill("TrigTLAMonitor", dr);

  return StatusCode::SUCCESS;
}
