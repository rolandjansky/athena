/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// Filter according to the following steps:
// Require four leptons, total charge zero, at least one SFOS pair
// Determine SFOS pair with mass closest to Z mass peak
//   This pair is the Z-candidate pair, the other pair is the H-candidate pair
// Cut on H-candidate and Z-candidate pairs invariant masses according to input options
//
// Designed for the Z(H->WW)->lllvlv analysis
//
// AuthorList:
//         A Liblong: Nov 2016

#include "GeneratorFilters/ZHWWLeptonPairMassFilter.h"

#include "TruthUtils/PIDHelpers.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include <math.h>
#include <vector>
#include <TLorentzVector.h>

using HepMC::GenParticle;

using Particles = std::vector<GenParticle*>;
using LeptonPair = std::pair<GenParticle*, GenParticle*>;

namespace {

const double M_Z = 91187.6;
const unsigned N_LEPTONS = 4;

/// Returns the invariant mass of two particles.
// Why does HepMC::FourVector not have arithmetic methods?
double inv_mass(const GenParticle* part1, const GenParticle* part2)
{
  const auto& p1 = part1->momentum();
  const auto& p2 = part2->momentum();
  auto p_sum = HepMC::FourVector(
      p1.px() + p2.px(),
      p1.py() + p2.py(),
      p1.pz() + p2.pz(),
      p1.e()  + p2.e()
  );
  return p_sum.m();
}

/// Given two unique indices in the set {0,1,2,3}, returns the other two.
std::pair<unsigned, unsigned> getOtherIndices4Lep(unsigned low_index, unsigned high_index)
{
  // Ensure the indices are passed in the right order
  if (high_index < low_index) return getOtherIndices4Lep(high_index, low_index);
  if (low_index == 0) {
    if (high_index == 1) return {2,3};
    else if (high_index == 2) return {1,3};
    else return {1,2};
  }
  else if (low_index == 1) {
    if (high_index == 2) return {0,3};
    else return {0,2};
  }
  else return {0,1};
}

/// Finds the indices in a vector of four leptons of the Z-candidate lepton pair, which is the
/// SFOS pair closest in mass to the Z peak.
std::pair<unsigned, unsigned> findZCandIndices(const Particles& leptons)
{
  // M_diff_min is the minimum difference so far between any M_ij and M_Z
  // Keep track of the indices corresponding to the best Z candidate pair
  // Have to use capital M for mass because of the naming guidelines for class members
  double M_diff_min = std::numeric_limits<double>::max();
  unsigned Z_cand_i = 0;
  unsigned Z_cand_j = 0;

  // Find the Z pair
  for (unsigned i = 0; i < N_LEPTONS-1; ++i) {
    for (unsigned j = i+1; j < N_LEPTONS; ++j) {
      const auto lep_i = leptons[i];
      const auto lep_j = leptons[j];
      const auto pid_i = lep_i->pdg_id();
      const auto pid_j = lep_j->pdg_id();
      if (pid_i == -pid_j) {
        auto M = inv_mass(lep_i, lep_j);
        auto M_diff = fabs(M - M_Z);
        if (M_diff < M_diff_min) {
          Z_cand_i = i;
          Z_cand_j = j;
          M_diff_min = M_diff;
        }
      }
    }
  }
  
  return {Z_cand_i, Z_cand_j};
}

}

ZHWWLeptonPairMassFilter::ZHWWLeptonPairMassFilter(const std::string& alg_name, ISvcLocator* pSvcLocator)
  : GenFilter(alg_name, pSvcLocator),
    m_log(messageService(), name())
{
  declareProperty("H_cand_mass_cut_upper", m_H_cand_mass_cut_upper = 75000); 
  declareProperty("H_cand_mass_cut_lower", m_H_cand_mass_cut_lower = 5000); 
  declareProperty("Z_cand_mass_max_distance_from_peak", m_Z_cand_mass_max_distance_from_peak = 15000); 
}

ZHWWLeptonPairMassFilter::~ZHWWLeptonPairMassFilter() {}

StatusCode ZHWWLeptonPairMassFilter::filterInitialize()
{
  return StatusCode::SUCCESS;
}

StatusCode ZHWWLeptonPairMassFilter::filterFinalize()
{
 return StatusCode::SUCCESS;
}

StatusCode ZHWWLeptonPairMassFilter::filterEvent()
{
  // Require four prompt leptons
  auto leptons = getPromptLeptons();
  if (leptons.size() != 4) {
    setFilterPassed(false);
    m_log << MSG::DEBUG << "Wrong number of prompt leptons. n_leptons == " << leptons.size() << endreq;
    return StatusCode::SUCCESS;
  }

  // Find the Z-candidate pair
  // The other pair is the H-candidate pair
  unsigned Z_cand_i, Z_cand_j;
  std::tie(Z_cand_i, Z_cand_j) = findZCandIndices(leptons);
  if (Z_cand_i == Z_cand_j) { // implying there was no SFOS pair
    setFilterPassed(false);
    m_log << MSG::DEBUG << "No SFOS pairs present" << endreq;
    return StatusCode::SUCCESS;
  }
  unsigned H_cand_i, H_cand_j;
  std::tie(H_cand_i, H_cand_j) = getOtherIndices4Lep(Z_cand_i, Z_cand_j);

  // Find the lepton pair masses
  auto H_pair = LeptonPair{leptons[H_cand_i], leptons[H_cand_j]};
  auto H_mass = inv_mass(H_pair.first, H_pair.second);
  auto Z_pair = LeptonPair{leptons[Z_cand_i], leptons[Z_cand_j]};
  auto Z_mass = inv_mass(Z_pair.first, Z_pair.second);

  m_log << MSG::DEBUG << "Flavours: " << H_pair.first->pdg_id() << ", " << H_pair.second->pdg_id()
      << ", " << Z_pair.first->pdg_id() << ", " << Z_pair.second->pdg_id() << endreq;
  m_log << MSG::DEBUG << "H_mass == " << H_mass << "; Z_mass == " << Z_mass << endreq;

  // Cut on the masses 
  if (H_mass < m_H_cand_mass_cut_lower || m_H_cand_mass_cut_upper < H_mass) {
    setFilterPassed(false);
    m_log << MSG::DEBUG << "H_mass outside of cut bounds." << H_mass << endreq;
    return StatusCode::SUCCESS;
  }
  if (fabs(Z_mass - M_Z) > m_Z_cand_mass_max_distance_from_peak) {
    setFilterPassed(false);
    m_log << MSG::DEBUG << "Z_mass outside of cut bounds." << H_mass << endreq;
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::SUCCESS;
}

/// Retrieve all prompt electrons and muons
Particles ZHWWLeptonPairMassFilter::getPromptLeptons()
{
  // Loop over all events in McEventCollection, then all particles in the event
  Particles leptons;
  for (auto event_itr = events_const()->begin(); event_itr!=events_const()->end(); ++event_itr) {
    const auto event = (*event_itr);
    for (auto pitr = event->particles_begin(); pitr != event->particles_end(); ++pitr) {
      const auto particle = *pitr;

      // check final-state particles only
      if ( particle->status() !=1 ) continue;

      // do not consider taus
      auto abs_pid = abs(particle->pdg_id());
      if (!(abs_pid ==  11 || abs_pid ==  13)) continue;

      if (!isPrompt(particle)) continue;

      leptons.push_back(particle);
    }
  }
  return leptons;
}

/// Checks if particle is prompt (:= has no hadronic parents)
// This has a generic structure because at one point I was checking for tau parents also
bool ZHWWLeptonPairMassFilter::isPrompt(const GenParticle* part)
{
  m_log << MSG::DEBUG << "Checking promptness" << endreq;
  auto orphaned = !part->production_vertex();
  if (orphaned) return false;
  auto has_hadronic_parent = hasParentThat(
      part,
      [] (const GenParticle* parent) {
        return MC::PID::isHadron(parent->pdg_id());
      }
  );
  return !has_hadronic_parent;
}

/// Checks if a particle has a parent satisfying an arbitrary criterium
bool ZHWWLeptonPairMassFilter::hasParentThat(
    const GenParticle* part,
    std::function<bool(const GenParticle*)> has_parent_predicate
)
{
  // Copied more or less from the Rivet equivalent
  const auto vertex = part->production_vertex();
  if (!vertex) {
    m_log << MSG::DEBUG << "Absent production vertex" << endreq;
    return false; //Not sure what an absent production vertex implies
  }

  auto parents_begin = vertex->particles_in_const_begin();
  auto parents_end = vertex->particles_in_const_end();
  const auto beams = vertex->parent_event()->beam_particles();
  // This is just to capture log for debugging
  auto& log = m_log;

  // Checks if such a parent exists
  auto matching_parent_itr = std::find_if(parents_begin, parents_end,
      [&has_parent_predicate, &beams, &log](const GenParticle* parent) {
        log << MSG::DEBUG << "Parent PDG ID == " << parent->pdg_id() << endreq;
        if (parent->status() != 2) {
          log << MSG::DEBUG << "Parent status == " << parent->status() << endreq;
          return false;
        }

        if (parent == beams.first || parent == beams.second) {
          log << MSG::DEBUG << "Parent is beam" << endreq;
          return false;
        }

        return has_parent_predicate(parent);
      }
  );

  return matching_parent_itr != parents_end;
}
