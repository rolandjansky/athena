/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  HltNaviChecker.cxx
*/
#include <vector>
#include "TrigDecisionTool/Combination.h"
#include "TrigNavigation/NavigationCore.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuAnalysis/HltNaviChecker.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjet.h"
#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

using namespace std;


HltNaviChecker::HltNaviChecker(const std::string& name, ISvcLocator* svcloc) :
  AthAlgorithm(name, svcloc), 
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"), 
  m_RoILinksCnvTool("RoILinksCnvTool/RoILinksCnvTool"), 
  m_chainNames(), 
  m_file(0), m_tree(0), m_NPassBits(0), m_EFElectronPassBitsIndex(0) {
  declareProperty("ChainNames", m_chainNames, "List of chains to inspect");
}

HltNaviChecker::~HltNaviChecker() {
}

StatusCode HltNaviChecker::initialize() {
  AthAlgorithm::initialize().ignore();

  if (m_trigDecisionTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve TrigDecisionTool" << endreq;
  }
  if (m_RoILinksCnvTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve RoILinksCnvTool" << endreq;
  }

  m_file = TFile::Open("chk_passbit.root", "RECREATE");
  m_tree = new TTree("t", "Tree to check TrigPassBits");
  m_EFElectronPassBitsIndex = new std::vector<unsigned int>();
  m_tree->Branch("NPassBits", &m_NPassBits, "NPassBits/I");
  m_tree->Branch("EFElectronPassBitsIndex", "std::vector<unsigned int>", 
		&m_EFElectronPassBitsIndex);

  return StatusCode::SUCCESS;
}

StatusCode HltNaviChecker::beginRun() {
  return m_RoILinksCnvTool->beginRun();
}

StatusCode HltNaviChecker::execute() {
  std::vector<ChainEntry> chains;
  RoILinks roi_links;
  static int ievent(0);

  msg(MSG::INFO) << "Event " << ievent << endreq;
  checkPassBits();

  m_RoILinksCnvTool->fill(chains, roi_links);

  std::vector<std::string>::const_iterator p_chain;
  for (p_chain=m_chainNames.begin(); p_chain!=m_chainNames.end(); ++p_chain) {
    msg(MSG::INFO) << "Chain decision for " << (*p_chain) 
		   << " ==> " << m_trigDecisionTool->isPassed(*p_chain) 
		   << endreq;
    checkFeatures(*p_chain);
  }


  ievent ++;
  return StatusCode::SUCCESS;
}

void HltNaviChecker::checkPassBits() {
  const TrigPassBitsCollection* coll(0);
  const DataHandle<TrigPassBitsCollection> coll1, coll2;

  if (evtStore()->retrieve(coll1, coll2).isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve TrigPassBitsCollection" << endreq;
  }
  int n(0);
  for (; coll1!=coll2; ++coll1) {
    n ++;
  }
  msg(MSG::INFO) << "N TrigPassBits collections: " << n << endreq;

  if (evtStore()->retrieve(coll, "HLT_passbits").isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve TrigPassBitsCollection" << endreq;
  }

  // Reset data
  m_NPassBits = 0;
  m_EFElectronPassBitsIndex->clear();

  if (coll) {
    m_NPassBits = coll->size();
  }
  const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup("EF_.*");
  if (cg) {
    const Trig::FeatureContainer fc = cg->features();
    const std::vector<Trig::Feature<egamma> > v = fc.get<egamma>();
    std::vector<Trig::Feature<egamma> >::const_iterator p;

    for (p=v.begin(); p!=v.end(); ++p) {
      const HLT::TriggerElement* te = p->te();
      const HLT::TriggerElement::FeatureVec& fv = te->getFeatureAccessHelpers();
      HLT::TriggerElement::FeatureVec::const_iterator q;
      for (q=fv.begin(); q!=fv.end(); ++q) {
	if (q->getCLID() != ClassID_traits<egammaContainer>::ID()) continue;
	const HLT::TriggerElement::ObjectIndex& oi = q->getIndex();
	msg(MSG::INFO) << "Feature clid=" << q->getCLID()
		       << " " << oi << endreq;
	for (unsigned int i=oi.objectsBegin(); i!=oi.objectsEnd(); ++i) {
	  m_EFElectronPassBitsIndex->push_back(i);
	}
      }
    }
  }
  m_tree->Fill();
}

StatusCode HltNaviChecker::finalize() {
  AthAlgorithm::finalize().ignore();
  if (m_file) {
    m_file->Write();
    m_file->Close();
    delete m_file;
    m_tree = 0;
    m_file = 0;
  }
  return StatusCode::SUCCESS;
}

template<class T>
void getAndDumpFeatures(Trig::FeatureContainer& fc, const std::string& cls, 
			MsgStream& log) {
  vector<Trig::Feature<T> > v = fc.get<T>("", TrigDefs::alsoDeactivateTEs);
  log << cls << " N=" << v.size() << endreq;
  for (unsigned int i=0; i<v.size(); ++i) {
    Trig::Feature<T>& x = v[i];
    log << "  [" << i << "] cptr=" << x.cptr() << " TE=" << x.te() 
	<< " label=" << x.label() << endreq;
  }
}

void HltNaviChecker::checkFeatures(const std::string& chain_name) {
  msg(MSG::INFO) << "*** Check HLT features in chain: " << chain_name << endreq;
  Trig::FeatureContainer fc = 
    m_trigDecisionTool->features(chain_name, TrigDefs::alsoDeactivateTEs);

  std::vector<Trig::Combination> combs = fc.getCombinations();
  std::vector<Trig::Combination>::const_iterator p_comb;
  std::vector<const HLT::TriggerElement*>::const_iterator p_te;
  msg(MSG::INFO) << "  Loop over combinations and look for L1 RoIs" << endreq;
  msg(MSG::INFO) << "  N combinations: " << combs.size() << endreq;
  int icomb=0;
  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb, ++icomb) {
    HLT::NavigationCore* navi = dynamic_cast<HLT::NavigationCore*>(p_comb->navigation());
    const std::vector<const HLT::TriggerElement*>& tes = p_comb->tes();
    msg(MSG::INFO) << "  Combinatation[" << icomb 
		   << "] N TEs: " << tes.size()
		   << " active=" << p_comb->active()
		   << endreq;
    int ite(0);

    for (p_te=tes.begin(); p_te!=tes.end(); ++p_te, ++ite) {
      msg(MSG::INFO) << "    TE[" << ite << "] RoI via TDT ancestor" << endreq;

      const vector<Trig::Feature<Jet_ROI> > xv1 = m_trigDecisionTool->ancestors<Jet_ROI>(*p_te);
      const vector<Trig::Feature<Muon_ROI> > xv2 = m_trigDecisionTool->ancestors<Muon_ROI>(*p_te);
      msg(MSG::INFO) << "      N jet RoIs by ancestors(): " << xv1.size() << endreq;
      for (unsigned int j=0; j<xv1.size(); ++j) {
	msg(MSG::INFO) << "      Jet_ROI[" << j << "] cptr=" << xv1[j].cptr() 
		       << " TE=" << xv1[j].te() 
		       << " label=" << xv1[j].label() << endreq;
      }
      msg(MSG::INFO) << "    N muon RoIs by ancestors(): " << xv2.size() << endreq;
      for (unsigned int j=0; j<xv2.size(); ++j) {
	msg(MSG::INFO) << "      Muon_ROI[" << j << "] cptr=" << xv2[j].cptr() 
		       << " TE=" << xv2[j].te() 
		       << " label=" << xv2[j].label() << endreq;
      }

      checkFeatures(*p_te, navi, "      ");
      //       const Trig::Feature<Jet_ROI> x = 
      // 	m_trigDecisionTool->ancestor<Jet_ROI>(*p_te);
      //       msg(MSG::INFO) << "    Jet_ROI via ancestor() cptr=" << x.cptr() << " TE=" << x.te() 
      // 		     << " label=" << x.label() << endreq;
      //       const Trig::Feature<Muon_ROI> x2 = 
      // 	m_trigDecisionTool->ancestor<Muon_ROI>(*p_te);
      //       msg(MSG::INFO) << "    Muon_ROI via ancestor() cptr=" << x2.cptr() << " TE=" << x2.te() 
      // 		     << " label=" << x2.label() << endreq;
    }
  }
  // Show direct dump for some classes
  getAndDumpFeatures<TrigRoiDescriptor>(fc, "TrigRoiDescriptor", msg(MSG::INFO));
  getAndDumpFeatures<MuonFeature>(fc, "MuonFeature", msg(MSG::INFO));
  getAndDumpFeatures<TrigL2Bjet>(fc, "TrigL2Bjet", msg(MSG::INFO));
  getAndDumpFeatures<TrigEFBjet>(fc, "TrigEFBjet", msg(MSG::INFO));
  getAndDumpFeatures<TrigInDetTrackCollection>(fc, "TrigInDetTrackCollection", msg(MSG::INFO));
  getAndDumpFeatures<TrigL2BjetContainer>(fc, "TrigL2BjetContainer", msg(MSG::INFO));
  getAndDumpFeatures<TrigEFBjetContainer>(fc, "TrigEFBjetConatiner", msg(MSG::INFO));
  getAndDumpFeatures<TrigVertex>(fc, "TrigVertex", msg(MSG::INFO));
  getAndDumpFeatures<TrigVertexCollection>(fc, "TrigVertexCollection", msg(MSG::INFO));
  getAndDumpFeatures<VxContainer>(fc, "VxContainer", msg(MSG::INFO));
  getAndDumpFeatures<TrigMuonEFInfoContainer>(fc, "TrigMuonEFInfoConatiner", msg(MSG::INFO));
  getAndDumpFeatures<TrigMuonEFInfo>(fc, "TrigMuonEFInfo", msg(MSG::INFO));
  getAndDumpFeatures<Jet_ROI>(fc, "Jet_ROI", msg(MSG::INFO));

}

void HltNaviChecker::checkFeatures(const HLT::TriggerElement* te, 
				   HLT::NavigationCore* navi, 
				   const std::string& prefix) {
  // Check all TEs in this TE
  std::string label;
  TrigConf::HLTTriggerElement::getLabel(te->getId(), label);
  bool status = te->getActiveState();
  const HLT::TriggerElement::FeatureVec& v = te->getFeatureAccessHelpers();
  HLT::TriggerElement::FeatureVec::const_iterator p_f;

  msg(MSG::INFO) << prefix << "TE: " << label << " status=" << status
		 << " feature size: " << v.size()
		 << endreq;

  int iii=0;
  for (p_f=v.begin(); p_f!=v.end(); ++p_f, ++iii) {
    unsigned int clid = p_f->getCLID();
    const HLT::TriggerElement::ObjectIndex& oi = p_f->getIndex();
    std::string feature_label = navi->label(clid, oi.subTypeIndex());
    msg(MSG::INFO) << prefix << "  Feature[" << iii << "] :" 
		   << " clid = " << p_f->getCLID()
		   << " subType: " << oi.subTypeIndex() 
		   << " label: " << feature_label
		   << " index=[" << oi.objectsBegin() 
		   << "," << oi.objectsEnd() << ")" << endreq;
  }
  const std::vector<HLT::TriggerElement*>& tes = navi->getDirectPredecessors(te);
  std::vector<HLT::TriggerElement*>::const_iterator p;
  for (p=tes.begin(); p!=tes.end(); ++p) {
    checkFeatures(*p, navi, prefix+"  ");
  }
}

