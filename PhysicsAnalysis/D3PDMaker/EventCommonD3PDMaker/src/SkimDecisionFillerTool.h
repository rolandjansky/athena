// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/SkimDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2010
 * @brief Block filler tool for SkimDecisions.
 *        Fills one branch per flag.
 */


#ifndef EVENTCOMMOND3PDMAKER_SKIMDECISIONFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_SKIMDECISIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <string>
#include <vector>


class SkimDecisionCollection;


namespace D3PD {


/**  
 * @brief Block filler tool for SkimDecisions.
 *        Fills one branch per flag.
 */
class SkimDecisionFillerTool
  : public BlockFillerTool<SkimDecisionCollection>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SkimDecisionFillerTool (const std::string& type,
                          const std::string& name, 
                          const IInterface* parent);


  /// Book variables for this block.
  StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode fill (const SkimDecisionCollection& p);


private:
  /// Property: List of decision flags to write.
  std::vector<std::string> m_flags;

  /// Property: Name of a variable to set to true if any of the specified
  /// decision flags are true.
  std::string m_anyPassedName;

  /// Description of each variable being written.
  struct Var
  {
    std::string key;
    bool* val;
  };

  /// List of variables being written.
  std::vector<Var> m_vars;

  /// Variable: Set to true if any of the specified flags passed.
  bool* m_passed;
};


} // namespace D3PD


#endif // EVENTCOMMOND3PDMAKER_SKIMDECISIONFILLERTOOL_H

