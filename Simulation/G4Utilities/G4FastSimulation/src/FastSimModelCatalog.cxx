/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FastSimulation/FastSimModelCatalog.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

FastSimModelCatalog* FastSimModelCatalog::thePointer = 0;

FastSimModelCatalog::FastSimModelCatalog():m_msg("FastSimModelCatalog")
{
}

FastSimModelCatalog* FastSimModelCatalog::GetInstance()
{
  if (!thePointer) {
    thePointer = new FastSimModelCatalog;
  }
  return thePointer;
}

void FastSimModelCatalog::RegisterModel(FastSimModel* model)
{
  if (theCatalog.find(model->GetName())!=theCatalog.end())
    {
      ATH_MSG_INFO( "Redefining Fast Simulation Model " << model->GetName() );
    }
  theCatalog[model->GetName()]=model;
  return;
}

FastSimModel* FastSimModelCatalog::GetModel(std::string name)
{
  fsModelCatalog::iterator it = theCatalog.find(name);
  if (it==theCatalog.end())
  {
    ATH_MSG_ERROR( "Fast Simulation Model " << name << " not found!" );
    return 0;
  }
  return (*it).second;
}

void FastSimModelCatalog::PrintAll()
{
  ATH_MSG_INFO( "Printing the list of fast simulation models:" );
  ATH_MSG_INFO( " Number of models :: " << theCatalog.size() );
  for (fsModelCatalog::const_iterator it=theCatalog.begin();it!=theCatalog.end();it++) {
    ATH_MSG_INFO( " --- model    " << (*it).first );
  }
  return;
}
