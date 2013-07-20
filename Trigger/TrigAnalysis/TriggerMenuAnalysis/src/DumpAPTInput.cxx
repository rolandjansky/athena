/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  DumpAPTInput.cxx
*/
#include <algorithm>
#include <functional>
#include "TriggerMenuAnalysis/DumpAPTInput.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfHLTData/HLTChain.h"

using namespace std;


DumpAPTInput::DumpAPTInput(const std::string& name, 
				   ISvcLocator* svcloc) :
  AthAlgorithm(name, svcloc), 
  mTrigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool") {
  declareProperty("OutputFileName", mOutputFileName="apt_input.txt", 
		  "Output file name");
}

DumpAPTInput::~DumpAPTInput() {
}

StatusCode DumpAPTInput::initialize() {
  if (mTrigDecisionTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve TrigDecisionTool" << endreq;
  }

  mOutputFile.open(mOutputFileName.c_str());

  mChainGroup_AllL1 = mTrigDecisionTool->getChainGroup("L1_.*");
  mChainGroup_AllL2 = mTrigDecisionTool->getChainGroup("L2_.*");
  mChainGroup_AllEF = mTrigDecisionTool->getChainGroup("EF_.*");

  return StatusCode::SUCCESS;
}

StatusCode DumpAPTInput::execute() {
  reset();

  readL1Map(mChainGroup_AllL1->getListOfTriggers(), mL1NameIdMap);
  readHLTMap(mChainGroup_AllL2->getListOfTriggers(), mL2NameIdMap);
  readHLTMap(mChainGroup_AllEF->getListOfTriggers(), mEFNameIdMap);

  std::map<int, std::string>::const_iterator p;
  for (p=mL1NameIdMap.begin(); p!=mL1NameIdMap.end(); ++p) {
    processL1(p->first, p->second);
  }
  for (p=mL2NameIdMap.begin(); p!=mL2NameIdMap.end(); ++p) {
    processL2(p->first, p->second);
  }
  for (p=mEFNameIdMap.begin(); p!=mEFNameIdMap.end(); ++p) {
    processEF(p->first, p->second);
  }

  return StatusCode::SUCCESS;
}

void DumpAPTInput::readL1Map(const std::vector<std::string>& names, 
			     std::map<int, std::string>& name_map) {
  Trig::ExpertMethods* m = mTrigDecisionTool->ExperimentalAndExpertMethods();
  if (m) m->enable();
  else {
    msg(MSG::WARNING) << "Cannot get TDT expert methods" << endreq;
  }
  std::vector<std::string>::const_iterator p;
  for (p=names.begin(); p!=names.end(); ++p) {
    const TrigConf::TriggerItem* item = m->getItemConfigurationDetails(*p);
    if (item) {
      int id = item->ctpId();
      name_map[id] = *p;
    } else {
      msg(MSG::WARNING) << "Cannot find TrigConif::TriggerItem for " 
			<< *p << endreq;
    }
  }
}

void DumpAPTInput::readHLTMap(const std::vector<std::string>& names, 
			    std::map<int, std::string>& name_map) {
  Trig::ExpertMethods* m = mTrigDecisionTool->ExperimentalAndExpertMethods();
  if (m) m->enable();
  else {
    msg(MSG::WARNING) << "Cannot get TDT expert methods for HLT" << endreq;
  }
  std::vector<std::string>::const_iterator p;
  for (p=names.begin(); p!=names.end(); ++p) {
    const TrigConf::HLTChain* chain = m->getChainConfigurationDetails(*p);
    if (chain) {
      int id = chain->chain_counter();
      name_map[id] = *p;
    } else {
      msg(MSG::WARNING) << "Cannot find TrigConif::HLTDChain for " 
			<< *p << endreq;
    }
  }
}


void DumpAPTInput::processL1(int id, const std::string& name) {
  bool a = mTrigDecisionTool->isPassed(name, 
				       TrigDefs::L1_isPassedBeforePrescale);
  bool b = mTrigDecisionTool->isPassed(name, 
				       TrigDefs::L1_isPassedAfterPrescale);
  bool c = mTrigDecisionTool->isPassed(name, TrigDefs::L1_isPassedAfterVeto);
  mL1_TBP.set(id, a);
  mL1_TAP.set(id, b);
  mL1_TAV.set(id, c);
  msg(MSG::DEBUG) << "Getting bits for " << name << endreq;
}

void DumpAPTInput::processL2(int id, const std::string& name) {
  bool a = mTrigDecisionTool->isPassed(name, TrigDefs::L2_passedRaw);
  bool b = mTrigDecisionTool->isPassed(name, TrigDefs::L2_resurrected);
  if (id < N_L2_BITS) {
    mL2_Raw.set(id, a);
    mL2_Rerun.set(id, b);
  }
  msg(MSG::DEBUG) << "Getting bits for " << name << endreq;
}

void DumpAPTInput::processEF(int id, const std::string& name) {
  bool a = mTrigDecisionTool->isPassed(name, TrigDefs::EF_passedRaw);
  bool b = mTrigDecisionTool->isPassed(name, TrigDefs::EF_resurrected);
  if (id < N_EF_BITS) {
    mEF_Raw.set(id, a);
    mEF_Rerun.set(id, b);
  }
  msg(MSG::DEBUG) << "Getting bits for " << name << endreq;
}

void DumpAPTInput::dump() {
}

void DumpAPTInput::reset() {
  mL1_TBP.reset();
  mL1_TAP.reset();
  mL1_TAV.reset();

  mL2_Raw.reset();
  mL2_Rerun.reset();

  mEF_Raw.reset();
  mEF_Rerun.reset();
}

StatusCode DumpAPTInput::finalize() {
  mOutputFile.close();

  return StatusCode::SUCCESS;
}

