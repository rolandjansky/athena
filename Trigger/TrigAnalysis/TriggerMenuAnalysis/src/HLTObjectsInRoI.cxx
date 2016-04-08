/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  HLTObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/NavigationCore.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <algorithm>
#include <iostream>

using namespace std;

HLTObjectsInRoI::HltFeature::~HltFeature() {}

HLTObjectsInRoI* HLTObjectsInRoI::create() {
  return 0;
}

bool HLTObjectsInRoI::LevelSigId::operator<(const LevelSigId& y) const {
  if (level < y.level ||
      (level == y.level && sig < y.sig)) {
    return true;
  } else {
    return false;
  }
}

MsgStream* HLTObjectsInRoI::s_log = 0;

MsgStream& HLTObjectsInRoI::log() const {
  if (s_log==0) {
    s_log = new MsgStream(Athena::getMessageSvc(), "HLTObjectsInRoI");
  }
  return (*s_log);
}

HLTObjectsInRoI::HLTObjectsInRoI() {
  m_associatedChains.clear();
  m_passedChains.clear();
}

HLTObjectsInRoI::~HLTObjectsInRoI() {
  std::map<LevelSigId, std::vector<HltFeature*> >::iterator p;
  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    std::vector<HltFeature*>::iterator p2;
    for (p2=p->second.begin(); p2!=p->second.end(); ++p2) {
      if (*p2) {
	delete (*p2);
	(*p2) = 0;
      }
    }
    p->second.clear();
  }
  m_hltFeatureDefs.clear();
}

std::vector<std::string> HLTObjectsInRoI::getAssociatedChains(void* feature) const {
  const HltFeature* tmp = findFeature(feature);
  return tmp->getAssociatedChains();
}

std::vector<std::string> HLTObjectsInRoI::getPassedChains(void* feature) const {
  const HltFeature* tmp = findFeature(feature);
  return tmp->getPassedChains();
}

std::vector<TrigStatus> HLTObjectsInRoI::getChains(const void* feature) const {
  const HltFeature* tmp = findFeature(feature);
  if (tmp) {
    return tmp->chains();
  } else {
    return std::vector<TrigStatus>();
  }
}

bool HLTObjectsInRoI::isAssociated(const std::string& chain_name, void* feature) const {
  bool status(false);
  std::vector<std::string> tmp = getAssociatedChains(feature);
  if (find(tmp.begin(), tmp.end(), chain_name) != tmp.end()) {
    status = true;
  }
  return status;
}

bool HLTObjectsInRoI::isPassed(const std::string& chain_name, void* feature) const {
  bool status(false);
  std::vector<std::string> tmp = getPassedChains(feature);
  if (find(tmp.begin(), tmp.end(), chain_name) != tmp.end()) {
    status = true;
  }
  return status;
}

void HLTObjectsInRoI::addRoITE(const HLT::TriggerElement* te) {
  if (find(m_RoITEs.begin(), m_RoITEs.end(), te) == m_RoITEs.end()) {
    m_RoITEs.push_back(te);
  }
}

void HLTObjectsInRoI::addRoITEs(const std::vector<HLT::TriggerElement*>& tes) {
  std::vector<HLT::TriggerElement*>::const_iterator p;
  for (p=tes.begin(); p!=tes.end(); ++p) {
    log() << MSG::DEBUG << "Adding RoITE : " << (*p) << endreq;
    addRoITE(*p);
  }
}

void HLTObjectsInRoI::addTE(const HLT::TriggerElement* te) {
  if (find(m_TEs.begin(), m_TEs.end(), te) == m_TEs.end()) {
    m_TEs.push_back(te);
  }
}

bool HLTObjectsInRoI::isAssociated(const std::string& cname) const {
  if (std::find(m_associatedChains.begin(), 
		m_associatedChains.end(), cname)==m_associatedChains.end()) {
    return false;
  } else {
    return true;
  }
}

bool HLTObjectsInRoI::isPassed(const std::string& cname) const {
  if (std::find(m_passedChains.begin(), 
		m_passedChains.end(), cname)==m_passedChains.end()) {
    return false;
  } else {
    return true;
  }
}


void HLTObjectsInRoI::addAssociatedChain(const std::string& cname) {
  if (std::find(m_associatedChains.begin(), 
		m_associatedChains.end(), cname)==m_associatedChains.end()) {
    m_associatedChains.push_back(cname);
  }
}

void HLTObjectsInRoI::addPassedChain(const std::string& cname) {
  if (std::find(m_passedChains.begin(), 
		m_passedChains.end(), cname)==m_passedChains.end()) {
    m_passedChains.push_back(cname);
  }
}

bool HLTObjectsInRoI::runBySameFex(const  HLTObjectsInRoI* x) const {
  std::map<LevelSigId, std::vector<HltFeature*> >::const_iterator p, q;
  std::map<LevelSigId, std::vector<HltFeature*> > xx;
  LevelSigId lsid;
  int n=0;
  bool status = true;
  const void *o1=0;
  const void *o2=0;

  if (x) xx = x->m_hltFeatureDefs;

  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    lsid = p->first;
    if ( (q=xx.find(lsid)) == xx.end()) {
      // Different object definitions
      log() << MSG::DEBUG << "Different object definitions" << endreq;
      status = false;
      break;
    }
    n = p->second.size();
    if (p->second.size() != q->second.size()) {
      // Different object definitions
      log() << MSG::DEBUG << "Different object definitions" << endreq;
      status = false;
      break;
    }
    for (int i=0; i<n; ++i) {
      o1 = (p->second)[i]->objectAddress();
      o2 = (q->second)[i]->objectAddress();
      if ( (o1!=0 && o2!=0) && (o1!=o2)) {
	// Both pointers are not null and they are different
	log() << MSG::DEBUG << "Both pointers are not null and they are different, "
	     << "step: " << (p->first).sig << " iobj: " << i 
	     << endreq;
	status = false;
	break;
      }
    }
    if (!status) break;
  }
  return status;
}

bool HLTObjectsInRoI::setDataFromTE(int isig, const HLT::TriggerElement* te, 
				    HLT::NavigationCore* navitool, int level) {
  LevelSigId lsid(level, isig);
  std::map<LevelSigId, std::vector<HltFeature*> >::iterator p;

  if ( (p=m_hltFeatureDefs.find(lsid)) != m_hltFeatureDefs.end()) {
    std::vector<HltFeature*>::iterator p2;
    for (p2=p->second.begin(); p2!=p->second.end(); ++p2) {
      if ( (*p2)->getObject(te, navitool) == 0) {
	log() << MSG::DEBUG << "Successfully retrieved object from Navigation (level="
	     << level << ", sig=" << isig << ")" << endreq;
      }
    }
  } else {
    log() << MSG::DEBUG << "TE is there at level="<< level << ", isig="<< isig 
	 << " but not specified to read" << endreq;
    log() << MSG::DEBUG << "Attached objects are the following: " << endreq;
    unsigned int n = te->getFeatureAccessHelpers().size();
    unsigned int clid = 0;
    for (unsigned int i=0; i<n; ++i) {
      clid = te->getFeatureAccessHelpers()[i].getCLID();
      std::string clsname="";
      log() << MSG::DEBUG << "TE["<<i<<"]: "
	   << "clid=" << clid << "class_name=" << clsname << endreq;
    }
    return false;
  }
  return true;
}

bool HLTObjectsInRoI::isInSameRoI(const HLTObjectsInRoI* /*x*/, 
				  HLT::NavigationCore* /*navitool*/) const {
  return false;  
}

bool HLTObjectsInRoI::isInSameRoI(const HLT::TriggerElement* te, 
				  HLT::NavigationCore* /*navitool*/) const {
  bool status=false;
  std::vector<const HLT::TriggerElement*>::const_iterator p_roi;
  std::vector<HLT::TriggerElement*>::const_iterator p;

  std::list<const HLT::TriggerElement*> tes;
  std::list<const HLT::TriggerElement*>::iterator p_te;

  const vector<HLT::TriggerElement*> rois = HLT::NavigationCore::getRoINodes(te);
  if (rois.size() == 1) {
    for (p_roi=m_RoITEs.begin(); p_roi!=m_RoITEs.end(); ++p_roi) {
      if ( (*p_roi) == rois[0]) {
	status = true;
	break;
      }
    }
  } else {
    status = false;
  }

  return status;
}

int HLTObjectsInRoI::setDataFromCombination(const Trig::Combination& comb, 
					    const std::string& chain_name, 
					    const Trig::TrigDecisionTool& tdt) {
  std::map<LevelSigId, std::vector<HltFeature*> >::iterator p;
  std::vector<HltFeature*>::iterator q;
  bool passed=false;
  bool foundit=false;
  const HLT::TriggerElement* te1=0;

  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    for (q=p->second.begin(); q!=p->second.end(); ++q) {
      //      if ( (*q)->objectAddress() == 0) {
      if (p->first.sig>0 && (*q)->getObject(comb, passed) == 0) {
	// Add the protection inside getObject. 
	// Need to call getObject for all combination/TE in order to get 
	// the foundit result
	foundit = true;
      }
      if (te1==0) {
	te1 = (*q)->getTEforObj(comb);
      }
      log() << MSG::DEBUG << "Is TE to get L1 RoI there? Te=" << te1 << endreq;
      //      }
      if ( (*q)->objectAddress() != 0) {
	(*q)->addChain(chain_name, passed);
      }
    }
  }
  if (te1) {
    for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
      if (p->first.sig==0) {
	for (q=p->second.begin(); q!=p->second.end(); ++q) {
	  (*q)->getRoIObject(tdt, te1);
	}
      }
    }
  }
  if (!foundit) {
    log() << MSG::DEBUG 
	  << "Couldn't find object for combination of chain: "
	  << chain_name << endreq;
  }

  return 0;
}

int HLTObjectsInRoI::updateChainStatus(const std::string& chain_name, 
				       const Trig::Combination& comb) {
  std::map<LevelSigId, std::vector<HltFeature*> >::const_iterator p;
  std::vector<HltFeature*>::const_iterator q;
  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    for (q=p->second.begin(); q!=p->second.end(); ++q) {
      if (*q) {
	const void* obj = (*q)->objectAddress();
	const HLT::TriggerElement* te = (*q)->getTEforObj(comb);
	if (obj && te) {
	  bool pass=false;
	  if (te->getActiveState()) pass = true;
	  (*q)->addChain(chain_name, pass);
	}
      }
    }
  }

  return 0;
}

int HLTObjectsInRoI::updateChainStatus(const std::string& chain_name, 
				       const Trig::FeatureContainer& fc) {
  std::map<LevelSigId, std::vector<HltFeature*> >::const_iterator p;
  std::vector<HltFeature*>::const_iterator q;
  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    for (q=p->second.begin(); q!=p->second.end(); ++q) {
      if (*q) {
	const void* obj = (*q)->objectAddress();
	const HLT::TriggerElement* te = (*q)->getTEforObj(fc);
	if (obj && te) {
	  bool pass=false;
	  if (te->getActiveState()) pass = true;
	  (*q)->addChain(chain_name, pass);
	}
      }
    }
  }

  return 0;
}

bool HLTObjectsInRoI::addFeatureChainStatus(const void* feature, 
					    const TrigStatus& chain_status) {
  HltFeature* f = findFeature(feature);
  if (f) {
    bool pass=false;
    if (chain_status.isPassed()) pass = true;
    f->addChain(chain_status.name(), pass);
    return true;
  } else {
    return false;
  }
}

void HLTObjectsInRoI::dump() const {
  log() << MSG::DEBUG << "Dump HLTObjectsInRoI" << endreq;
  std::map<LevelSigId, std::vector<HltFeature*> >::const_iterator p;
  std::vector<HltFeature*>::const_iterator q;
  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    for (q=p->second.begin(); q!=p->second.end(); ++q) {
      dumpFeature(*q);
    }
  }
//   std::vector<std::string>::const_iterator p;
//   log() << MSG::DEBUG << "Associated chains: " << endreq;
//   for (p=m_associatedChains.begin(); p!=m_associatedChains.end(); ++p) {
//     log() << MSG::DEBUG << "   - " << (*p) << endreq;
//   }
//   log() << MSG::DEBUG << "Passed chains: " << endreq;
//   for (p=m_passedChains.begin(); p!=m_passedChains.end(); ++p) {
//     log() << MSG::DEBUG << "   - " << (*p) << endreq;
//   }
}

void HLTObjectsInRoI::dumpFeature(const HltFeature* feature) const {
  const std::vector<TrigStatus>& chains = feature->chains();
  std::vector<TrigStatus>::const_iterator p_chain;

  log() << MSG::DEBUG << "  Feature: " << feature->name() << " pointer=" << feature->objectAddress() << endreq;
  log() << MSG::DEBUG << "    Associated chains: " << endreq;
  for (p_chain=chains.begin(); p_chain!=chains.end(); ++p_chain) {
    log() << MSG::DEBUG << "    - " << p_chain->name() << " : " << p_chain->isPassed() << endreq;
  }
}

const HLTObjectsInRoI::HltFeature* HLTObjectsInRoI::findFeature(const void *obj) const {
  std::map<LevelSigId, std::vector<HltFeature*> >::const_iterator p;
  std::vector<HltFeature*>::const_iterator q;

  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    for (q=p->second.begin(); q!=p->second.end(); ++q) {
      if ( (*q)->objectAddress() == obj) {
	return *q;
      }
    }
  }
  return 0;
}

HLTObjectsInRoI::HltFeature* HLTObjectsInRoI::findFeature(const void *obj) {
  std::map<LevelSigId, std::vector<HltFeature*> >::iterator p;
  std::vector<HltFeature*>::iterator q;

  for (p=m_hltFeatureDefs.begin(); p!=m_hltFeatureDefs.end(); ++p) {
    for (q=p->second.begin(); q!=p->second.end(); ++q) {
      if ( (*q)->objectAddress() == obj) {
	return *q;
      }
    }
  }
  return 0;
}

void HLTObjectsInRoI::HltFeature::addChain(const string& chain_name, 
					   bool passed) {
  std::vector<TrigStatus>::iterator p;
  bool foundit = false;
  for (p=m_chains.begin(); p!=m_chains.end(); ++p) {
    if (p->name() == chain_name) {
      if (passed) {
	p->setStatus(1);
      } else {
	p->setStatus(0);
      }
      foundit = true;
      break;
    }
  }
  if (!foundit) {
    TrigStatus tmp(chain_name, passed);
    m_chains.push_back(tmp);
  }
}

vector<std::string> HLTObjectsInRoI::HltFeature::getAssociatedChains() const {
  std::vector<TrigStatus>::const_iterator p;
  std::vector<std::string> tmp;
  for (p=m_chains.begin(); p!=m_chains.end(); ++p) {
    tmp.push_back(p->name());
  }
  return tmp;
}

vector<std::string> HLTObjectsInRoI::HltFeature::getPassedChains() const {
  std::vector<TrigStatus>::const_iterator p;
  std::vector<std::string> tmp;
  for (p=m_chains.begin(); p!=m_chains.end(); ++p) {
    if (p->isPassed()) tmp.push_back(p->name());
  }
  return tmp;
}

