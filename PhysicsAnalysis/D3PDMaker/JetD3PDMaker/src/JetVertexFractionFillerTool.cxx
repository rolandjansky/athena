/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetVertexFractionFillerTool.cxx
 * @author scott snyder
 * @date Feb, 2011
 * @brief Block filler tool for the Jet Vertex Fraction
 */


#include "JetVertexFractionFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/IUserDataSvc.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetVertexFractionFillerTool::JetVertexFractionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_userDataSvc ("UserDataSvc", name)
{
  declareProperty ("UserDataSvc", m_userDataSvc,
                   "The UserDataSvc");

  declareProperty ("FromUD", m_fromUD = false,
                   "Take the JVF from UserData?  (Note: this is always "
                   "the case if full JVF info is requested.)");

  declareProperty ("FillFullJVF", m_fillFullJVF = false,
                   "Fill JVF info for all vertices?  (Note: JVF is always "
                   "taken from UD in this case.)");

  declareProperty ("UDPrefix", m_udprefix,
                   "Prefix to add to UD labels.");

  m_jvtxf = 0;
  m_fullJVF = 0;
}
  

StatusCode JetVertexFractionFillerTool::initialize()
{
  if (m_fromUD || m_fillFullJVF)
    CHECK( m_userDataSvc.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode JetVertexFractionFillerTool::book()
{
  CHECK( addVariable ("jvtxf",     m_jvtxf  )  );
  if (m_fillFullJVF)
    CHECK( addVariable ("jvtxfFull", m_fullJVF)  );
  
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
StatusCode JetVertexFractionFillerTool::fill (const Jet& p)
{
  if (!m_fromUD && !m_fillFullJVF) {
    *m_jvtxf = p.getMoment ("JVF");
    return StatusCode::SUCCESS;
  }

  std::string lab = m_udprefix + "fullJVF";
  std::vector<double> jvf;
  if (!m_userDataSvc->getInMemElementDecoration (p, lab, jvf).isSuccess()) {
    if (!m_userDataSvc->getElementDecoration (p, lab, jvf).isSuccess()) {
      REPORT_MESSAGE(MSG::ERROR)
        << "Can't find UserData element variable " << lab;
      return StatusCode::RECOVERABLE;
    }
  }

  if (m_fillFullJVF)
    m_fullJVF->assign (jvf.begin(), jvf.end());
  if (!jvf.empty())
    *m_jvtxf = jvf[0];
  else
    *m_jvtxf = -1;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
