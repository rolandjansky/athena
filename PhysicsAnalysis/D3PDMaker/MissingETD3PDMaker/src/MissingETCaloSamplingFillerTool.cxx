/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETCaloFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date 24 Sep, 2009
 * @brief Block filler tool for Missing ET Calo objects.
 */


#include "MissingETD3PDMaker/MissingETCaloSamplingFillerTool.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "AthenaKernel/errorcheck.h"


namespace {


const char* CaloIndexNames[MissingEtCalo::Size] = {
  "PEMB",
  "EMB",
  "PEMEC",
  "EME",
  "TILE",
  "HEC",
  "FCAL",
};


} // anonymous namespace


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETCaloSamplingFillerTool::MissingETCaloSamplingFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<MissingEtCalo> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETCaloSamplingFillerTool::book()
{
  for (int i = 0; i < MissingEtCalo::Size; i++) {
    Vars& v = m_vars[i];
    std::string sname = CaloIndexNames[i];
    CHECK( addVariable(("etx_")   + sname, v.etx) );
    CHECK( addVariable(std::string("ety_")   + sname, v.ety) );
    CHECK( addVariable(std::string("sumet_") + sname, v.sumet) );
    CHECK( addVariable(std::string("phi_")   + sname, v.phi) );
    CHECK( addVariable(std::string("nCell_") + sname, v.nCell) );
  }

  return StatusCode::SUCCESS;
}


StatusCode MissingETCaloSamplingFillerTool::fill (const MissingEtCalo& p)
{   
  for (int i = 0; i < MissingEtCalo::Size; i++) {
    MissingEtCalo::CaloIndex index = static_cast<MissingEtCalo::CaloIndex> (i);
    Vars& v = m_vars[i];
    *v.sumet = p.etSumCalo(index);
    *v.etx = p.exCalo(index);
    *v.ety = p.eyCalo(index);
    *v.phi = atan2(p.eyCalo(index), p.exCalo(index));
    *v.nCell = p.ncellCalo(index);
  }
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
