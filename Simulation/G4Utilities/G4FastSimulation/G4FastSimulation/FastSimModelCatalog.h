/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastSimModelCatalog_H
#define FastSimModelCatalog_H

#include "G4FastSimulation/FastSimModel.h"
#include <map>
#include <string>

typedef std::map<std::string, FastSimModel*, std::less<std::string> >  fsModelCatalog;

class FastSimModelCatalog 
{
 public:
  static FastSimModelCatalog* GetInstance();

  void RegisterModel(FastSimModel* model);
  FastSimModel* GetModel(std::string name);
	
  void PrintAll();

 private:
  //Constructor
  inline FastSimModelCatalog();
  
  fsModelCatalog theCatalog;
  static FastSimModelCatalog* thePointer;

};

#endif
