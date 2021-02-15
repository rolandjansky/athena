/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <sstream>
#include "./nodeIDPrinter.h"

std::string nodeIDPrinter(const std::string& name,
                          int nodeID,
                          int parentID,
                          bool pass,
                          const std::string& timerStr){

  std::stringstream ss;
  ss << "name(): " << name <<"  node: " << nodeID
     << " parent: " << parentID
     << " pass: " << std::boolalpha << pass << " " <<timerStr << '\n';

  return ss.str();
}

std::string nodeIDPrinter(const std::string& name,
                          bool pass,
                          const std::string& timerStr){

  std::stringstream ss;
  ss << "name(): " << name <<"  pass: " <<std::boolalpha << pass << " "
     << timerStr << '\n';

  return ss.str();
}

std::string nodeIDPrinter(const std::string& name,
                          int nodeID,
                          int parentID,
                          const std::optional<bool>& pass,
                          const std::string& timerStr){
  if(pass.has_value()){
    return nodeIDPrinter(name, nodeID, parentID, *pass, timerStr);
  }
  std::stringstream ss;
  ss << "name(): " <<name <<"  node: " << nodeID
     << " parent: " << parentID
     << " pass: " << "ERROR " <<timerStr << '\n';

  return ss.str();
}

std::string nodeIDPrinter(const std::string& name,
                          const std::optional<bool>& pass,
                          const std::string& timerStr){
  if(pass.has_value()){
    return nodeIDPrinter(name, *pass, timerStr);
  }

  
  std::stringstream ss;
  
  ss << "name(): " << name << " pass: " << "ERROR " <<timerStr << '\n';

  return ss.str();
}

std::string nodeIDPrinter(const std::string& name,
                          int nodeID,
                          int parentID){

  std::stringstream ss;
  ss << "name(): " << name <<"  node: " << nodeID
     << " parent: " << parentID
     << '\n';
  
  return ss.str();
}
