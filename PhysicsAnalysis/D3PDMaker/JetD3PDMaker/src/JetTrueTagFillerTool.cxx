/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetFlavorTagComponentsFillerTool.cxx
 * @author Erik Devetak <edevetak@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for Jet BTag related quantities. The class implements most of the code in PhysicsAnalysis/JetTagging/JetTagValidation/JetTagNtuple/tags/JetTagNtuple-00-00-25/src/JetTagNtuple_JetTool.cxx by 
 */


#include "JetTrueTagFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/TruthInfo.h"

#include <iostream>

namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetTrueTagFillerTool::JetTrueTagFillerTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTrueTagFillerTool::book()
{

  CHECK(addVariable ("flavor_truth_dRminToB",   m_dRminToB));
  CHECK(addVariable ("flavor_truth_dRminToC",   m_dRminToC));
  CHECK(addVariable ("flavor_truth_dRminToT",   m_dRminToT));
  CHECK(addVariable ("flavor_truth_BHadronpdg", m_truepdg ));
  CHECK(addVariable ("flavor_truth_trueflav",   m_iflav   ));
  
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
StatusCode JetTrueTagFillerTool::fill(const Jet& p)
{
  std::string label("N/A");

  const Analysis::TruthInfo* mcinfo = p.getTagInfo<Analysis::TruthInfo>("TruthInfo");
  if(mcinfo) 
    {
      int iflav =0;
      label = mcinfo->jetTruthLabel();

      if(label=="B") iflav = 5;
      if(label=="C") iflav = 4;
      if(label=="T") iflav = 15;
      *m_dRminToB = mcinfo->deltaRMinTo("B");
      *m_dRminToC = mcinfo->deltaRMinTo("C");
      *m_dRminToT = mcinfo->deltaRMinTo("T");
      *m_truepdg = mcinfo->BHadronPdg();
      *m_iflav = iflav;
      //truebvtx = mcinfo->BDecayVertex();
  }
  else
    {
      *m_dRminToB = -1000;
      *m_dRminToC = -1000;
      *m_dRminToT = -1000;
      *m_truepdg = 0;
      *m_iflav =-1000;
    }
  //<< " BCode= " << truepdg << " r(B)= " << truebvtx.perp() << " z(B)= " << truebvtx.z();

  return StatusCode::SUCCESS;
}


}// namespace D3PD
