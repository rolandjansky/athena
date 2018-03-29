/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthIsolationTool.cxx
// Author: Kevin Finelli (kevin.finelli@cern.ch)
// Calculate isolation at truth level for given lists of truth particles

#include "DerivationFrameworkMCTruth/TruthIsolationTool.h"
#include "xAODTruth/TruthEventContainer.h"
#include "HepPID/ParticleIDMethods.hh"
#include <vector>
#include <string>
#include <algorithm>

// Constructor
DerivationFramework::TruthIsolationTool::TruthIsolationTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_coneSizesSort(0),
  m_coneSizes2(0)
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty ("isoParticlesKey",
            m_isoParticlesKey = "TruthParticles",
            "Name of TruthParticle key for input");
    declareProperty ("allParticlesKey",
            m_allParticlesKey = "TruthParticles",
            "Name of Truthparticle key to find in iso cone");
    declareProperty ("IsolationConeSizes", m_coneSizes = std::vector<float>{0.2},
            "Vector of sizes of dR cone in which to include particles");
    declareProperty ("ChargedParticlesOnly", m_chargedOnly = false,
            "Only keep charged particles in isolation cone");
    declareProperty ("particleIDsToCalculate", m_listOfPIDs = std::vector<int>{11,13,22},
            "List of the pdgIDs of particles for which to calculate isolation");
    declareProperty ("excludeIDsFromCone", m_excludeFromCone = std::vector<int>(),
        "List of the pdgIDs of particles to exclude from the cone when calculating isolation");
    declareProperty ("IsolationVarNamePrefix", m_isoVarNamePrefix,
            "Prefix of name of the variable to add to output xAOD");

    m_coneSizes2 = new std::vector<float>();
    m_coneSizesSort = new std::vector<float>();
}

// Destructor
DerivationFramework::TruthIsolationTool::~TruthIsolationTool() {
  delete m_coneSizes2;
  delete m_coneSizesSort;
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthIsolationTool::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthIsolationTool::finalize()
{
    ATH_MSG_VERBOSE("finalize() TruthIsolationTool");
    return StatusCode::SUCCESS;
}

// Function to do isolation calc, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthIsolationTool::addBranches() const
{
    // Retrieve the truth collections
    const xAOD::TruthParticleContainer* importedIsoTruthParticles(nullptr);
    if (evtStore()->retrieve(importedIsoTruthParticles, m_isoParticlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_isoParticlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    const xAOD::TruthParticleContainer* importedAllTruthParticles(nullptr);
    if (evtStore()->retrieve(importedAllTruthParticles, m_allParticlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_allParticlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    //sort (descsending) the cone sizes vector to optimize calculation
    m_coneSizesSort->assign(m_coneSizes.begin(), m_coneSizes.end());
    std::sort(m_coneSizesSort->begin(), m_coneSizesSort->end(), [](float a, float b){return a>b;});

    //define the output variables
    std::vector<SG::AuxElement::Decorator< float > > decorators_iso;
    for ( auto csize_itr : *m_coneSizesSort ) {
      std::ostringstream sizess;
      sizess << m_isoVarNamePrefix << (int)((csize_itr)*100.);
      decorators_iso.push_back( SG::AuxElement::Decorator< float >(sizess.str()) );
    }

    //get struct of helper functions
    DerivationFramework::DecayGraphHelper decayHelper;

    // Isolation is applied to selected particles only
    std::vector<const xAOD::TruthParticle*> listOfParticlesForIso;
    decayHelper.constructListOfFinalParticles(importedIsoTruthParticles, listOfParticlesForIso, m_listOfPIDs);

    // Vectors to store particles which will be dressed
    std::vector<const xAOD::TruthParticle*>  candidateParticlesList;

    std::vector<int> emptyList;
    //make a list of all candidate particles that could fall inside the cone of the particle of interest from listOfParticlesForIso
    decayHelper.constructListOfFinalParticles(importedAllTruthParticles, candidateParticlesList, emptyList, true, m_chargedOnly);

    //create the cached conesize^2 vector
    m_coneSizes2->clear();
    for ( auto csize_itr : *m_coneSizesSort ) {
      m_coneSizes2->push_back(csize_itr * csize_itr);
    }

    // Standard particle loop over final state particles of interest
    for (const auto& part : listOfParticlesForIso) {
      std::vector<float> isolationsCalcs(m_coneSizes2->size(), 0.0);
      calcIsos(part, candidateParticlesList, isolationsCalcs);

      for ( unsigned int icone = 0; icone < m_coneSizesSort->size(); ++icone ) {
        decorators_iso.at(icone)(*part) = isolationsCalcs.at(icone);
      }
    }

    return StatusCode::SUCCESS;
}

void DerivationFramework::TruthIsolationTool::calcIsos(const xAOD::TruthParticle* particle,
        const std::vector<const xAOD::TruthParticle*> &candidateParticlesList,
        std::vector<float> &isoCalcs) const
{
    //given a bare TruthParticle particle, calculate the isolation(s) using the
    //particles in candidateParticlesList, filling isoCalcs in order corresponding
    //to the sorted cone sizes

    float part_eta = particle->eta();
    float part_phi = particle->phi();
    for (const auto& cand_part : candidateParticlesList) {
      if (find(m_excludeFromCone.begin(), m_excludeFromCone.end(), cand_part->pdgId()) != m_excludeFromCone.end()) {
        //skip if we find a particle in the exclude list
        continue;
      }
      if (cand_part->barcode() != particle->barcode()) {
        //iteration over sorted cone sizes
        for ( unsigned int icone = 0; icone < m_coneSizes2->size(); ++icone ) {
          if (calculateDeltaR2(cand_part, part_eta, part_phi) < m_coneSizes2->at(icone) ) {
            //sum the transverse momenta
            isoCalcs.at(icone) = isoCalcs.at(icone) + cand_part->pt();
          } else {
            //break out of the loop safely since the cone sizes are sorted descending
            break;
          }
        }
      }
    }
    return;
}

float DerivationFramework::TruthIsolationTool::calculateDeltaR2(const xAOD::IParticle *p1, float eta2, float phi2) const
{
  //calculate dR^2 this way to hopefully do fewer sqrt and TVector3::Pseudorapidity calls
  float phi1 = p1->phi();
  float eta1 = p1->eta();
  float deltaPhi = fabs(phi1-phi2);
  if (deltaPhi>TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
  float deltaPhiSq = deltaPhi * deltaPhi;
  float deltaEtaSq = (eta1-eta2)*(eta1-eta2);
  return deltaPhiSq+deltaEtaSq;
}
