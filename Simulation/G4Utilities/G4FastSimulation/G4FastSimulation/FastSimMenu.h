/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastSimMenu_H
#define FastSimMenu_H

#include <string>

class FastSimMenu 
{
 public:
  FastSimMenu();

  static void ListFastSimModels();
  static void AssignFsModel(std::string regName, std::string modelName);
};

#endif
