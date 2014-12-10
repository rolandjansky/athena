// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: UserDataFillerTool.h 635137 2014-12-10 18:34:44Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/UserDataFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Copy data from UserDataSvc to D3PD.
 */

#ifndef D3PDMAKERCORECOMPS_USERDATAFILLERTOOL_H
#define D3PDMAKERCORECOMPS_USERDATAFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AthenaKernel/IUserDataSvc.h"
#include "RootUtils/Type.h"
#include "GaudiKernel/ServiceHandle.h"
#include <vector>
#include <string>
class IAthenaBarCode;


namespace D3PD {


/**
 * @brief Copy data from UserDataSvc to D3PD.
 */
class UserDataFillerTool
  : public BlockFillerTool<IAthenaBarCode>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  UserDataFillerTool (const std::string& type,
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
  virtual StatusCode fill (const IAthenaBarCode& p);


private:
  /// Parse the variables property and fill @c m_vars.
  StatusCode parseVars();

  /// Property: The UserDataSvc.
  ServiceHandle<IUserDataSvc> m_userDataSvc;

  /// Property: Prefix to add to UD labels.
  std::string m_udprefix;

  /// Property: Specify variables to fill.
  /// A sequence of VAR, LABEL, TYPE
  /// VAR is the D3PD variable name.
  /// LABEL is the UDS label.  If blank, defaults to VAR.
  /// TYPE is the name of the UD type.
  std::vector<std::string> m_varString;


  /// Describe one variable.
  struct Var
  {
    /**
     * @brief Constructor.
     * @param the_name Name of the variable.
     * @param the_docstring Docstring for the variable.
     * @param the_label UserData label of the variable.
     * @param the_type Type of the variable.
     */
    Var (const std::string& the_name,
         const std::string& the_docstring,
         const std::string& the_label,
         const RootUtils::Type& the_type);

    /// Name of the variable.
    std::string name;

    /// Docstring for the variable.
    std::string docstring;

    /// UserData label to read.
    std::string label;

    /// Type of the variable.
    RootUtils::Type type;

    /// Pointer passed to @c ID3PD.
    void* ptr;
  };

  /// Parsed list of variables.
  std::vector<Var> m_vars;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_USERDATAFILLERTOOL_H
