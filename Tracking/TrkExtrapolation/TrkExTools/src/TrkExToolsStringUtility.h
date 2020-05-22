/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
 
/**
 * @file   TrkExToolsStringUtility.h
 * @brief  namespaced string utilities for TrkExTools
 * @author shaun.roe@cern.ch
 **/

#include <vector>
#include <string>

namespace TrkExTools{
  /// Return the name suffix, e.g. return 'myNiceTool' from 'ToolSvc.myNiceTool'
  std::string
  getToolSuffix(const std::string & fullToolName);
  
  ///return a vector of extracted names
  std::vector<std::string>
  extractToolNames(const std::vector<std::string> & toolNameVector);

  ///indicate whether a string is a valid Tool variable name
  bool
  validToolName(const std::string & toolName);

  /// Give an error message if tool names are invalid; empty string if good
  std::string
  possibleToolNameError(const std::vector<std::string> & toolNameVector);

  /// Give the number of unique entries in a vector
  unsigned int
  numberOfUniqueEntries(const std::vector<std::string> & nameVector);
}