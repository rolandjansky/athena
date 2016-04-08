/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_INDEXFINDER_H
#define TRIGGERMENUANALYSIS_INDEXFINDER_H
/*
  IndexFinder.h
*/
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "TriggerMenuAnalysis/IndexFinderBase.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

template<class Feature_t, class Cont_t, class Elem_t=Feature_t>
class IndexFinder : public IndexFinderBase {
public:
  typedef Feature_t feature_type;
  typedef Cont_t container_type;
  typedef Elem_t element_type;

public:
  IndexFinder(const std::string& cont_key, 
	      StoreGateSvc& sg_svc, 
	      MsgStream& msg) : 
    IndexFinderBase(),
    m_contKey(cont_key), m_cont(0), m_sgsvc(&sg_svc), m_label(""), m_log(&msg), m_TDT(0), m_L1RoIName(""), m_ROIWord(-1) {}
  IndexFinder(const Cont_t* x, 
	      StoreGateSvc& sg_svc, 
	      MsgStream& msg) : 
    IndexFinderBase(),
    m_contKey(""), m_cont(x), m_sgsvc(&sg_svc), m_label(""), m_log(&msg), m_TDT(0), m_L1RoIName(""), m_ROIWord(-1) {}
  IndexFinder(const std::string& cont_key) :
    IndexFinderBase(),
    m_contKey(cont_key), m_cont(0), m_sgsvc(0), m_label(""), m_log(0), m_TDT(0), m_L1RoIName(""), m_ROIWord(-1) {}
  IndexFinder(const std::string& cont_key, const std::string& label) :
    IndexFinderBase(),
    m_contKey(cont_key), m_cont(0), m_sgsvc(0), m_label(label), m_log(0), m_TDT(0), m_L1RoIName(""), m_ROIWord(-1) {}
  IndexFinder(const Cont_t* x) :
    IndexFinderBase(),
    m_contKey(""), m_cont(x), m_sgsvc(0), m_label(""), m_log(0), m_TDT(0), m_L1RoIName(""), m_ROIWord(-1) {}
  ~IndexFinder() {}

  void setLabel(const std::string& label) { m_label = label; }

  const std::string& label() const { return m_label; }
  FeatureIndex findIndex(const Trig::Combination& comb, 
			 std::string& te_label);
  std::vector<FeatureIndex> findIndexes(const Trig::Combination& comb, 
					std::string& te_label);

//   FeatureIndex findIndex(const Trig::Combination& comb, 
// 			 MsgStream* logger);
//   FeatureIndex findIndex(const Trig::Combination& comb, 
// 			 StoreGateSvc& sg_svc, MsgStream& msg);

//   std::vector<FeatureIndex> findIndexes(const Trig::Combination& comb, 
// 					MsgStream* logger);

  std::string elementTypeName() const {
    return ClassID_traits<Feature_t>::typeName();
  }
  std::string typeLabel() const {
    if (m_label.empty()) {
    return ClassID_traits<Feature_t>::typeName();
    } else {
      return ClassID_traits<Feature_t>::typeName()+"_"+m_label;
    }
  }

  int findIndex(const Elem_t* addr);

  void clear() { m_cont = 0; }

  void setTrigDecisionTool(Trig::TrigDecisionTool* tdt) { m_TDT=tdt; }
  void setLogger(MsgStream* logger) { m_log = logger; }
  void setL1RoIName(std::string RoIName) { m_L1RoIName=RoIName; }
  virtual void setL1ROIWord(const long RoIWord) { m_ROIWord=RoIWord; }
  bool checkROIWord(const HLT::TriggerElement* te);

private:
  std::string m_contKey;
  const Cont_t* m_cont;
  StoreGateSvc* m_sgsvc;
  std::string m_label;
  MsgStream* m_log;
  Trig::TrigDecisionTool* m_TDT;
  std::string m_L1RoIName;
  long m_ROIWord;
};

#include "TriggerMenuNtuple/FeatureIndex.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Feature.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

template<class Feature_t, class Cont_t, class Elem_t>
FeatureIndex 
IndexFinder<Feature_t,Cont_t,Elem_t>::findIndex(const Trig::Combination& comb, 
						std::string& te_label) { 
  std::vector<Trig::Feature<Feature_t> > fs = 
    comb.get<Feature_t>(m_label, TrigDefs::alsoDeactivateTEs);

  int index = 0;
  bool status(false);
  //(*m_log) << MSG::DEBUG << "Feature size is: " << fs.size() << endreq;
  if (fs.size() > 0) {
    //    const Feature_t* x = fs[0].cptr();
    HLT::TriggerElement* te = NULL;
    for(unsigned int i=0; i<fs.size(); ++i) {
      if( checkROIWord(fs[i].te()) ) {
	te = const_cast<HLT::TriggerElement*>( fs[i].te() );
	break;
      }
    }
    //(*m_log) << MSG::DEBUG << "TE = " << te << endreq;
    if( te ) {
      te_label="";
      TrigConf::HLTTriggerElement::getLabel(te->getId(), te_label);
      status = te->getActiveState();
      const HLT::TriggerElement::FeatureVec& v = te->getFeatureAccessHelpers();
      HLT::TriggerElement::FeatureVec::const_iterator p_f;
      unsigned int clid = ClassID_traits<Feature_t>::ID();
      index = -2;
      //(*m_log) << MSG::DEBUG << "Size of feature access helper (clid="
      //<< clid << " : " << v.size() << endreq;
      for (p_f=v.begin(); p_f!=v.end(); ++p_f) {
	if (p_f->getCLID() == clid) {
	  const HLT::TriggerElement::ObjectIndex& oi = p_f->getIndex();
	  index = oi.objectsBegin();
	  break;
	}
      }
    } else {
      status = false;
      index = -1;
    }
  } else {
    status = false;
    index = -2;
  }

  FeatureIndex findex(index, status);
  return findex;
}


template<class Feature_t, class Cont_t, class Elem_t>
bool IndexFinder<Feature_t, Cont_t, Elem_t>::checkROIWord(const HLT::TriggerElement* te) {
  bool status = false;

  if (m_ROIWord < 0) status = true;

  if( m_L1RoIName == "Muon_ROI" ) {
    std::vector<Trig::Feature<Muon_ROI> > rois = 
      m_TDT->ancestors<Muon_ROI>(te, "initialRoI", TrigDefs::alsoDeactivateTEs);
    std::vector<Trig::Feature<Muon_ROI> >::const_iterator p_roi;
    for (p_roi=rois.begin(); p_roi!=rois.end(); ++p_roi) {
      if( p_roi->cptr() && (m_ROIWord == (long)(p_roi->cptr()->getROIWord() ) ) ) {
	status = true;
      }
    }
  } else if( m_L1RoIName == "EmTau_ROI" ) {
    std::vector<Trig::Feature<EmTau_ROI> > rois = 
      m_TDT->ancestors<EmTau_ROI>(te, "initialRoI", TrigDefs::alsoDeactivateTEs);
    std::vector<Trig::Feature<EmTau_ROI> >::const_iterator p_roi;
    for (p_roi=rois.begin(); p_roi!=rois.end(); ++p_roi) {
      if( p_roi->cptr() && (m_ROIWord == (long)(p_roi->cptr()->getROIWord() ) ) ) {
	status = true;
      }
    }
  } else if( m_L1RoIName == "Jet_ROI" ) {
    std::vector<Trig::Feature<Jet_ROI> > rois = 
      m_TDT->ancestors<Jet_ROI>(te, "initialRoI", TrigDefs::alsoDeactivateTEs);
    std::vector<Trig::Feature<Jet_ROI> >::const_iterator p_roi;
    for (p_roi=rois.begin(); p_roi!=rois.end(); ++p_roi) {
      if( p_roi->cptr() && (m_ROIWord == (long)(p_roi->cptr()->getROIWord() ) ) ) {
	status = true;
      }
    }
  }

  return status;
}


// template<class Feature_t, class Cont_t, class Elem_t>
// FeatureIndex IndexFinder<Feature_t, Cont_t, Elem_t>::findIndex(const Trig::Combination& comb, 
// 							       MsgStream* logger) {
//   this->m_log = logger;
//   return findIndex(comb);
// }

// template<class Feature_t, class Cont_t, class Elem_t>
// FeatureIndex IndexFinder<Feature_t,Cont_t,Elem_t>::findIndex(const Trig::Combination& comb, 
// 							     StoreGateSvc& sg_svc, MsgStream& msg) {
//   this->m_sgsvc = &sg_svc;
//   this->m_log = &msg;
//   return findIndex(comb, &msg);
// }

template<class Feature_t, class Cont_t, class Elem_t>
int IndexFinder<Feature_t,Cont_t,Elem_t>::findIndex(const Elem_t* addr) {
  int index=-1;
  bool foundit=false;

  if (m_cont == 0) {
    if (m_sgsvc->retrieve(m_cont, m_contKey).isFailure()) {
      (*m_log) << MSG::WARNING << "Cannot retrieve Container " << m_contKey << endreq;
    } else {
      (*m_log) << MSG::VERBOSE 
	     << "Successfully retrieved container: " << m_contKey << endreq;
    }
  }

  if (m_cont) {
    typename Cont_t::const_iterator q;
//     (*m_log) << MSG::VERBOSE << "Search element with pointer "
// 	   << std::hex << addr << std::dec 
// 	   << " in container (size: " << m_cont->size() << ")" << endreq;
    index = -1;
    for (q=m_cont->begin(); q!=m_cont->end(); ++q) {
      index ++;
      if (addr == (*q)) {
// 	(*m_log) << MSG::VERBOSE
// 	       << "Found element in the container with the same pointer" 
// 	       << endreq;
	foundit = true;
	break;
      }
    }
  }
  if (!foundit) {
    index = -1;
  } 

  return index;
}

template<class Feature_t, class Cont_t, class Elem_t>
std::vector<FeatureIndex> 
IndexFinder<Feature_t,Cont_t,Elem_t>::findIndexes(const Trig::Combination& comb, 
						  std::string& te_label) {
  std::vector<FeatureIndex> index;

  unsigned int clid = ClassID_traits<Feature_t>::ID();
  std::string clsnm = ClassID_traits<Feature_t>::typeName();
  
  //  (*m_log) << MSG::DEBUG << "Looking for feature with label " << m_label
  //	 << " class: " << clsnm << endreq;

  std::vector<Trig::Feature<Feature_t> > fs = 
    comb.get<Feature_t>(m_label, TrigDefs::alsoDeactivateTEs);
  uint16_t subtypeOfLabel = subTypeIndex(clid, m_label, comb);

  //  (*m_log) << MSG::DEBUG 
  //	 << "  N(feature from Combination::get):" <<  fs.size() << endreq;

  typename std::vector<Trig::Feature<Feature_t> >::const_iterator f;
  for (f=fs.begin(); f!=fs.end(); ++f) {
    const HLT::TriggerElement* te = f->te();
    std::string lbl("?");
    TrigConf::HLTTriggerElement::getLabel(te->getId(), lbl);
//     (*m_log) << MSG::DEBUG
// 	   << "Feature te=" << te 
// 	   << " id=" << te->getId()
// 	   << " name=" << lbl
// 	   << " label=" << f->label()
// 	   << " cptr=" << f->cptr() << endreq;
    if( !checkROIWord( te ) ) { continue; }

    te_label = "";
    TrigConf::HLTTriggerElement::getLabel(te->getId(), te_label);
    fillIndexes(index, te, clid, subtypeOfLabel, m_label, m_log);
  }
  return index;
}

// template<class Feature_t, class Cont_t, class Elem_t>
// std::vector<FeatureIndex> IndexFinder<Feature_t,Cont_t,Elem_t>::findIndexes(const Trig::Combination& comb, 
// 									    MsgStream* logger) {
//   setLogger(logger);
//   return findIndexes(comb);
// }



template<class IndFinder>
std::vector<int> findIndexesForCont(const Trig::Combination& comb, 
				    IndFinder& indexFinder) {
  typedef typename IndFinder::feature_type Feature_t;

  std::vector<Trig::Feature<Feature_t> > fs = 
    comb.get<Feature_t>("", TrigDefs::alsoDeactivateTEs);
  std::vector<int> index;
  
  if (fs.size() > 0) {
    const Feature_t* x = fs[0].cptr(); 
    typename Feature_t::const_iterator p; // Feature is a container
    //     std::cout << "Number of elements in the container feature: " 
    // 	      << x->size() << std::endl;
    for (p=x->begin(); p!=x->end(); ++p) {
      index.push_back(indexFinder.findIndex(*p));
    }
  }
  return index;
}

template<class Feature_t, class IndFinder>
std::vector<int> findIndexesForCont(const Feature_t* cont, 
				    IndFinder& indexFinder) {
  std::vector<int> index;
  
  if (cont) {
    typename Feature_t::const_iterator p; // Feature is a container
    //     std::cout << "Number of elements in the container feature: " 
    // 	      << cont->size() << std::endl;
    for (p=cont->begin(); p!=cont->end(); ++p) {
      index.push_back(indexFinder.findIndex(*p));
    }
  }
  return index;
}


#endif // TRIGGERMENUANALYSIS_INDEXFINDER_H
