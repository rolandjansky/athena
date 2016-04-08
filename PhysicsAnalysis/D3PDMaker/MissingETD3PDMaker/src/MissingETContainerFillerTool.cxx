/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETContainerFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Simple filler for xAOD missing et.
 */


#include "MissingETContainerFillerTool.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETContainerFillerTool::MissingETContainerFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<xAOD::MissingETContainer> (type, name, parent),
        m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "Getter for the object used to define the output variables.");
}



/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode MissingETContainerFillerTool::initialize()
{
  CHECK( m_getter.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETContainerFillerTool::book()
{
  const xAOD::MissingETContainer* metc =
    m_getter->get<xAOD::MissingETContainer>();

  for (const xAOD::MissingET* m : *metc) {
    Vars& v = m_vars[m->name()];
    std::string prefix = m->name();
    if (!prefix.empty())
      prefix += "_";
    CHECK( addVariable (prefix + "etx",   v.m_etx) );
    CHECK( addVariable (prefix + "ety",   v.m_ety) );
    CHECK( addVariable (prefix + "sumet", v.m_sumet) );
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
StatusCode
MissingETContainerFillerTool::fill (const xAOD::MissingETContainer& p)
{
  for (const xAOD::MissingET* m : p) {
    auto it = m_vars.find(m->name());
    if (it != m_vars.end()) {
      Vars& v = it->second;
      *v.m_etx   = m->mpx();
      *v.m_ety   = m->mpy();
      *v.m_sumet = m->sumet();
    }
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
