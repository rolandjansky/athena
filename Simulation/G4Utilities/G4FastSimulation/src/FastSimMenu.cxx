/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FastSimulation/FastSimMenu.h"
#include "G4FastSimulation/FastSimModelCatalog.h"
#include "G4FastSimulation/FastSimModel.h"

#include <iostream>

#include "G4FastSimulationManager.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"

#include "FadsUtilities/Tokenizer.h"

#include <string>
#include <map>
#include <vector>

bool isMatch(const std::string& a,const std::string b)
{
  // straightforward cases
  if (a=="*") return true;
  if (a==b) return true;
  // wildcards
  FADS::Tokenizer tokens("*",a);
  bool returnValue=true;
  std::string temp=b;
  for (unsigned int i=0;i<tokens.size();i++)
  {
    if (tokens[i].empty()) continue;
    std::string::size_type npos=temp.find(tokens[i]);
    if (i==0 && npos) 
      return false;
    if (npos!=std::string::npos)
    {
      temp=temp.substr(npos, temp.size()-npos);
    }
    else 
    {
      returnValue=false;
      break;
    }
  }
  if (returnValue && !tokens[tokens.size()-1].empty())
  {
    std::string temp=tokens[tokens.size()-1];
    std::string temp2=b.substr(b.size()-temp.size(),temp.size());
    if (temp!=temp2) 
      return false;
  }
  return returnValue;
}

FastSimMenu::FastSimMenu()
{
  std::cout << "Creating the FastSimMenu\n ";
}

void FastSimMenu::ListFastSimModels()
{
  FastSimModelCatalog* cat = FastSimModelCatalog::GetInstance();
  cat->PrintAll();
}

void FastSimMenu::AssignFsModel(std::string regName, std::string modelName)
{
  std::cout << " +++++ AssignFsModel called for " 
	    << regName << " and " << modelName << "\n";

  FastSimModelCatalog* cat = FastSimModelCatalog::GetInstance();
  FastSimModel* model = cat->GetModel(modelName);

  if(model)
  {
    G4RegionStore* rs = G4RegionStore::GetInstance();

    for (unsigned int i=0;i<rs->size();i++)
    {
      G4Region* reg=(*rs)[i];

      if (reg==0) continue;
      if (isMatch(regName,reg->GetName()))
      {
	std::cout<<"Assigning fast simulation model " << modelName 
		 << " to region "<< regName << "\n";

	G4FastSimulationManager* theFastSimulationManager = reg->GetFastSimulationManager();
	if ( theFastSimulationManager == 0 )
	  theFastSimulationManager = new G4FastSimulationManager(reg);
	theFastSimulationManager->AddFastSimulationModel(model);
	theFastSimulationManager->ActivateFastSimulationModel(model->GetName());
      }
    }
  }
  else
    std::cout<<"Warning the fast simulation model " << modelName << " not found!\n";
}
