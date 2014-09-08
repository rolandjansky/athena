/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/JetSignalStateFillerTool.cxx
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date September, 2009
 * @brief Block filler tool for a four-momentum.
 *
 * FIXME: Use FourMomFiller instead!
 */


#include "JetSignalStateFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetSignalStateFillerTool::JetSignalStateFillerTool (const std::string& type,
						    const std::string& name,
						    const IInterface* parent)
  : BlockFillerTool<Jet> (type, name, parent)
{
  
  // Properties taken from FourMomFillerTool
  
  declareProperty ("WriteE",        m_do_E        = false);
  declareProperty ("WriteP",        m_do_p        = false);
  declareProperty ("WriteEt",       m_do_Et       = false);
  declareProperty ("WritePt",       m_do_pt       = true);
  declareProperty ("WriteM",        m_do_m        = true);
  declareProperty ("WriteRapidity", m_do_rapidity = false);
  declareProperty ("WriteTanTh",    m_do_tanth    = false);
  declareProperty ("WriteEtaPhi",   m_do_etaphi   = true);
  declareProperty ("WriteRect",     m_do_rect     = false);

  // Extra properties for Signal States

  declareProperty ("SignalStateNumber", m_sigstate = 0);
  declareProperty ("SignalStatePrefix", m_prefix   = "emscale");

  m_E = 0;
  m_p = 0;
  m_Et = 0;
  m_pt = 0;
  m_m = 0;
  m_y = 0;
  m_tanth = 0;
  m_eta = 0;
  m_phi = 0;
  m_px = 0;
  m_py = 0;
  m_pz = 0;
}


/**
 * @brief Book variables for this block.
 */
StatusCode JetSignalStateFillerTool::book()
{
  if (m_do_E)        CHECK( addVariablePrefixed ("E",     m_E)  );
  if (m_do_p)        CHECK( addVariablePrefixed ("p",     m_p)  );
  if (m_do_Et)       CHECK( addVariablePrefixed ("Et",    m_Et)  );
  if (m_do_pt)       CHECK( addVariablePrefixed ("pt",    m_pt)  );
  if (m_do_m)        CHECK( addVariablePrefixed ("m",     m_m)  );
  if (m_do_rapidity) CHECK( addVariablePrefixed ("y",     m_y)  );
  if (m_do_tanth)    CHECK( addVariablePrefixed ("tanth", m_tanth)  );

  if (m_do_etaphi) {
    CHECK( addVariablePrefixed ("eta", m_eta) );
    CHECK( addVariablePrefixed ("phi", m_phi) );
  }

  if (m_do_rect) {
    CHECK( addVariablePrefixed ("px",   m_px)  );
    CHECK( addVariablePrefixed ("py",   m_py)  );
    CHECK( addVariablePrefixed ("pz",   m_pz)  );
  }

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
StatusCode JetSignalStateFillerTool::fill (const Jet& p)
{
  SignalStateHelper t(&p, P4SignalState::State(m_sigstate));
  
  if (m_do_E)        *m_E     = p.e();
  if (m_do_p)        *m_p     = p.p();
  if (m_do_Et)       *m_Et    = p.et();
  if (m_do_pt)       *m_pt    = p.pt();
  if (m_do_m)        *m_m     = p.m();
  if (m_do_rapidity) *m_y     = p.rapidity();
  if (m_do_tanth)    *m_tanth = p.tanTh();

  if (m_do_etaphi) {
    *m_eta = p.eta();
    *m_phi = p.phi();
  }

  if (m_do_rect) {
    *m_px = p.px();
    *m_py = p.py();
    *m_pz = p.pz();
  }

  return StatusCode::SUCCESS;
}

template <class T>
StatusCode JetSignalStateFillerTool::addVariablePrefixed (const std::string& name,
							  T* & ptr,
							  const std::string& /*dim = ""*/)
{
  std::string newname = m_prefix + "_" + name;
  return addVariable(newname, ptr);
}


} // namespace D3PD
