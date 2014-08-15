/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FastSimulation/FastSimModelCatalog.h"

FastSimModelCatalog* FastSimModelCatalog::thePointer = 0;

FastSimModelCatalog::FastSimModelCatalog()
{
}

FastSimModelCatalog* FastSimModelCatalog::GetInstance()
{
  if (!thePointer) 
    thePointer = new FastSimModelCatalog;
  return thePointer;
}

void FastSimModelCatalog::RegisterModel(FastSimModel* model)
{
  if (theCatalog.find(model->GetName())!=theCatalog.end())
    std::cout << "Redefining Fast Simulation Model " << model->GetName() << "\n";

  theCatalog[model->GetName()]=model;
}

FastSimModel* FastSimModelCatalog::GetModel(std::string name)
{
  fsModelCatalog::iterator it = theCatalog.find(name);
  if (it==theCatalog.end())
  {
    std::cout << "Fast Simulation Model " << name << " not found!\n";
    return 0;
  }
  else 
    return (*it).second;
}

void FastSimModelCatalog::PrintAll()
{
  std::cout<<"Printing the list of fast simulation models:\n";
  std::cout<<" Number of models :: " << theCatalog.size() << "\n";
  for (fsModelCatalog::const_iterator it=theCatalog.begin();it!=theCatalog.end();it++)
    std::cout << " --- model    " << (*it).first << "\n";
}
