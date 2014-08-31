// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @class  SerializeCommon
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 * temporary utility to map to a common class names between 
 *                       CLASS_DEFs and Reflex dictionaries 
 */


#include <string>

namespace TrigSerializeResult {
  namespace {
    bool isxAOD(const std::string& containerName) {
      return containerName.find("xAOD::") == 0 and containerName.find("AuxContainer") == std::string::npos; // the name should start from the xAOD      
    }
    std::string xAODContainedClass(const std::string& containerName) {
      auto containerBegin = containerName.find("Container");
      std::string objname = std::string(containerName, 0, containerBegin) // front of the name
	+ containerName.substr(containerName.size()-3,3); // version
      return objname;
    }
  }

  std::string remapToDictName(const std::string &nameOfClass){
    std::string noc = nameOfClass;
    if ( isxAOD(nameOfClass) ) {
      noc="DataVector<"+xAODContainedClass(nameOfClass)+">";
    }  // overwrites
    else if (nameOfClass=="Analysis :: TauJetContainer") 
      noc="Analysis::TauJetContainer";
    else if (nameOfClass=="Analysis :: TauDetailsContainer")
      noc="Analysis::TauDetailsContainer";
    return noc;
  }

}

