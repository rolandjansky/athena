// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/src/AuxDataFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Copy aux data to D3PD.
 */

#ifndef D3PDMAKERCORECOMPS_AUXDATAFILLERTOOL_H
#define D3PDMAKERCORECOMPS_AUXDATAFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AthContainers/AuxElement.h"
#include "RootUtils/Type.h"
#include <vector>
#include <string>
#include <memory>


namespace D3PD {


/**
 * @brief Copy aux data to D3PD.
 */
class AuxDataFillerTool
  : public BlockFillerTool<SG::AuxElement>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  AuxDataFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const SG::AuxElement& p);


private:
  /// Parse the variables property and fill @c m_vars.
  StatusCode parseVars();

  /// Property: Prefix to add to aux data names.
  std::string m_auxprefix;

  /// Property: Specify variables to fill.
  /// Each is of the form VAR[=AUXVAR][<DEF][#DOCSTRING].
  std::vector<std::string> m_varString;


  /// Describe one variable.
  struct Var
  {
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
    Var (const std::string& the_name,
         const std::string& the_docstring,
         const std::vector<std::string>& the_labels,
         const std::vector<std::string>& the_label_classes,
         const std::string& the_defstring,
         bool the_has_default);


    /**
     * @brief Try to retrieve an aux data item from p.
     * @param p The element to read from.
     *
     * Returns 0 if no alternatives are available.
     */
    const void* access (const SG::AuxElement& p) const;

    /// Name of the variable.
    std::string name;

    /// Docstring for the variable.
    std::string docstring;

    /// If present, a default object instance to use if the aux variable
    /// is not present.
    RootUtils::Type::unique_ptr defobj;

    /// If true, this aux variable can be defaulted.
    bool has_default;

    /// Type of the item
    RootUtils::Type type;

    /// Type of the variable, as @c type_info.
    const std::type_info* ti;

    /// Aux data accessor(s) for the item.
    std::vector<SG::AuxElement::TypelessConstAccessor> accessors;

    /// Pointer passed to @c ID3PD.
    void* ptr;

    /// Name of the aux data item(s).
    std::string label;
  };


  /// Parsed list of variables.
  std::vector<std::unique_ptr<Var> > m_vars;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_AUXDATAFILLERTOOL_H
