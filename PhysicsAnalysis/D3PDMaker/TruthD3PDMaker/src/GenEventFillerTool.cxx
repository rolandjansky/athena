/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventFillerTool.cxx 504001 2012-06-05 16:30:11Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenEventFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for @c GenEvent information.
 */


#include "GenEventFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GenEventFillerTool::GenEventFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : BlockFillerTool<HepMC::GenEvent> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode GenEventFillerTool::book()
{
  CHECK( addVariable ("signal_process_id",   m_signal_process_id) );
  CHECK( addVariable ("event_number",        m_event_number) );
  CHECK( addVariable ("event_scale",         m_event_scale) );
  CHECK( addVariable ("alphaQCD",            m_alphaQCD) );
  CHECK( addVariable ("alphaQED",            m_alphaQED) );
  CHECK( addVariable ("pdf_id1",             m_pdf_id1) );
  CHECK( addVariable ("pdf_id2",             m_pdf_id2) );
  CHECK( addVariable ("pdf_x1",              m_pdf_x1) );
  CHECK( addVariable ("pdf_x2",              m_pdf_x2) );
  CHECK( addVariable ("pdf_scale",           m_pdf_scale) );
  CHECK( addVariable ("pdf1",                m_pdf1) );
  CHECK( addVariable ("pdf2",                m_pdf2) );
  CHECK( addVariable ("weight",              m_weight) );
  return StatusCode::SUCCESS;
}


StatusCode GenEventFillerTool::fill (const HepMC::GenEvent& p)
{
  *m_signal_process_id = p.signal_process_id();
  *m_event_number      = p.event_number();
  *m_event_scale       = p.event_scale();
  *m_alphaQCD          = p.alphaQCD();
  *m_alphaQED          = p.alphaQED();
  for(size_t i=0;i<(size_t)p.weights().size();i++)
    m_weight->push_back(p.weights()[i]);
  if (p.pdf_info()) {
    *m_pdf_id1   = p.pdf_info()->id1();
    *m_pdf_id2   = p.pdf_info()->id2();
    *m_pdf_x1    = p.pdf_info()->x1();
    *m_pdf_x2    = p.pdf_info()->x2();
    *m_pdf_scale = p.pdf_info()->scalePDF();
    *m_pdf1      = p.pdf_info()->pdf1();
    *m_pdf2      = p.pdf_info()->pdf2();
  } else {
    *m_pdf_id1 = *m_pdf_id2 = 0;
    *m_pdf_x1 = *m_pdf_x2 = *m_pdf_scale = *m_pdf1 = *m_pdf2 = -1.;
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
