/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  IndexFinderBase.cxx
*/
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#define private public
#define protected public
#include "TrigDecisionTool/Combination.h"
#include "TrigNavigation/NavigationCore.h"
#undef private
#undef public

#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuAnalysis/IndexFinderBase.h"

using namespace std;

IndexFinderBase::IndexFinderBase() {
}

int IndexFinderBase::subTypeIndex(unsigned int clid, const std::string& label, 
				  const Trig::Combination& comb) const {
  HLT::NavigationCore* navi = comb.navigation();

  const std::map<unsigned int, std::map< uint16_t, std::string> >& lookup = 
    navi->m_lookupLabels;
  std::map<unsigned int, std::map< uint16_t, std::string> >::const_iterator p1;
  std::map< uint16_t, std::string >::const_iterator p2;

  int index=-1;
  if ( (p1=lookup.find(clid)) != lookup.end()) {
    for (p2=p1->second.begin(); p2!=p1->second.end(); ++p2) {
      if (p2->second == label) {
	index = p2->first;
	break;
      }
    }
  }

  return index;
}

void IndexFinderBase::fillIndexes(std::vector<FeatureIndex>& index, 
				  const HLT::TriggerElement* te, 
				  unsigned int clid, 
				  unsigned int subtypeOfLabel, 
				  const std::string& label, 
				  MsgStream* /*logger*/) {
				  
  unsigned int i;
  bool status = te->getActiveState();
  const HLT::TriggerElement::FeatureVec& v = te->getFeatureAccessHelpers();

  std::string te_label="";
  TrigConf::HLTTriggerElement::getLabel(te->getId(), te_label);
  //  (*logger) << MSG::DEBUG 
  //	    << "  N helpers for TE (" << te_label
  //	    << "): " << v.size() 
  //	    << " clid=" << clid << endreq;
  HLT::TriggerElement::FeatureVec::const_iterator p_f;
  for (p_f=v.begin(); p_f!=v.end(); ++p_f) {
    //    (*logger) << MSG::DEBUG << "    comparing against clid=" << p_f->getCLID() 
    //	      << " (ref=" << clid << ")" << endreq;
    if (p_f->getCLID() == clid) {
      //      (*logger) << MSG::DEBUG << "    found FeatureAccessHelper for CLID: " 
      //		<< clid << endreq;
      const HLT::TriggerElement::ObjectIndex& oi = p_f->getIndex();
      if (label.empty() || oi.subTypeIndex() == subtypeOfLabel) {
	//	(*logger) << MSG::DEBUG << "    objectsBegin/End = " 
	//	       << oi.objectsBegin() << "/" << oi.objectsEnd() << endreq;
	for (i=oi.objectsBegin(); i!=oi.objectsEnd(); ++i) {
	  index.push_back(FeatureIndex(i, status));
	}
      }
    } else {
      //(*logger) << MSG::DEBUG << "    Wrong CLID: " << p_f->getCLID() << endreq;
    }
  }
}

