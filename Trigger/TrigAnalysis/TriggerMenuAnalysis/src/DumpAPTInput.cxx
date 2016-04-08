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
  m_outputFile(), 
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"), 
  m_L1NameIdMap(), m_L2NameIdMap(), m_EFNameIdMap(), 
  m_L1_TBP(), m_L1_TAP(), m_L1_TAV(), 
  m_L2_Raw(), m_L2_Rerun(), m_EF_Raw(), m_EF_Rerun() {
  declareProperty("OutputFileName", m_outputFileName="apt_input.txt", 
		  "Output file name");
  m_chainGroup_AllL1 = 0;
  m_chainGroup_AllL2 = 0;
  m_chainGroup_AllEF = 0;
}

DumpAPTInput::~DumpAPTInput() {
}

StatusCode DumpAPTInput::initialize() {
  if (m_trigDecisionTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve TrigDecisionTool" << endreq;
  }

  m_outputFile.open(m_outputFileName.c_str());

  m_chainGroup_AllL1 = m_trigDecisionTool->getChainGroup("L1_.*");
  m_chainGroup_AllL2 = m_trigDecisionTool->getChainGroup("L2_.*");
  m_chainGroup_AllEF = m_trigDecisionTool->getChainGroup("EF_.*");

  return StatusCode::SUCCESS;
}

StatusCode DumpAPTInput::execute() {
  reset();

  readL1Map(m_chainGroup_AllL1->getListOfTriggers(), m_L1NameIdMap);
  readHLTMap(m_chainGroup_AllL2->getListOfTriggers(), m_L2NameIdMap);
  readHLTMap(m_chainGroup_AllEF->getListOfTriggers(), m_EFNameIdMap);

  std::map<int, std::string>::const_iterator p;
  for (p=m_L1NameIdMap.begin(); p!=m_L1NameIdMap.end(); ++p) {
    processL1(p->first, p->second);
  }
  for (p=m_L2NameIdMap.begin(); p!=m_L2NameIdMap.end(); ++p) {
    processL2(p->first, p->second);
  }
  for (p=m_EFNameIdMap.begin(); p!=m_EFNameIdMap.end(); ++p) {
    processEF(p->first, p->second);
  }

  return StatusCode::SUCCESS;
}

void DumpAPTInput::readL1Map(const std::vector<std::string>& names, 
			     std::map<int, std::string>& name_map) {
  Trig::ExpertMethods* m = m_trigDecisionTool->ExperimentalAndExpertMethods();
  if (m==0) {
    msg(MSG::WARNING) << "Cannot get TDT expert methods" << endreq;
  } else {
    m->enable();
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
}

void DumpAPTInput::readHLTMap(const std::vector<std::string>& names, 
			    std::map<int, std::string>& name_map) {
  Trig::ExpertMethods* m = m_trigDecisionTool->ExperimentalAndExpertMethods();
  if (m==0) {
    msg(MSG::WARNING) << "Cannot get TDT expert methods for HLT" << endreq;
  } else {
    m->enable();
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
}


void DumpAPTInput::processL1(int id, const std::string& name) {
  bool a = m_trigDecisionTool->isPassed(name, 
				       TrigDefs::L1_isPassedBeforePrescale);
  bool b = m_trigDecisionTool->isPassed(name, 
				       TrigDefs::L1_isPassedAfterPrescale);
  bool c = m_trigDecisionTool->isPassed(name, TrigDefs::L1_isPassedAfterVeto);
  m_L1_TBP.set(id, a);
  m_L1_TAP.set(id, b);
  m_L1_TAV.set(id, c);
  msg(MSG::DEBUG) << "Getting bits for " << name << endreq;
}

void DumpAPTInput::processL2(int id, const std::string& name) {
  bool a = m_trigDecisionTool->isPassed(name, TrigDefs::L2_passedRaw);
  bool b = m_trigDecisionTool->isPassed(name, TrigDefs::L2_resurrected);
  if (id < N_L2_BITS) {
    m_L2_Raw.set(id, a);
    m_L2_Rerun.set(id, b);
  }
  msg(MSG::DEBUG) << "Getting bits for " << name << endreq;
}

void DumpAPTInput::processEF(int id, const std::string& name) {
  bool a = m_trigDecisionTool->isPassed(name, TrigDefs::EF_passedRaw);
  bool b = m_trigDecisionTool->isPassed(name, TrigDefs::EF_resurrected);
  if (id < N_EF_BITS) {
    m_EF_Raw.set(id, a);
    m_EF_Rerun.set(id, b);
  }
  msg(MSG::DEBUG) << "Getting bits for " << name << endreq;
}

void DumpAPTInput::dump() {
}

void DumpAPTInput::reset() {
  m_L1_TBP.reset();
  m_L1_TAP.reset();
  m_L1_TAV.reset();

  m_L2_Raw.reset();
  m_L2_Rerun.reset();

  m_EF_Raw.reset();
  m_EF_Rerun.reset();
}

StatusCode DumpAPTInput::finalize() {
  m_outputFile.close();

  return StatusCode::SUCCESS;
}

