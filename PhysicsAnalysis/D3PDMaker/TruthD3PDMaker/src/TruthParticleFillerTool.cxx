/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleFillerTool.cxx
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date Dec, 2009
 * @brief Block filler tool for TruthParticle's.
 */

#include "TruthParticleFillerTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleFillerTool::TruthParticleFillerTool (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : Base (type, name, parent),
    m_ppsvc ("PartPropSvc", name)
{
  declareProperty ("PDGIDVariable",  m_PDGIDVariable = "pdgId");
  declareProperty ("PartPropSvc", m_ppsvc,
                   "Particle property service.");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TruthParticleFillerTool::initialize()
{
  CHECK( m_ppsvc.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthParticleFillerTool::book()
{
  CHECK( addVariable("status",    m_status) );
  CHECK( addVariable("barcode",   m_barcode) );
  CHECK( addVariable(m_PDGIDVariable,     m_pdgId) );
  CHECK( addVariable("charge",    m_charge) );
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
StatusCode TruthParticleFillerTool::fill (const TruthParticle& p)
{
  *m_status = p.status();
  *m_barcode = p.barcode();
  *m_pdgId = p.pdgId();
  *m_charge = p.charge();
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
StatusCode TruthParticleFillerTool::fill (const xAOD::TruthParticle& p)
{
  *m_status = p.status();
  *m_barcode = p.barcode();
  *m_pdgId = p.pdgId();

  const HepPDT::ParticleDataTable* pdt = m_ppsvc->PDT();
  const HepPDT::ParticleData* pd = pdt->particle (std::abs(p.pdgId()));
  *m_charge = pd ? pd->charge() : 0;
  if (p.pdgId() < 0)
    *m_charge = - *m_charge;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
