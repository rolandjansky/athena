/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDAnalysis/src/TruthParticleFilterTool.cxx
 * @author scott snyder <snyder@bnl.gov>, based on code from Maarten Boonekamp.
 * @date Dec, 2009
 * @brief Filter truth particles for writing to D3PD.
 */


#include "TruthParticleFilterTool.h"
#include "EventKernel/PdtPdg.h"
#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "HepPID/ParticleIDMethods.hh"
#include "GaudiKernel/SystemOfUnits.h"
#include <algorithm>


namespace {


const int PARTONPDGMAX = 43;
const int NPPDGMIN = 1000000;
const int NPPDGMAX = 8999999;
const int PHOTOSMIN = 10000;
const int GEANTMIN = 200000;


} // anonymous namespace


namespace D3PD {


using Gaudi::Units::GeV;


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleFilterTool::TruthParticleFilterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TruthParticleFilterBase (type, name, parent)
    , m_particleCountSoFar(0)
{
  m_barcode_trace.reserve(10);
  declareProperty ("WritePartons",
                   m_writePartons = true,
                   "Keep partons?");

  declareProperty ("WriteHadrons",
                   m_writeHadrons = false,
                   "Keep hadrons?");

  declareProperty ("WriteBHadrons",
                   m_writeBHadrons = true,
                   "Keep b-hadrons?");

  declareProperty ("WriteGeant",
                   m_writeGeant = false,
                   "Keep Geant particles?");

  declareProperty ("GeantPhotonPtThresh",
                   m_geantPhotonPtThresh = 0.5*GeV,
                   "Write Geant photons with Pt above this threshold.  "
                   "Set to < 0 to not write any.");

  declareProperty ("WriteTauHad",
                   m_writeTauHad = false,
                   "Keep hadronic taus?");

  declareProperty ("PartonPtThresh",
                   m_partonPtThresh = -1,
                   "Write partons with Pt above this threshold.");

  declareProperty ("WriteBSM",
                   m_writeBSM = false,
                   "Keep BSM particles?");

  declareProperty ("WriteBosons",
                   m_writeBosons = false,
                   "Keep bosons?");

  declareProperty ("WriteBSMProducts",
                   m_writeBSMProducts = false,
                   "Keep BSM particle decay products?");

  declareProperty ("WriteBosonProducts",
                   m_writeBosonProducts = false,
                   "Keep boson decay products?");

  declareProperty ("WriteTopAndDecays",
                   m_writeTopAndDecays = false,
                   "Keep top partons and immediate decay products?");

  declareProperty("WriteEverything",
                  m_writeEverything = false,
                  "Keep absolutely everything (overrides all other flags)");

  declareProperty("WriteAllLeptons",
                  m_writeAllLeptons = false,
                  "Keep absolutely all leptons");

  declareProperty("WriteStatus3",
                  m_writeStatus3 = false,
                  "Save all particles with status code 3");

  declareProperty("WriteFirstN",
                  m_writeFirstN = -1,
                  "Keep first N particles in record");
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode TruthParticleFilterTool::execute()
{
  m_particleCountSoFar = 0;

  CHECK( TruthParticleFilterBase::execute() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Test to see if we want to keep a particle.
 */
bool
TruthParticleFilterTool::isAccepted (const HepMC::GenParticle* p)
{
  bool ok = false;

  int pdg_id = std::abs (p->pdg_id());
  int barcode = p->barcode();

  // Keep first N particles
  if (m_particleCountSoFar<m_writeFirstN){
    m_particleCountSoFar++;
    ok=true;
  }

  if (p->barcode() > GEANTMIN && !m_writeGeant && !m_writeEverything && !ok) {
    if (! (pdg_id == PDG::gamma &&
           m_geantPhotonPtThresh >= 0 &&
           p->momentum().perp() > m_geantPhotonPtThresh) )
      return false;
  }

  // Do we want to save everything?
  if (m_writeEverything)
    ok = true;

  // Save status code 3 particles
  if (m_writeStatus3 && p->status()==3)
    ok = true;

  // OK if we select partons and are at beginning of event record
  if( m_writePartons &&
      (pdg_id <= PARTONPDGMAX || (pdg_id >= NPPDGMIN && pdg_id <= NPPDGMAX) ) &&
      (m_partonPtThresh<0 || p->momentum().perp()>m_partonPtThresh) )
    ok = true;

  //  OK if we should select hadrons and are in hadron range 
  if( m_writeHadrons && HepPID::isHadron (pdg_id) && barcode < PHOTOSMIN )
    ok = true;

  // OK if we should select b hadrons and are in hadron range 
  if( m_writeBHadrons && barcode < PHOTOSMIN && HepPID::isHadron (pdg_id) && HepPID::hasBottom (pdg_id) )
    ok= true;

  // PHOTOS range: check whether photons come from parton range or 
  // hadron range
  int motherPDGID = 999999999;
  if( barcode > PHOTOSMIN && barcode < GEANTMIN &&
      p->production_vertex() )
  {
    const HepMC::GenVertex* vprod = p->production_vertex();
    if (vprod->particles_in_size() > 0) {
      const HepMC::GenParticle* mother = *vprod->particles_in_const_begin();
      if (mother) motherPDGID = mother->pdg_id();
    }
    if( m_writePartons && !HepPID::isHadron( motherPDGID ) )
      ok = true;
    if( m_writeHadrons && HepPID::isHadron( motherPDGID ) )
      ok = true;
  }

  // OK if we should select G4 particles and are in G4 range
  if( m_writeGeant && barcode > GEANTMIN )
    ok = true;

  if(isLeptonFromTau(p)) 
    ok = true;

  if(isFsrFromLepton(p))
    ok = true;

  // Hadronic tau decays
  if(m_writeTauHad){
    m_barcode_trace.clear();
    if (isFromTau(p))
      ok = true;
  }

  // Bosons
  if(m_writeBosons && isBoson(p))
    ok = true;

  // BSM particles
  if(m_writeBSM && isBSM(p))
    ok = true;

  // Top quarks
  if(m_writeTopAndDecays && pdg_id==6)
    ok = true;

  // All leptons
  if(m_writeAllLeptons && (pdg_id>10 && pdg_id<19)) // Include 4th generation...
    ok = true;

  if ((m_writeBSMProducts || m_writeBosonProducts || m_writeTopAndDecays) && p->production_vertex()){ // Either way we have to go through parents
    HepMC::GenVertex::particle_iterator itrParent = p->production_vertex()->particles_begin(HepMC::parents);
    HepMC::GenVertex::particle_iterator endParent = p->production_vertex()->particles_end(HepMC::parents);
    for(;itrParent!=endParent; ++itrParent){
      if (!(*itrParent)) continue;
      if ((m_writeBSMProducts && isBSM( (*itrParent) )) ||
          (m_writeBosonProducts && isBoson( (*itrParent) )) ||
          (m_writeTopAndDecays && abs((*itrParent)->pdg_id())==6) ){
        ok = true;
        break;
      }
    }

  }

  return ok;
}

bool TruthParticleFilterTool::isLeptonFromTau(const HepMC::GenParticle* part) const{

  int pdg = part->pdg_id();

  if(abs(pdg) != 11 &&
     abs(pdg) != 12 &&
     abs(pdg) != 13 &&
     abs(pdg) != 14 &&
     abs(pdg) != 15 &&
     abs(pdg) != 16) return false; // all leptons including tau.
  
  HepMC::GenVertex* prod = part->production_vertex();
  if(!prod) return false; // no parent.

  // Simple loop catch
  if (prod==part->end_vertex()) return false;

  // Loop over the parents of this particle.
  HepMC::GenVertex::particle_iterator itrParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;itrParent!=endParent; ++itrParent){
    int parentId = (*itrParent)->pdg_id();
    if(abs(parentId) == 15) {
      ATH_MSG_DEBUG("Particle with pdgId = " << pdg << ", matched to tau");
      return true; // Has tau parent
    }

    if(parentId == pdg) { // Same particle just a different MC status
      // Go up the generator record until a tau is found or not. 
      // Note that this requires a connected *lepton* chain, while calling
      //  isFromTau would allow leptons from hadrons from taus
      if(isLeptonFromTau(*itrParent)) {
        return true;
      }
    }
  }

  return false;
}

bool TruthParticleFilterTool::isFromTau(const HepMC::GenParticle* part) {

  int pdg = part->pdg_id();

  HepMC::GenVertex* prod = part->production_vertex();
  if(!prod) return false; // no parent.

  // Simple loop catch
  if (prod==part->end_vertex()) return false;

  // More complex loop catch
  if ( find(m_barcode_trace.begin(),m_barcode_trace.end(),prod->barcode()) != m_barcode_trace.end()){
    ATH_MSG_DEBUG( "Found a loop (a la Sherpa sample).  Backing out." );
    return false;
  }
  m_barcode_trace.push_back(prod->barcode());

  // Loop over the parents of this particle.
  HepMC::GenVertex::particle_iterator itrParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  int n_iter=0;
  for(;itrParent!=endParent; ++itrParent){
    if (!(*itrParent)) continue;
    n_iter++;
    if (n_iter>2) break; // No point in trying - this vertex does not have a quantum meaning...

    int parentId = (*itrParent)->pdg_id();
    if(abs(parentId) == 15) {
      // Check if one of the children of this parent was a tau - if it is, then it is
      //   photon radiation, and we already cover that under FSR
      bool has_fsr = false;
      if ( (*itrParent)->end_vertex() ){
        for (HepMC::GenVertex::particle_iterator itrChild = (*itrParent)->end_vertex()->particles_begin(HepMC::children);
             itrChild!=(*itrParent)->end_vertex()->particles_end(HepMC::children);++itrChild){
          if (!(*itrChild)) continue;
          if (abs((*itrChild)->pdg_id())==15){
            has_fsr = true;
            break;
          } // Caught FSR check
        } // loop over immediate children
      } // Parent has an end vertex
      if (has_fsr) return false;
      ATH_MSG_DEBUG("Particle with pdgId = " << pdg << ", matched to tau");
      return true; // Has tau parent
    }

    // Go up the generator record until a tau is found or not.
    if(isFromTau(*itrParent)) {
      return true;
    }
  }

  return false;
}

bool TruthParticleFilterTool::isBSM(const HepMC::GenParticle* part) const{

  int pdg = part->pdg_id();

  if ( (31<abs(pdg) && abs(pdg)<38) || // BSM Higgs / W' / Z' / etc
       abs(pdg)==39 ||
       abs(pdg)==41 ||
       abs(pdg)==42 ||
       (1000000<abs(pdg) && abs(pdg)<1000040) || // left-handed SUSY
       (2000000<abs(pdg) && abs(pdg)<2000040) || // right-handed SUSY
       abs(pdg)==7 || abs(pdg)==8 || // 4th Generation
       (abs(pdg)>=9000001 && abs(pdg)<=9000006) ) // Monotop from MadGraph
    return true;

  return false;
}

bool TruthParticleFilterTool::isBoson(const HepMC::GenParticle* part) const{

  int pdg = part->pdg_id();

  if(abs(pdg) != 22  &&
     abs(pdg) != 23 &&
     abs(pdg) != 24 &&
     abs(pdg) != 25 ) return false;

  if(abs(pdg)==22 && part->momentum().perp()<3.*GeV) return false;

  return true;
}

bool TruthParticleFilterTool::isFsrFromLepton(const HepMC::GenParticle* part) const {
  int pdg = part->pdg_id();
  if(abs(pdg) != 22) return false; // photon
  if(part->barcode() >=  200000) return false; // Geant photon

  HepMC::GenVertex* prod = part->production_vertex();
  if(!prod) return false; // no parent.

  // Loop over the parents of this particle.
  HepMC::GenVertex::particle_iterator itrParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;itrParent!=endParent; ++itrParent){
    int parentId = (*itrParent)->pdg_id();
    if(abs(parentId) == 11 || 
       abs(parentId) == 13 ||
       abs(parentId) == 15) {
      ATH_MSG_DEBUG("Photon with barcode " << part->barcode() << " matched to particle with pdgId = " << parentId );
      return true; // Has lepton parent
    }

    if(parentId == pdg) { // Same particle just a different MC status
      
      // Go up the generator record until a tau is found or not.
      if(isFsrFromLepton(*itrParent)) return true;
    }
  }
  
  return false;
}


} // namespace D3PD
