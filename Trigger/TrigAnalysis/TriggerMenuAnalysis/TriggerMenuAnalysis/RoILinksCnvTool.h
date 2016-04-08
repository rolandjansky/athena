/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_ROILINKSCNVTOOL_H
#define TRIGGERMENUANALYSIS_ROILINKSCNVTOOL_H
/*
  RoILinksCnvTool.h
*/
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>
#include <set>
#include <string>


class MsgStream;
class HLTObjectsInRoI;


class RoILinksCnvTool : public AthAlgTool {
public:
  static const InterfaceID& interfaceID();

public:
  RoILinksCnvTool(const std::string& type, const std::string& name, 
		  const IInterface* p);
  ~RoILinksCnvTool();

  StatusCode initialize();
  StatusCode beginRun();
  StatusCode finalize();

  void findDefinedChains(const std::vector<std::string>& chains0, 
			 std::vector<std::string>& chains);
  void printListOfTriggers(std::vector<std::string>& chains, 
			   const std::string& type);
  void clear();

  void fill(std::vector<ChainEntry>& chain_entries, RoILinks& roi_links);
  void record(std::vector<ChainEntry>& chain_entries, RoILinks& roi_links);

  int setMuonRoILinks(RoILinks& links, const Trig::Combination& comb);
  int setElectronRoILinks(RoILinks& links, const Trig::Combination& comb);

protected:
  MsgStream& log() const;

  void recordCombLinks(const std::vector<CombLinks>& x, 
		       const std::string& label);

  template<class Elem, class Cont>
  int findIndex(const Elem* p, const std::string& elemKey, 
		const std::string& contKey, bool force_retrieve=false);

  int findHltLevel(const std::string& chain_name) const;
  int fillChains(std::vector<ChainEntry>& chain_entries, RoILinks& roi_links, 
		 const std::string& slice_name, 
		 const std::vector<std::string>& chain_names, 
		 ChainEntry::RoIType l2type, ChainEntry::RoIType eftype);
  int processChain(const std::string& chain_name, 
		   ChainEntry::RoIType roi_type, 
		   std::vector<ChainEntry>& chain_entries, 
		   RoILinks& roi_links);
//   int fillCombLinks(CombLinks& links, 
// 		    const Trig::Combination& comb, 
// 		    ChainEntry::RoIType roitype);
  int findIndexOfL1RoI(const std::string& l1roi, 
		       const HLT::TriggerElement* te, 
		       const std::vector<Muon_ROI>& roilist_mu, 
		       const std::vector<EmTau_ROI>& roilist_emtau, 
		       const std::vector<Jet_ROI>& roilist_jet, 
		       long& roiword);
  void findIndicesOfL1RoI(const std::string& l1roi, 
			  const HLT::TriggerElement* te, 
			  const std::vector<Muon_ROI>& roilist_mu, 
			  const std::vector<EmTau_ROI>& roilist_emtau, 
			  const std::vector<Jet_ROI>& roilist_jet, 
			  std::map<int, long>& roiIndexWord);


  bool hasOtherL1RoI(const std::string& l1roi, 
		     const HLT::TriggerElement* te, 
		     const std::vector<Muon_ROI>& roilist_mu, 
		     const std::vector<EmTau_ROI>& roilist_emtau, 
		     const std::vector<Jet_ROI>& roilist_jet);

  template<class ROI>
  int findIndexOfRoI(const std::vector<const HLT::TriggerElement*>& tes, 
		     const std::vector<ROI>& roilist);
  template<class ROI>
  int findIndexOfRoI(const HLT::TriggerElement* te, 
		     const std::vector<ROI>& roilist);
  template<class ROI>
  int findIndexOfRoI(const HLT::TriggerElement* te, 
		     const std::vector<ROI>& roilist,
		     long& RoIWord);
  template<class ROI>
  void findIndicesOfRoI(const HLT::TriggerElement* te, 
		       const std::vector<ROI>& roilist, 
		       std::map<int, long>& roiIndexWord);
private:
  bool m_saveInactiveCombination;

  std::vector<std::string> m_chains_Muon0;
  std::vector<std::string> m_chains_Electron0;
  std::vector<std::string> m_chains_Tau0;
  std::vector<std::string> m_chains_Jet0;
  std::vector<std::string> m_chains_Bjet0;
  std::vector<std::string> m_chains_MET0;
  std::vector<std::string> m_chains_MinBias0;
  std::vector<std::string> m_chains_Bphysics0;
  std::vector<std::string> m_chains_TileMu0;
  std::vector<std::string> m_chains_Any0;
  
  std::vector<std::string> m_chains_Muon;
  std::vector<std::string> m_chains_Electron;
  std::vector<std::string> m_chains_Tau;
  std::vector<std::string> m_chains_Jet;
  std::vector<std::string> m_chains_Bjet;
  std::vector<std::string> m_chains_MET;
  std::vector<std::string> m_chains_MinBias;
  std::vector<std::string> m_chains_Bphysics;
  std::vector<std::string> m_chains_TileMu;
  std::vector<std::string> m_chains_Any;

  std::vector<std::string> m_chainsToSaveAllCombinations;
  std::set<std::string> m_chainsAllComb;
  
  ServiceHandle<TrigConf::ITrigConfigSvc> m_trigConfSvc;
  ToolHandle<TrigAccessTool> m_trigAccessTool;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  Trig::ExpertMethods* m_expertMethods;

  std::vector<std::string> m_chainEntryKeys;
  std::vector<std::string> m_combLinksKeys;

  RoILinks m_RoILinks;
  std::vector<HLTObjectsInRoI*> m_muonRoIVec;
  std::vector<HLTObjectsInRoI*> m_electronRoIVec;

};

// template<>
// int RoILinksCnvTool::findIndexOfRoI<EnergySum_ROI>(const std::vector<const HLT::TriggerElement*>& tes, 
// 						   const std::vector<EnergySum_ROI>& roilist);

template<class Elem, class Cont>
int RoILinksCnvTool::findIndex(const Elem* p, const std::string& /*elemKey*/, 
			       const std::string& contKey, 
			       bool force_retrieve) {
  int index=-1;
  static const Cont* cont=0;
  bool foundit=false;

  if (force_retrieve || cont==0) {
    if (evtStore()->retrieve(cont, contKey).isFailure()) {
      log() << MSG::WARNING << "Cannot retrieve Container " << contKey << endreq;
    }
  }

  if (cont) {
    if (msgLvl(MSG::DEBUG) ) {
      log() << MSG::DEBUG << "Search element with pointer "
	    << std::hex << p << std::dec 
	    << " : container size: " << cont->size() << endreq;
    }
    index = -1;
    typename Cont::const_iterator q;
    for (q=cont->begin(); q!=cont->end(); ++q) {
      index ++;
      if (p == (*q)) {
	if (msgLvl(MSG::DEBUG) ) {
	  log() << MSG::DEBUG 
		<< "Found element in the container with the same pointer" 
		<< endreq;
	}
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

template<class ROI>
int RoILinksCnvTool::findIndexOfRoI(const std::vector<const HLT::TriggerElement*>& tes, 
				    const std::vector<ROI>& roilist) {
  int index(-1);
  bool foundit=false;

  std::vector<const HLT::TriggerElement*>::const_iterator p_te;
  typename std::vector<ROI>::const_iterator p_roi;

  for (p_te=tes.begin(); p_te!=tes.end(); ++p_te) {
    const std::vector<Trig::Feature<ROI> > rois = 
      m_trigDecisionTool->ancestors<ROI>(*p_te, "initialRoI", TrigDefs::alsoDeactivateTEs);
    if (rois.size() == 0) continue;
    const Trig::Feature<ROI>& roi = rois.front();
    if (roi.cptr()) {
      index = 0;
      foundit = false;
      for (p_roi=roilist.begin(); p_roi!=roilist.end(); ++p_roi) {
	if (p_roi->getROIWord() == roi.cptr()->getROIWord()) {
	  foundit = true;
	  break;
	} else {
	  ++index;
	}
      }
      if (foundit) {
	// GOOD if there is one RoI of this type in the Combination
	return index;
	break;
      }
    }
  }
  return -1;
}


template<class ROI>
int RoILinksCnvTool::findIndexOfRoI(const HLT::TriggerElement* te, 
				    const std::vector<ROI>& roilist) {
  typename std::vector<ROI>::const_iterator p_roi;
  const std::vector<Trig::Feature<ROI> > rois = m_trigDecisionTool->ancestors<ROI>(te, "initialRoI", TrigDefs::alsoDeactivateTEs);
  if (rois.size() == 0) return -1;
  const Trig::Feature<ROI>& roi = rois.front();
  if (roi.cptr()) {
    int index = 0;
    for (p_roi=roilist.begin(); p_roi!=roilist.end(); ++p_roi) {
      if (p_roi->getROIWord() == roi.cptr()->getROIWord()) {
	return index;
	} else {
	++index;
      }
    }
  }
  return -1;
}

template<class ROI>
int RoILinksCnvTool::findIndexOfRoI(const HLT::TriggerElement* te, 
				    const std::vector<ROI>& roilist,
				    long& RoIWord) {
  typename std::vector<ROI>::const_iterator p_roi;
  const std::vector<Trig::Feature<ROI> > rois = m_trigDecisionTool->ancestors<ROI>(te, "initialRoI", TrigDefs::alsoDeactivateTEs);
  if (rois.size() == 0) return -1;
  const Trig::Feature<ROI>& roi = rois.front();
  if (roi.cptr()) {
    int index = 0;
    for (p_roi=roilist.begin(); p_roi!=roilist.end(); ++p_roi) {
      if (p_roi->getROIWord() == roi.cptr()->getROIWord()) {
	RoIWord = p_roi->getROIWord();
	return index;
      } else {
	++index;
      }
    }
  }
  return -1;
}

template<class ROI>
void RoILinksCnvTool::findIndicesOfRoI(const HLT::TriggerElement* te, 
				       const std::vector<ROI>& roilist, 
				       std::map<int, long>& roiIndexWord) {
  typename std::vector<ROI>::const_iterator p_roi;
  const std::vector<Trig::Feature<ROI> > rois = m_trigDecisionTool->ancestors<ROI>(te, "initialRoI", TrigDefs::alsoDeactivateTEs);
  typename std::vector<Trig::Feature<ROI> >::const_iterator p;

  for (p=rois.begin(); p!=rois.end(); ++p) {
    const Trig::Feature<ROI>& roi = *p;
    if (roi.cptr()) {
      int index = 0;
      for (p_roi=roilist.begin(); p_roi!=roilist.end(); ++p_roi, ++index) {
	if (p_roi->getROIWord() == roi.cptr()->getROIWord()) {
	  roiIndexWord[index] = p_roi->getROIWord();
	}
      }
    }
  }
}

#endif // TRIGGERMENUANALYSIS_ROILINKSCNVTOOL_H
