/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventCommonD3PDMaker/src/FourMomFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for a four-momentum.
 */


#include "FourMomFillerTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/I4Momentum.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TLorentzVector.h"
#include <typeinfo>
#include <cmath>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
FourMomFillerTool::FourMomFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : Base (type, name, parent)
{
  m_do_E        = true;
  m_do_p        = true;
  m_do_Et       = true;
  m_do_pt       = true;
  m_do_m        = true;
  m_do_rapidity = true;
  m_do_tanth    = true;
  m_do_etaphi   = true;
  m_do_rect     = true;
  FourMomFillerTool::book().ignore(); // Avoid coverity warnings.

  declareProperty ("WriteE",        m_do_E        = false);
  declareProperty ("WriteP",        m_do_p        = false);
  declareProperty ("WriteEt",       m_do_Et       = false);
  declareProperty ("WritePt",       m_do_pt       = true);
  declareProperty ("WriteM",        m_do_m        = true);
  declareProperty ("WriteRapidity", m_do_rapidity = false);
  declareProperty ("WriteTanTh",    m_do_tanth    = false);
  declareProperty ("WriteEtaPhi",   m_do_etaphi   = true);
  declareProperty ("WriteRect",     m_do_rect     = false);
}


/**
 * @brief Book variables for this block.
 */
StatusCode FourMomFillerTool::book()
{
  if (m_do_E)        CHECK( addVariable ("E",     m_E)  );
  if (m_do_p)        CHECK( addVariable ("p",     m_p)  );
  if (m_do_Et)       CHECK( addVariable ("Et",    m_Et)  );
  if (m_do_pt)       CHECK( addVariable ("pt",    m_pt)  );
  if (m_do_m)        CHECK( addVariable ("m",     m_m)  );
  if (m_do_rapidity) CHECK( addVariable ("y",     m_y)  );
  if (m_do_tanth)    CHECK( addVariable ("tanth", m_tanth)  );

  if (m_do_etaphi) {
    CHECK( addVariable ("eta", m_eta) );
    CHECK( addVariable ("phi", m_phi) );
  }

  if (m_do_rect) {
    CHECK( addVariable ("px",   m_px)  );
    CHECK( addVariable ("py",   m_py)  );
    CHECK( addVariable ("pz",   m_pz)  );
  }

  return StatusCode::SUCCESS;
}


#define FILL_BODY(P, PT, TANTH) \
 do {                                                               \
  if (m_do_E)        *m_E     = static_cast<float> (p.e());         \
  if (m_do_p)        *m_p     = static_cast<float> (P);             \
  if (m_do_Et)       *m_Et    = static_cast<float> (p.et());        \
  if (m_do_pt)       *m_pt    = static_cast<float> (PT);            \
  if (m_do_m)        *m_m     = static_cast<float> (p.m());         \
  if (m_do_rapidity) *m_y     = static_cast<float> (p.rapidity());  \
  if (m_do_tanth)    *m_tanth = static_cast<float> (TANTH);         \
                                                                    \
  if (m_do_etaphi) {                                                \
    *m_eta = static_cast<float> (p.eta());                          \
    *m_phi = static_cast<float> (p.phi());                          \
  }                                                                 \
                                                                    \
  if (m_do_rect) {                                                  \
    *m_px = static_cast<float> (p.px());                            \
    *m_py = static_cast<float> (p.py());                            \
    *m_pz = static_cast<float> (p.pz());                            \
  }                                                                 \
 } while(0)


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 */
StatusCode FourMomFillerTool::fill (const I4Momentum& p)
{
  FILL_BODY(p.p(), p.pt(), p.tanTh());

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 */
StatusCode FourMomFillerTool::fill (const INavigable4Momentum& p)
{
  return fill (static_cast<const I4Momentum&>(p));
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 */
StatusCode FourMomFillerTool::fill (const CLHEP::HepLorentzVector& p)
{
  FILL_BODY(p.mag(), p.perp(), std::tan(p.theta()));

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 */
StatusCode FourMomFillerTool::fill (const xAOD::IParticle& p)
{
  return fill (p.p4());
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 */
StatusCode FourMomFillerTool::fill (const TLorentzVector& p)
{
  double pt = p.Pt();

  if (m_do_E)        *m_E     = static_cast<float> (p.E());
  if (m_do_p)        *m_p     = static_cast<float> (p.P());
  if (m_do_Et)       *m_Et    = static_cast<float> (p.Et());
  if (m_do_pt)       *m_pt    = static_cast<float> (pt);
  if (m_do_m)        *m_m     = static_cast<float> (p.M());
  if (m_do_rapidity) *m_y     = static_cast<float> (p.Rapidity());
  if (m_do_tanth)    *m_tanth = static_cast<float> (tan(p.Theta()));

  if (m_do_etaphi) {
    if (pt < 1e-6) {
      if (p.Pz() > 0)
        *m_eta = 10e10;
      else
        *m_eta = 10e-10;
    }
    else
      *m_eta = static_cast<float> (p.Eta());
    *m_phi = static_cast<float> (p.Phi());
  }

  if (m_do_rect) {
    *m_px = static_cast<float> (p.Px());
    *m_py = static_cast<float> (p.Py());
    *m_pz = static_cast<float> (p.Pz());
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
