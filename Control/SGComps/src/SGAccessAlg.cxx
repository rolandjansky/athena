/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/DataProxy.h"
#include "SGTools/SGIFolder.h"
#include "StoreGate/StoreGateSvc.h"

#include "SGAccessAlg.h"

SGAccessAlg::SGAccessAlg(const std::string& name, ISvcLocator* isl) :
  AthAlgorithm(name, isl),
  m_targetStore("StoreGateSvc",name),
  m_toLoad("SG::Folder/ToLoad", this)
{
  declareProperty("TargetStore", 
                  m_targetStore, 
                  "the store containing the items to be loaded");
  declareProperty("ToLoad", 
                  m_toLoad,
                  "the SG::IFolder with a list of items to be loaded");
}

SGAccessAlg::~SGAccessAlg()
{}

StatusCode
SGAccessAlg::initialize() 
{
  StatusCode sc;
  ATH_MSG_VERBOSE("Initializing " << name());

  (sc=m_targetStore.retrieve()).isSuccess() &&
    (sc=m_toLoad.retrieve()).isSuccess();
  return sc;
}

StatusCode
SGAccessAlg::execute() 
{
  ATH_MSG_DEBUG("In execute");
  SG::IFolder::const_iterator i(m_toLoad->begin()); 
  SG::IFolder::const_iterator iEnd(m_toLoad->end());

  while (i != iEnd) { 
    ATH_MSG_DEBUG("loading [" << i->id() << "#" << i->key() << "]...");
    //find proxy matching folder item and access its contents
    SG::DataProxy* p = m_targetStore->proxy(i->id(), i->key());
    if (0 != p) {
      DataObject *dobj = p->accessData();
      ATH_MSG_VERBOSE("loaded data object " 
                      << p->clID() 
                      << '/' << p->name()
                      << " @" << dobj);
      if (0 == dobj) {
        ATH_MSG_INFO("problem loading data object ["
                     << i->id() << "#" << i->key() << "]!");
      }
    }
    ++i;
  }
  return StatusCode::SUCCESS;
}
