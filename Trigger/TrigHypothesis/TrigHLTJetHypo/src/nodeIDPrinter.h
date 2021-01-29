/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* 
   function to provide a standard way of output hypo tree node information.
*/

#ifndef TRIGHLTJETHYPO_NODEIDPRINTER_H
#define TRIGHLTJETHYPO_NODEIDPRINTER_H


#include <string>
#include <optional>

std::string nodeIDPrinter(const std::string& name,
                          int nodeID,
                          int parentID,
                          bool pass,
                          const std::string& time="");

std::string nodeIDPrinter(const std::string& name,
                          int nodeID,
                          int parentID,
                          const std::optional<bool>& pass,
                          const std::string& time="");

std::string nodeIDPrinter(const std::string& name,
                          const std::optional<bool>& pass,
                          const std::string& time);


std::string nodeIDPrinter(const std::string& name,
                          int nodeID,
                          int parentID);
#endif
