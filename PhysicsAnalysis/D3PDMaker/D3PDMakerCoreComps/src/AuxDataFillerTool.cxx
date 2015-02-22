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
#include "boost/algorithm/string/split.hpp"
#include "TROOT.h"


namespace D3PD {


/**
 * @brief Constructor.
 * @param the_name Name of the variable.
 * @param the_docstring Docstring for the variable.
 * @param the_labels Name of the aux data item(s).
 * @param the_label_classes Class(es) of the aux data item(s),
 *                          or blank strings.
 * @param the_defstring String giving the default value.
 * @param the_has_default Can this variable be defaulted?
 */
AuxDataFillerTool::Var::Var (const std::string& the_name,
                             const std::string& the_docstring,
                             const std::vector<std::string>& the_labels,
                             const std::vector<std::string>& the_label_classes,
                             const std::string& the_defstring,
                             bool the_has_default)
  : name (the_name),
    docstring (the_docstring),
    has_default (the_has_default),
    ptr (0)
{
  const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

  std::string firstName;
  SG::auxid_t firstId = SG::null_auxid;
  ti = 0;
  assert (the_labels.size() == the_label_classes.size());
  for (size_t i = 0; i < the_labels.size(); i++) {
    SG::auxid_t auxid = reg.findAuxID (the_labels[i], the_label_classes[i]);
    if (auxid != SG::null_auxid) {
      accessors.emplace_back (the_labels[i], the_label_classes[i]);

      std::string name = the_label_classes[i] + "::" + the_labels[i];
      if (!label.empty())
        label += ",";
      label += name;

      // Find the type of the variable.
      if (ti == 0) {
        ti = reg.getType (auxid);
        type.init (reg.getTypeName (auxid));
        firstName = name;
        firstId = auxid;
      }
      else {
        if (ti != reg.getType (auxid)) {
          std::string errstr = "Inconsistent types for aux data: " +
            firstName + " (" + reg.getTypeName (firstId) +") vs " +
            name + " (" + reg.getTypeName (auxid) + ")";
          throw std::runtime_error (errstr);
        }
      }
    }
  }

  if (the_has_default && !the_defstring.empty()) {
    defobj = RootUtils::Type::unique_ptr (type.create(), type);
    type.fromString (defobj.get(), the_defstring);
  }
}


/**
 * @brief Try to retrieve an aux data item from p.
 * @param p The element to read from.
 *
 * Returns 0 if no alternatives are available.
 */
const void* AuxDataFillerTool::Var::access (const SG::AuxElement& p) const
{
  for (const auto& a : accessors) {
    if (a.isAvailable(p))
      return a(p);
  }
  return 0;
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
                   "It may also be a comma-separated string of variable "
                   "names; the first one found to be present will be used. "
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
  for (auto& v : m_vars) {
    const void* aux = v->access(p);
    if (aux)
      v->type.assign (v->ptr, aux);
    else if (!v->has_default) {
      // Trigger an exception.
      v->accessors[0](p);
    }
  }

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

    std::vector<std::string> labels;
    boost::algorithm::split (labels, label, boost::algorithm::is_any_of(","));
    std::vector<std::string> label_classes;
    for (std::string& l : labels) {
      std::string label_class;
      ipos = l.find ("::");
      if (ipos != std::string::npos) {
        label_class = l.substr (0, ipos);
        l.erase (0, ipos+2);
      }
      boost::algorithm::trim (l);
      l = m_auxprefix + l;
      boost::algorithm::trim (label_class);
      label_classes.push_back (label_class);
    }

    boost::algorithm::trim (name);
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
        if (ti) {
          for (size_t i = 0; i < labels.size(); i++)
            reg.getAuxID (*ti, labels[i], label_classes[i]);
        }
      }
    }

    try {
      m_vars.push_back (CxxUtils::make_unique<Var>
                        (name, docstring, labels, label_classes,
                         defstring, has_default));
    }
    catch (const std::runtime_error& e) {
      REPORT_MESSAGE(MSG::ERROR)
        << "Can't find aux data item(s) " << label
        << " [" << e.what() << "]";
      return StatusCode::FAILURE;
    }

    if (m_vars.back()->accessors.empty()) {
      REPORT_MESSAGE(MSG::ERROR)
        << "Can't find aux data item(s) " << label;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
