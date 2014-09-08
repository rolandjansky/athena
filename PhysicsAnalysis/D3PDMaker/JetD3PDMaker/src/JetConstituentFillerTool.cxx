/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetConstituentFillerTool.cxx
 * @author Michele Petteni < mpetteni AT cern.ch > , 
           Haifeng Li <Haifeng.Li@cern.ch>
 * @date November, 2009
 * @brief Block filler tool for Jet Constituents 
 */

#include "JetConstituentFillerTool.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetConstituentIterator.h"
//#include "JetUtils/JetCaloHelper.h"

#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"

#include <iostream>

namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetConstituentFillerTool::JetConstituentFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  // Extra properties for Signal States

  declareProperty ("SignalStateNumber", m_sigstate = -1);
  declareProperty ("SignalStatePrefix", m_prefix   = "default");

  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetConstituentFillerTool::book()
{

  CHECK( addVariable ("Nconst"               , m_Ncon   ));
  CHECK( addVariable ("ptconst_"  + m_prefix , m_ptcon  ));
  CHECK( addVariable ("econst_"   + m_prefix , m_econ   ));
  CHECK( addVariable ("etaconst_" + m_prefix , m_etacon ));
  CHECK( addVariable ("phiconst_" + m_prefix , m_phicon ));
  CHECK( addVariable ("weightconst_" + m_prefix , m_weightcon));
  
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

StatusCode JetConstituentFillerTool::fill(const Jet& p)
{

  JetConstituentIterator iterJet = JetConstituentIterator::first( &p ); 
  
  if ( m_sigstate != -1){
    iterJet = JetConstituentIterator::first( &p,P4SignalState::State(m_sigstate) );
  }

  JetConstituentIterator lJet    = JetConstituentIterator::last(&p); 

  *m_Ncon = p.size();

  for ( ; iterJet != lJet; ++iterJet)
    {
      m_ptcon->push_back((*iterJet)->pt());
      m_econ->push_back((*iterJet)->e());
      m_etacon->push_back((*iterJet)->eta());
      m_phicon->push_back((*iterJet)->phi());
      // m_weightcon->push_back(p.getWeight((*iterJet)));
    }


  return StatusCode::SUCCESS;
}


} // namespace D3PD
