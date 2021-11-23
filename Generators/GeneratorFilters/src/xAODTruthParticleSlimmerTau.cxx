/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthLinks/ElementLink.h"

#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "EventInfo/EventStreamInfo.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"

#include "GeneratorFilters/xAODTruthParticleSlimmerTau.h"

using namespace std;

xAODTruthParticleSlimmerTau::xAODTruthParticleSlimmerTau(const string &name, ISvcLocator *svcLoc)
    : AthAlgorithm(name, svcLoc), m_classifier("MCTruthClassifier/MCTruthClassifier")
{
  declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName = "TruthParticles");
  declareProperty("xAODTruthTauParticleContainerName", m_xaodTruthTauParticleContainerName = "TruthTaus");
  declareProperty("ForceRerun", m_forceRerun = false);
  declareProperty("tau_pt_selection", m_tau_pt_selection = 1. * Gaudi::Units::GeV); //User provides units in MeV!
  declareProperty("abseta_selection", m_abseta_selection = 4.5);
}

StatusCode xAODTruthParticleSlimmerTau::initialize()
{
  ATH_CHECK(m_classifier.retrieve());

  ATH_MSG_INFO("xAOD input TruthParticleContainer name = " << m_xaodTruthParticleContainerName);
  ATH_MSG_INFO("xAOD output TruthTauParticleContainer name = " << m_xaodTruthTauParticleContainerName);
  return StatusCode::SUCCESS;
}

CLHEP::HepLorentzVector xAODTruthParticleSlimmerTau::sumDaughterNeutrinos(const xAOD::TruthParticle *part)
{
  CLHEP::HepLorentzVector nu(0, 0, 0, 0);
  if (((std::abs(part->pdgId()) == 12) || (std::abs(part->pdgId()) == 14) || (std::abs(part->pdgId()) == 16)) && part->status() != 3)
  {
    nu.setPx(part->px());
    nu.setPy(part->py());
    nu.setPz(part->pz());
    nu.setE(part->e());
  }
  if (!part->hasDecayVtx())
    return nu;

  for (size_t n = 0; n < part->nChildren(); ++n)
    nu += sumDaughterNeutrinos(part->child(n));

  return nu;
}

StatusCode xAODTruthParticleSlimmerTau::execute()
{

  CLHEP::HepLorentzVector nutau;

  // If the containers already exists then assume that nothing needs to be done
  if (evtStore()->contains<xAOD::TruthParticleContainer>(m_xaodTruthTauParticleContainerName) &&
      !m_forceRerun)
  {
    ATH_MSG_WARNING("xAOD Tau Truth Particles are already available in the event");
    return StatusCode::SUCCESS;
  }

  // Create new output container
  xAOD::TruthParticleContainer *xTruthTauParticleContainer = new xAOD::TruthParticleContainer();
  CHECK(evtStore()->record(xTruthTauParticleContainer, m_xaodTruthTauParticleContainerName));
  xAOD::TruthParticleAuxContainer *xTruthTauParticleAuxContainer = new xAOD::TruthParticleAuxContainer();
  CHECK(evtStore()->record(xTruthTauParticleAuxContainer, m_xaodTruthTauParticleContainerName + "Aux."));
  xTruthTauParticleContainer->setStore(xTruthTauParticleAuxContainer);
  ATH_MSG_INFO("Recorded TruthTauParticleContainer with key: " << m_xaodTruthTauParticleContainerName);

  // Retrieve full TruthParticle container
  const xAOD::TruthParticleContainer *xTruthParticleContainer;
  if (evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName).isFailure())
  {
    ATH_MSG_ERROR("No TruthParticle collection with name " << m_xaodTruthParticleContainerName << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  // Set up decorators

  const static SG::AuxElement::Decorator<unsigned int> originDecorator("classifierParticleOrigin");
  const static SG::AuxElement::Decorator<unsigned int> typeDecorator("classifierParticleType");
  const static SG::AuxElement::Decorator<unsigned int> outcomeDecorator("classifierParticleOutCome");
  const static SG::AuxElement::Decorator<unsigned int> classificationDecorator("Classification");
  const static SG::AuxElement::Decorator<int> parenthadronPIDDecorator("parentHadronID");

  // sum of neutrinos 4-vector in tau decay products
  const static SG::AuxElement::Decorator<CLHEP::HepLorentzVector> nuDecorator("nuVector");
  const static SG::AuxElement::Decorator<int> tauTypeDecorator("tauType");

  // Loop over full TruthParticle container
  unsigned int nParticles = xTruthParticleContainer->size();
  for (unsigned int iPart = 0; iPart < nParticles; ++iPart)
  {
    ElementLink<xAOD::TruthParticleContainer> eltp(*xTruthParticleContainer, iPart);
    const xAOD::TruthParticle *theParticle = (*xTruthParticleContainer)[iPart];

    int this_absPdgID = theParticle->absPdgId();
    float this_abseta = theParticle->abseta();
    float this_pt = theParticle->pt();
    int this_status = theParticle->status();

    //Save Taus above 1 GeV, & within detector acceptance (4.5)
    // we want to avoid status 3 taus
    if (this_status != 3 && this_absPdgID == 15 && this_pt >= m_tau_pt_selection && this_abseta < m_abseta_selection)
    {
      xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();

      xTruthTauParticleContainer->push_back(xTruthParticle);

      // Fill with numerical content
      xTruthParticle->setPdgId(theParticle->pdgId());
      xTruthParticle->setBarcode(theParticle->barcode());
      xTruthParticle->setStatus(theParticle->status());
      xTruthParticle->setM(theParticle->m());
      xTruthParticle->setPx(theParticle->px());
      xTruthParticle->setPy(theParticle->py());
      xTruthParticle->setPz(theParticle->pz());
      xTruthParticle->setE(theParticle->e());

      const xAOD::TruthParticle *tau = theParticle;
      nutau = sumDaughterNeutrinos(tau);

      nuDecorator(*xTruthParticle) = nutau;

      int tauType = 0;
      for (size_t n = 0; n < tau->nChildren(); ++n)
      {
        if (tau->child(n)->absPdgId() == 12)
          tauType = 1; //Tau decays into an electron
        else if (tau->child(n)->absPdgId() == 14)
          tauType = 2; //Tau decays into a muon
        else if (tau->child(n)->absPdgId() == 15)
          tauType = 11; //Tau radiates a particle and decays into another tau
      }
      tauTypeDecorator(*xTruthParticle) = tauType;

#ifdef MCTRUTHCLASSIFIER_CONST
      IMCTruthClassifier::Info info;
      std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification =
          m_classifier->particleTruthClassifier(theParticle, &info);
      unsigned int particleOutCome = info.particleOutCome;
#else
      std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification =
          m_classifier->particleTruthClassifier(theParticle);
      unsigned int particleOutCome = m_classifier->getParticleOutCome();
#endif

      unsigned int result = (unsigned int)m_classifier->classify(theParticle);

      int hadron_pdg = (int)m_classifier->getParentHadronID(theParticle);

      unsigned int particleType = classification.first;
      unsigned int particleOrigin = classification.second;
      typeDecorator(*xTruthParticle) = particleType;
      originDecorator(*xTruthParticle) = particleOrigin;
      outcomeDecorator(*xTruthParticle) = particleOutCome;

      classificationDecorator(*xTruthParticle) = result;
      parenthadronPIDDecorator(*xTruthParticle) = hadron_pdg;
    }

  } //end of loop over particles

  return StatusCode::SUCCESS;
}
