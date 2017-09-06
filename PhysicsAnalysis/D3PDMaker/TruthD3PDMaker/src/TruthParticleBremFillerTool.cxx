/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticleBremFillerTool.cxx 604362 2014-07-01 05:25:22Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/TruthParticleBremFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Fill the hasHardBrem flag.
 */


#include "TruthParticleBremFillerTool.h"
#include "xAODTruth/TruthVertex.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleBremFillerTool::TruthParticleBremFillerTool
   (const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : BlockFillerTool<xAOD::TruthParticle> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthParticleBremFillerTool::book()
{
  CHECK( addVariable ("hasHardBrem", m_hasHardBrem) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TruthParticleBremFillerTool::fill (const xAOD::TruthParticle& p)
{
  const xAOD::TruthVertex* vx = p.decayVtx();
  if (vx) {
    size_t sz = vx->nOutgoingParticles();
    int n_children = 0;
    int pdgid_child1 = 0;
    int pdgid_child2 = 0;

    for (size_t i = 0; i < sz; i++) {
      const xAOD::TruthParticle* child = vx->outgoingParticle(i);
      if (child) {
        if(n_children==0) pdgid_child1 = child->pdgId();
        if(n_children==1) pdgid_child2 = child->pdgId();
        n_children++;
      }
    }

    if (n_children   == 2  &&
        ((std::abs(pdgid_child1)==11  && pdgid_child2==22) ||
         (std::abs(pdgid_child2)==11  && pdgid_child1==22)))
    {
      *m_hasHardBrem = true;
    }
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
