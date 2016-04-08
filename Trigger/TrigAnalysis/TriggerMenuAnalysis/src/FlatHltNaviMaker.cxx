/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  FlatHltNaviMaker.cxx
*/
#include <vector>
#include "TriggerMenuAnalysis/FlatHltNaviMaker.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"

using namespace std;


FlatHltNaviMaker::FlatHltNaviMaker(const std::string& name, 
				   ISvcLocator* svcloc) :
  AthAlgorithm(name, svcloc), m_RoILinksCnvTool("RoILinksCnvTool") {
}

FlatHltNaviMaker::~FlatHltNaviMaker() {
}

StatusCode FlatHltNaviMaker::initialize() {
  AthAlgorithm::initialize().ignore();
  
  if (m_RoILinksCnvTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve RoILinksCnvTool" << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode FlatHltNaviMaker::beginRun() {
  return m_RoILinksCnvTool->beginRun();
}

StatusCode FlatHltNaviMaker::execute() {
  std::vector<ChainEntry> chains;
  RoILinks roi_links;
  m_RoILinksCnvTool->fill(chains, roi_links);
  m_RoILinksCnvTool->record(chains, roi_links);

  return StatusCode::SUCCESS;
}

StatusCode FlatHltNaviMaker::finalize() {
  AthAlgorithm::finalize().ignore();
  return StatusCode::SUCCESS;
}

