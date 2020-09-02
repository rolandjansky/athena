/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleFillerTool.cxx 564177 2013-10-04 12:08:34Z dhayden $
/**
 * @file EventCommonD3PDMaker/src/GenParticleFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for a HepMC GenParticle.
 */


#include "GenParticleFillerTool.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/SimpleVector.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GenParticleFillerTool::GenParticleFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : BlockFillerTool<HepMC::GenParticle> (type, name, parent)
{
  // Avoid coverity warnings
  m_do_E = true;
  m_do_p = true;
  m_do_pt = true;
  m_do_m = true;
  m_do_mothertype = true;
  m_do_motherbarcode = true;
  m_do_px = true;
  m_do_py = true;
  m_do_pz = true;
  book().ignore();

  declareProperty ("WriteE",        m_do_E        = false);
  declareProperty ("WriteP",        m_do_p        = false);
  declareProperty ("WritePt",       m_do_pt       = true);
  declareProperty ("WriteM",        m_do_m        = true);
  declareProperty ("WriteMotherType",    m_do_mothertype     = true);
  declareProperty ("WriteMotherBarcode", m_do_motherbarcode  = true);
  declareProperty ("WritePx",        m_do_px        = false);
  declareProperty ("WritePy",        m_do_py        = false);
  declareProperty ("WritePz",        m_do_pz        = false);
}


/**
 * @brief Book variables for this block.
 */
StatusCode GenParticleFillerTool::book()
{
  if (m_do_E)        CHECK( addVariable ("E",     m_E)  );
  if (m_do_p)        CHECK( addVariable ("p",     m_p)  );
  if (m_do_pt)       CHECK( addVariable ("pt",    m_pt)  );
  if (m_do_m)        CHECK( addVariable ("m",     m_m)  );
  if (m_do_px)       CHECK( addVariable ("px",     m_px)  );
  if (m_do_py)       CHECK( addVariable ("py",     m_py)  );
  if (m_do_pz)       CHECK( addVariable ("pz",     m_pz)  );

  CHECK( addVariable ("eta", m_eta) );
  CHECK( addVariable ("phi", m_phi) );

  CHECK( addVariable ("type",          m_type) );
  CHECK( addVariable ("status",        m_status) );
  CHECK( addVariable ("barcode",       m_barcode) );
  if(m_do_mothertype) CHECK( addVariable ("mothertype",    m_mothertype) );
  if(m_do_motherbarcode) CHECK( addVariable ("motherbarcode", m_motherbarcode) );

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
StatusCode GenParticleFillerTool::fill (const HepMC::GenParticle& p)
{
  HepMC::FourVector v = p.momentum();
  if (m_do_E)        *m_E     = static_cast<float> (v.e());
  if (m_do_p)        *m_p     = static_cast<float> (v.rho());
  if (m_do_pt)       *m_pt    = static_cast<float> (v.perp());
  if (m_do_m)        *m_m     = static_cast<float> (v.m());
  if (m_do_px)       *m_px    = static_cast<float> (v.px());
  if (m_do_py)       *m_py    = static_cast<float> (v.py());
  if (m_do_pz)       *m_pz    = static_cast<float> (v.pz());

  *m_eta = static_cast<float> (v.eta());
  *m_phi = static_cast<float> (v.phi());

  *m_type = p.pdg_id();
  *m_status = p.status();
  *m_barcode = p.barcode();

  if (const HepMC::GenVertex* vprod = p.production_vertex()) {
    if (vprod->particles_in_size() > 0) {
      const HepMC::GenParticle* mother = *vprod->particles_in_const_begin();
      if (mother) {
        if(m_do_mothertype) *m_mothertype = mother->pdg_id();
	if(m_do_motherbarcode) *m_motherbarcode = mother->barcode();
      }
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
