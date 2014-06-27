/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/TruthParticleFilterTool.cxx
 * @author Mike Hance <mike.hance@cern.ch>
 * @date August 2010
 * @brief Filter truth particles for writing to D3PD.
 */


#include "TruthParticleFilterCutTool.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "boost/foreach.hpp"
#include <utility>


using CLHEP::GeV;


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleFilterCutTool::TruthParticleFilterCutTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : AthAlgTool (type, name, parent)
{
  declareProperty ("PDG_IDs",
		   m_pdgIDs,
		   "Types of particles to keep");

  declareProperty ("pT_low",
		   m_cut_lower_pT = -1,
		   "Lower pT cut");
  
  declareProperty ("pT_high",
		   m_cut_upper_pT = -1,
		   "Upper pT cut");

  declareProperty ("eta_low",
		   m_cut_lower_eta = 0,
		   "Lower eta cut");

  declareProperty ("eta_high",
		   m_cut_upper_eta = -1,
		   "Upper eta cut");
}


/**
 * @brief Standard Gaudi @c queryInterface method.
 */
StatusCode
TruthParticleFilterCutTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITruthParticleFilterCutTool::interfaceID() )  {
    *ppvIf = static_cast<ITruthParticleFilterCutTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }
  
  return AlgTool::queryInterface( riid, ppvIf );
}


/**
 * @brief Check the input PDG against the list of PDG IDs passed into the constructor
 */
bool TruthParticleFilterCutTool::check_PDG(unsigned int pdg_id){
  if(m_pdgIDs.size()==0) 
    return true;

  for(unsigned int i=0; i<m_pdgIDs.size(); i++){
    if(abs(m_pdgIDs[i]) == abs(pdg_id)) return true;
  }
  return false;
}

  
/**
 * @brief Test to see if we want to keep a particle.
 */
bool
TruthParticleFilterCutTool::passesCut (const HepMC::GenParticle* p)
{
  int pdg_id = std::abs (p->pdg_id());
  // Avoid problems evaluating eta().
  if (p->momentum().perp() < 1e-3*GeV) return false;
  float eta  = fabs(p->momentum().eta());
  float pT   = p->momentum().e()/cosh(eta);

  bool local_cut_results = true;
  {
    local_cut_results &= (check_PDG(pdg_id));
    local_cut_results &= (pT > m_cut_lower_pT || m_cut_lower_pT < 0);
    local_cut_results &= (pT < m_cut_upper_pT || m_cut_upper_pT < 0);
    local_cut_results &= (eta > m_cut_lower_eta || m_cut_lower_eta < 0);
    local_cut_results &= (eta < m_cut_upper_eta || m_cut_upper_eta < 0);
  }

  return local_cut_results;
}

/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TruthParticleFilterCutTool::initialize()
{
  return AthAlgTool::initialize();
}


/**
 * @brief Standard Gaudi finalize method.
 */
StatusCode TruthParticleFilterCutTool::finalize()
{
  return AthAlgTool::finalize();
}

/**
 * @brief Standard Gaudi execute method.
 */
StatusCode TruthParticleFilterCutTool::execute()
{
  return StatusCode::SUCCESS;
}




} // namespace D3PD
