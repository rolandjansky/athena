/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/DataProxy.h"
#include "SGTools/SGIFolder.h"
#include "StoreGate/StoreGateSvc.h"

#include "SGDeleteAlg.h"

using namespace std;
using namespace SG;

SGDeleteAlg::SGDeleteAlg(const std::string& name, ISvcLocator* isl) :
  AthAlgorithm(name, isl),
  m_targetStore("StoreGateSvc",name),
  m_toDelete("SG::Folder/ToDelete", this)
{
  declareProperty("TargetStore", m_targetStore, 
		  "the store containing the items to be deleted");
  declareProperty("ToDelete", m_toDelete,
		  "the SG::IFolder with a list of items to be deleted");
}

SGDeleteAlg::~SGDeleteAlg()
{}

StatusCode
SGDeleteAlg::initialize() {
  StatusCode sc;
  ATH_MSG_VERBOSE("Initializing " << name());
  (sc=m_targetStore.retrieve()).isSuccess() &&
    (sc=m_toDelete.retrieve()).isSuccess();
  return sc;
}

StatusCode
SGDeleteAlg::execute() 
{
  ATH_MSG_DEBUG("In execute");
  SG::IFolder::const_iterator i(m_toDelete->begin()); 
  SG::IFolder::const_iterator iEnd(m_toDelete->end());

  while (i != iEnd) { 
    //find proxy matching folder item and reset (delete) its contents
    DataProxy* p2d(m_targetStore->proxy(i->id(), i->key()));
    if (0 != p2d) {
      m_targetStore->clearProxyPayload(p2d);
      ATH_MSG_VERBOSE("cleared data object " 
		      << p2d->clID() 
		      << '/' << p2d->name());
    }
    ++i;
  }
  return StatusCode::SUCCESS;
}
