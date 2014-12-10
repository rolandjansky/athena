/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: UserDataFillerTool.cxx 635137 2014-12-10 18:34:44Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/UserDataFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Copy data from UserDataSvc to D3PD.
 */


#include "UserDataFillerTool.h"
#include "Navigation/IAthenaBarCode.h"
#include "AthenaKernel/errorcheck.h"
#include "TROOT.h"
#include "boost/foreach.hpp"


namespace D3PD {


/**
 * @brief Constructor.
 * @param the_name Name of the variable.
 * @param the_docstring Docstring for the variable.
 * @param the_label UserData label of the variable.
 * @param the_type Type of the variable.
 */
UserDataFillerTool::Var::Var (const std::string& the_name,
                              const std::string& the_docstring,
                              const std::string& the_label,
                              const RootUtils::Type& the_type)
  : name (the_name),
    docstring (the_docstring),
    label (the_label),
    type (the_type),
    ptr (0)
{
}


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
UserDataFillerTool::UserDataFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : BlockFillerTool<IAthenaBarCode> (type, name, parent),
    m_userDataSvc ("UserDataSvc", name)
{
  declareProperty ("UserDataSvc", m_userDataSvc,
                   "The UserDataSvc");

  declareProperty ("UDPrefix", m_udprefix,
                   "Prefix to add to UD labels.");

  declareProperty ("Vars", m_varString,
                   "Specify variables to fill.\n"
                   "A sequence of VAR, LABEL, TYPE.\n"
                   "VAR is the D3PD variable name.  "
                   "It may contain a comment following a colon.\n"
                   "LABEL is the UDS label.  If blank, defaults to VAR.\n"
                   "TYPE is the name of the UD type.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode UserDataFillerTool::initialize()
{
  CHECK( m_userDataSvc.retrieve() );
  return BlockFillerTool<IAthenaBarCode>::initialize();
}


/**
 * @brief Book variables for this block.
 */
StatusCode UserDataFillerTool::book()
{
  CHECK( parseVars() );
  BOOST_FOREACH (Var& v, m_vars) {
    std::string docstring = "[UserData: ";
    docstring += v.label;
    docstring += "] ";
    docstring += v.docstring;
    CHECK( addVariable (v.name, *v.type.getTypeInfo(), v.ptr, docstring) );
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
StatusCode UserDataFillerTool::fill (const IAthenaBarCode& p)
{
  BOOST_FOREACH (Var& v, m_vars) {
    void* ptr = 0;
    if (m_userDataSvc->vgetInMemElementDecoration (p,
                                                   v.label,
                                                   *v.type.getTypeInfo(),
                                                   ptr,
                                                   true) != 0)
    {
      if (m_userDataSvc->vgetElementDecoration (p,
                                                v.label,
                                                *v.type.getTypeInfo(),
                                                ptr))
      {
        REPORT_MESSAGE(MSG::ERROR)
          << "Can't find UserData element variable " << v.label;
        return StatusCode::RECOVERABLE;
      }
    }

    v.type.assign (v.ptr, ptr);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Parse the variables property and fill @c m_vars.
 */
StatusCode UserDataFillerTool::parseVars()
{
  for (size_t i = 0; i+3 <= m_varString.size(); i += 3) {
    std::string name = m_varString[i];

    std::string docstring;
    std::string::size_type ipos = name.find (":");
    if (ipos != std::string::npos) {
      docstring = name.substr (ipos+1, std::string::npos);
      name = name.substr (0, ipos);
    }

    std::string label = m_varString[i+1];
    if (label.empty())
      label = name;
    RootUtils::Type type;
    try {
      type = RootUtils::Type (m_varString[i+2]);
    }
    catch (std::runtime_error&) {
      gROOT->GetClass (m_varString[i+2].c_str());
      try {
        type = RootUtils::Type (m_varString[i+2]);
      }
      catch (std::runtime_error&) {
        REPORT_MESSAGE(MSG::ERROR) << "Can't find type "
                                   << m_varString[i+2];
        return StatusCode::FAILURE;
      }
    }

    m_vars.push_back (Var (name, docstring, m_udprefix + label, type));
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
