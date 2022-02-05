/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthLinks/ElementLink.h"

#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "TruthUtils/HepMCHelpers.h"
#include "TruthUtils/PIDHelpers.h"

#include "GeneratorFilters/xAODTruthParticleSlimmerMET.h"


xAODTruthParticleSlimmerMET::xAODTruthParticleSlimmerMET(const std::string &name, ISvcLocator *svcLoc)
    : AthAlgorithm(name, svcLoc)
{
    declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName = "TruthParticles");
    declareProperty("xAODTruthParticleContainerNameMET", m_xaodTruthParticleContainerNameMET = "TruthMET");
    declareProperty("xAODTruthEventContainerName", m_xaodTruthEventContainerName = "TruthEvents");
}

StatusCode xAODTruthParticleSlimmerMET::initialize()
{
    ATH_MSG_INFO("xAOD input TruthParticleContainer name = " << m_xaodTruthParticleContainerName);
    ATH_MSG_INFO("xAOD output TruthParticleContainerMET name = " << m_xaodTruthParticleContainerNameMET);
    return StatusCode::SUCCESS;
}

StatusCode xAODTruthParticleSlimmerMET::execute()
{
    // If the containers already exists then assume that nothing needs to be done
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_xaodTruthParticleContainerNameMET))
    {
        ATH_MSG_WARNING("xAOD MET Truth Particles are already available in the event");
        return StatusCode::SUCCESS;
    }

    // Create new output container
    xAOD::TruthParticleContainer *xTruthParticleContainerMET = new xAOD::TruthParticleContainer();
    CHECK(evtStore()->record(xTruthParticleContainerMET, m_xaodTruthParticleContainerNameMET));
    xAOD::TruthParticleAuxContainer *xTruthParticleAuxContainerMET = new xAOD::TruthParticleAuxContainer();
    CHECK(evtStore()->record(xTruthParticleAuxContainerMET, m_xaodTruthParticleContainerNameMET + "Aux."));
    xTruthParticleContainerMET->setStore(xTruthParticleAuxContainerMET);
    ATH_MSG_INFO("Recorded TruthParticleContainerMET with key: " << m_xaodTruthParticleContainerNameMET);

    // Retrieve full TruthParticle container
    const xAOD::TruthParticleContainer *xTruthParticleContainer;
    if (evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName).isFailure())
    {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_xaodTruthParticleContainerName << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
    if (evtStore()->retrieve(xTruthEventContainer, m_xaodTruthEventContainerName).isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name " << m_xaodTruthEventContainerName << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Set up decorators if needed
    const static SG::AuxElement::Decorator<bool> isFromWZDecorator("isFromWZ");
    const static SG::AuxElement::Decorator<bool> isFromTauDecorator("isFromTau");

    // Loop over full TruthParticle container
    xAOD::TruthEventContainer::const_iterator itr;
    for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {

        unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* theParticle =  (*itr)->truthParticle(iPart);

          // stable and non-interacting, implemented from DerivationFramework 
          //https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/python/MCTruthCommon.py#L183
          // which in turn use the implementation from Reconstruction
          //https://gitlab.cern.ch/atlas/athena/blob/21.0/Reconstruction/MET/METReconstruction/Root/METTruthTool.cxx#L143
          if (!MC::isGenStable(theParticle->status(),theParticle->barcode())) continue;
          if (!MC::isNonInteracting(theParticle->pdgId())) continue;


          xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();
          xTruthParticleContainerMET->push_back( xTruthParticle );

          // Fill with numerical content
          xTruthParticle->setPdgId(theParticle->pdgId());
          xTruthParticle->setBarcode(theParticle->barcode());
          xTruthParticle->setStatus(theParticle->status());
          xTruthParticle->setM(theParticle->m());
          xTruthParticle->setPx(theParticle->px());
          xTruthParticle->setPy(theParticle->py());
          xTruthParticle->setPz(theParticle->pz());
          xTruthParticle->setE(theParticle->e());

          //Decorate
          isFromWZDecorator(*xTruthParticle) = fromWZ(theParticle);
          isFromTauDecorator(*xTruthParticle) = fromTau(theParticle);
        
        }
    }

    return StatusCode::SUCCESS;
}

bool xAODTruthParticleSlimmerMET::fromWZ( const xAOD::TruthParticle* part ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function
  //  it's used in place of code in MCTruthClassifier as long as this package is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Hit a hadron -> return false
  // Hit a parton -> return true
  //   This catch is important - we *cannot* look explicitly for the W or Z, because some
  //    generators do not include the W or Z in the truth record (like Sherpa)
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!part->hasProdVtx()) return false;

  unsigned int nIncomingParticles = part->prodVtx()->nIncomingParticles();
  for (unsigned int iPart = 0; iPart<nIncomingParticles; iPart++)
  {
    const xAOD::TruthParticle* incoming_particle = part->prodVtx()->incomingParticle(iPart); 
    int parent_pdgid = incoming_particle->pdgId();
    if (MC::PID::isW(parent_pdgid) || MC::PID::isZ(parent_pdgid)) return true;
    if (MC::PID::isHadron( parent_pdgid ) ) return false;
    if ( std::abs( parent_pdgid ) < 9 ) return true;
    if ( parent_pdgid == part->pdgId() ) return fromWZ( incoming_particle );
  }
  return false;
}

bool xAODTruthParticleSlimmerMET::fromTau( const xAOD::TruthParticle* part ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function
  //  it's used in place of code in MCTruthClassifier as long as this package is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Find a tau -> return true
  // Find a hadron or parton -> return false
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!part->hasProdVtx()) return false;

  unsigned int nIncomingParticles = part->prodVtx()->nIncomingParticles();
  for (unsigned int iPart = 0; iPart<nIncomingParticles; iPart++)
  {
    const xAOD::TruthParticle* incoming_particle = part->prodVtx()->incomingParticle(iPart); 
    int parent_pdgid = incoming_particle->pdgId();
    if ( std::abs( parent_pdgid ) == 15  && fromWZ(incoming_particle)) return true;
    if (MC::PID::isHadron( parent_pdgid ) || std::abs( parent_pdgid ) < 9 ) return false;
    if ( parent_pdgid == incoming_particle->pdgId() ) return fromTau( incoming_particle );
  }
  return false;
}
