// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonD3PDMaker/src/SkimDecisionFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2010
 * @brief Block filler tool for SkimDecisions.
 *        Fills one branch per flag.
 */


#include "SkimDecisionFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SkimDecisionFillerTool::SkimDecisionFillerTool (const std::string& type, 
                                                const std::string& name, 
                                                const IInterface* parent )
  : BlockFillerTool<SkimDecisionCollection>( type, name, parent ),
    m_passed (0)
{
  declareProperty ("Flags", m_flags,
                   "Specify variables to fill.\n\
This is a collection of strings of the form FLAG:VAR:DOCSTRING.\n\
FLAG is the name of the flag to fill.\n\
VAR is the name of the D3PD variable to fill.  If left empty,\n\
FLAG will be used as the variable name.\n\
DOCSTRING is the documentation string to use for this variable.");

  declareProperty ("AnyPassedName", m_anyPassedName,
                   "If set, this names a variable to be set to true\n\
if any flags in this block are true.");
}


/**
 * @brief Book variables for this block.
 */
StatusCode SkimDecisionFillerTool::book()
{
  m_vars.reserve (m_flags.size());
  for (std::string s : m_flags) {
    Var var;
    std::string::size_type ipos = s.find (':');
    var.key = s.substr (0, ipos);
    std::string varname;
    std::string docstring;
    if (ipos != std::string::npos) {
      ++ipos;
      std::string::size_type jpos = s.find (':', ipos);
      if (jpos != std::string::npos) {
        varname = s.substr (ipos, jpos-ipos);
        docstring = s.substr (jpos+1);
      }
      else
        varname = s.substr (ipos);
    }
    if (varname.empty())
      varname = var.key;
    var.val = 0;
    m_vars.push_back (var);
    
    CHECK( addVariable (varname, m_vars.back().val, docstring) );
  }

  if (!m_anyPassedName.empty())
    CHECK( addVariable (m_anyPassedName, m_passed,
                        "True if any flags in this block were true.") );

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
StatusCode SkimDecisionFillerTool::fill (const SkimDecisionCollection& p)
{
  for (const SkimDecision* s : p) {
    for (Var& v : m_vars) {
      if (s->isAccepted() && s->getName() == v.key) {
        *v.val = true;
        if (m_passed)
          *m_passed = true;
        break;
      }
    }
  }
     
  return StatusCode::SUCCESS;
}


} // namespace D3PD
