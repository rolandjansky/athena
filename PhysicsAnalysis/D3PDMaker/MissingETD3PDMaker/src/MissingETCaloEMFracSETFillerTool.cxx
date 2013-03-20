/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETCaloFillerTool.cfxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date 24 Sep, 2009
 * @brief Block filler tool for Missing ET Calo objects.
 */


#include "MissingETD3PDMaker/MissingETCaloEMFracSETFillerTool.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETCaloEMFracSETFillerTool::MissingETCaloEMFracSETFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<MissingEtCalo> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETCaloEMFracSETFillerTool::book()
{
  CHECK( addVariable ("SUMET_EMFrac", m_SUMET_BASE_EMfrac) );
  return StatusCode::SUCCESS;
}


StatusCode MissingETCaloEMFracSETFillerTool::fill (const MissingEtCalo& p)
{
  float temp = 0;
  if (p.sumet() != 0) {
    temp += p.etSumCalo(MissingEtCalo::EMB);
    temp += p.etSumCalo(MissingEtCalo::PEMB);
    temp += p.etSumCalo(MissingEtCalo::EME);
    temp += p.etSumCalo(MissingEtCalo::PEMEC);
    temp = temp/p.sumet();
  }
  *m_SUMET_BASE_EMfrac = temp; 

  return StatusCode::SUCCESS;
}


} // namespace D3PD
