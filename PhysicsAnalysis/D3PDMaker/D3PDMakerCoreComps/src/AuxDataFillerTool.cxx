/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/src/AuxDataFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Copy aux data to D3PD.
 */


#include "AuxDataFillerTool.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"
#include "boost/algorithm/string/trim.hpp"
#include "TROOT.h"


namespace D3PD {


/**
 * @brief Constructor.
 * @param the_name Name of the variable.
 * @param the_docstring Docstring for the variable.
 * @param the_label Name of the aux data item.
 * @param the_label_class Class of the aux data item, or a blank string.
 */
AuxDataFillerTool::Var::Var (const std::string& the_name,
                             const std::string& the_docstring,
                             const std::string& the_label,
                             const std::string& the_label_class,
                             const std::string& the_defstring,
                             bool the_has_default)
  : name (the_name),
    docstring (the_docstring),
    label (the_label),
    has_default (the_has_default),
    accessor (the_label, the_label_class),
    ptr (0)
{
  // Find the type of the variable.
  const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
  SG::auxid_t auxid = reg.findAuxID (the_label, the_label_class);
  assert (auxid != SG::null_auxid); // otherwise accessor ctor would have thrown
  ti = reg.getType (auxid);
  type.init (reg.getTypeName (auxid));

  if (the_has_default && !the_defstring.empty()) {
    defobj = RootUtils::Type::unique_ptr (type.create(), type);
    type.fromString (defobj.get(), the_defstring);
  }
}


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
AuxDataFillerTool::AuxDataFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<SG::AuxElement> (type, name, parent)
{
  declareProperty ("AuxPrefix", m_auxprefix,
                   "Prefix to add to aux data names.");

  declareProperty ("Vars", m_varString,
                   "Specify variables to fill.  "
                   "Each is of the form VAR[=AUXVAR][<DEF][#DOCSTRING].\n"
                   "VAR is the D3PD variable name.  "
                   "AUXVAR is the aux data item name.  It may contain a class "
                   "name before a ::.  If omitted, it defaults to VAR. "
                   "If the < is present, then it is not an error for this "
                   "variable to be missing.  If not empty, DEF specifies the "
                   "default value to use (works only for basic types). "
                   "DEF may be preceded with a type name: `TYPE: DEF' to "
                   "specify a type in the case the type of this aux variable "
                   "is unknown. "
                   "An optional documentation string may be given after a #.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode AuxDataFillerTool::initialize()
{
  return BlockFillerTool<SG::AuxElement>::initialize();
}


/**
 * @brief Book variables for this block.
 */
StatusCode AuxDataFillerTool::book()
{
  CHECK( parseVars() );
  for (auto& v : m_vars) {
    std::string docstring = "[AuxData: ";
    docstring += v->label;
    docstring += "] ";
    docstring += v->docstring;
    CHECK( addVariable (v->name, *v->ti, v->ptr, docstring, v->defobj.get()) );
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
StatusCode AuxDataFillerTool::fill (const SG::AuxElement& p)
{
  for (auto& v : m_vars)
    if (!v->has_default || v->accessor.isAvailable (p))
      v->type.assign (v->ptr, v->accessor (p));

  return StatusCode::SUCCESS;
}


/**
 * @brief Parse the variables property and fill @c m_vars.
 */
StatusCode AuxDataFillerTool::parseVars()
{
  for (std::string name : m_varString) {
    std::string docstring;
    std::string::size_type ipos = name.find ('#');
    if (ipos != std::string::npos) {
      docstring = name.substr (ipos+1, std::string::npos);
      name = name.substr (0, ipos);
    }

    std::string defstring;
    bool has_default = false;
    ipos = name.find ('<');
    if (ipos != std::string::npos) {
      defstring = name.substr (ipos+1, std::string::npos);
      name.erase (ipos, std::string::npos);
      has_default = true;
    }

    std::string label = name;
    ipos = name.find ('=');
    if (ipos != std::string::npos) {
      name.erase (ipos, std::string::npos);
      label.erase (0, ipos+1);
    }

    std::string label_class;
    ipos = label.find ("::");
    if (ipos != std::string::npos) {
      label_class = label.substr (0, ipos);
      label.erase (0, ipos+2);
    }

    boost::algorithm::trim (name);
    boost::algorithm::trim (label);
    boost::algorithm::trim (label_class);
    boost::algorithm::trim (docstring);
    boost::algorithm::trim (defstring);

    // If a type was given in the default field, make sure this variable
    // name has been registered.
    if (!defstring.empty()) {
      ipos = defstring.find (':');
      if (ipos != std::string::npos) {
        std::string typname = defstring.substr (0, ipos);
        boost::algorithm::trim (typname);
        defstring.erase (0, ipos+1);
        
        RootUtils::Type typ (typname);
        SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
        const std::type_info* ti = typ.getTypeInfo();
        if (ti)
          reg.getAuxID (*ti, label, label_class);
      }
    }

    try {
      m_vars.push_back (CxxUtils::make_unique<Var>
                        (name, docstring, m_auxprefix + label, label_class,
                         defstring, has_default));
    }
    catch (const std::runtime_error& e) {
      REPORT_MESSAGE(MSG::ERROR)
        << "Can't find aux data item " << m_auxprefix+label
        << " [" << e.what() << "]";
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
