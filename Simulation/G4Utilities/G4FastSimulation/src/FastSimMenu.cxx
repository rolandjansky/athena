/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FastSimulation/FastSimMenu.h"
#include "G4FastSimulation/FastSimModelCatalog.h"
#include "G4FastSimulation/FastSimModel.h"

#include "FadsUtilities/Tokenizer.h"

#include "G4FastSimulationManager.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"

#include <iostream>
#include <map>
#include <string>
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
  std::cout << "FastSimMenu    Creating the FastSimMenu" << std::endl;
}

void FastSimMenu::ListFastSimModels()
{
  FastSimModelCatalog* cat = FastSimModelCatalog::GetInstance();
  cat->PrintAll();
}

void FastSimMenu::AssignFsModel(std::string regName, std::string modelName)
{
  std::cout << "FastSimMenu   +++++ AssignFsModel called for "
            << regName << " and " << modelName << std::endl;

  FastSimModelCatalog* cat = FastSimModelCatalog::GetInstance();
  FastSimModel* model = cat->GetModel(modelName);
  if(0 == model) { return; }

  G4RegionStore* rs = G4RegionStore::GetInstance();

  for (unsigned int i=0;i<rs->size();i++)
    {
      G4Region* reg=(*rs)[i];

      if (reg==0) continue;
      if (isMatch(regName,reg->GetName()))
        {
          std::cout<<"FastSimMenu   Assigning fast simulation model " << modelName
                   << " to region "<< regName << std::endl;

          G4FastSimulationManager* theFastSimulationManager = reg->GetFastSimulationManager();
          if ( theFastSimulationManager == 0 )
            theFastSimulationManager = new G4FastSimulationManager(reg);
          theFastSimulationManager->AddFastSimulationModel(model);
          theFastSimulationManager->ActivateFastSimulationModel(model->GetName());
        }
    }
  return;
}
