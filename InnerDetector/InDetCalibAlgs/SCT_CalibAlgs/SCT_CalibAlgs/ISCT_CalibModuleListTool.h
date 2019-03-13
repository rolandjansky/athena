/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibModuleListTool.h
 * Header file for the SCT_CalibModuleListTool class
 * @author Shaun Roe
 **/

#ifndef ISCT_CalibModuleListTool_h
#define ISCT_CalibModuleListTool_h

#include "Identifier/Identifier.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// STL and boost headers
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

class ISCT_CalibModuleListTool : virtual public IAlgTool {
 public:
  //@name Service methods, reimplemented
  //@{
  ISCT_CalibModuleListTool() {/**nop**/};
  virtual ~ISCT_CalibModuleListTool() = default;
  static const InterfaceID& interfaceID();
  //@}
  virtual StatusCode readModuleList(std::map<Identifier, std::set<Identifier>>& moduleList) const =0;

 protected:
  template<class T>
    std::pair<std::string, bool> retrievedTool(T& tool) const {
    if (tool.retrieve().isFailure()) return std::make_pair(std::string{"Unable to retrieve "}+tool.name(), false);
    return std::make_pair("", true);
  }
};

inline const InterfaceID& ISCT_CalibModuleListTool::interfaceID() {
  static const InterfaceID IID{"ISCT_CalibModuleListTool", 1, 0};
  return IID;
}
#endif
